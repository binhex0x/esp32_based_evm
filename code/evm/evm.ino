/*
   Project: Electronic Voting Machine (EVM)
   Description: This program implements an Electronic Voting Machine using an ESP32 microcontroller,
   TFT display, keypad, EEPROM, and a shift register.

   Features:
   - User interface for voting and managing candidates
   - Persistent storage of votes and candidate statuses using EEPROM
   - Real-time display of voting results
   - Control of candidate enable/disable status
   - Power management using a 18650 lithium-ion battery and MT3608 boost converter

   Components Used:
   - ESP32 Microcontroller
   - TFT Display (using TFT_eSPI library)
   - Keypad (using Keypad library)
   - EEPROM for data storage
   - Shift Register (74HC595) for LED control
   - Passive Buzzer for audio feedback

   Author: [YOGESH KUMAR]
   Date:
   Version: 1.0
*/


#include <Arduino.h>
#include "img.h"
#include <TFT_eSPI.h>
#include <Keypad.h>
#include <EEPROM.h>

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT screen
const int latchPin = 26;    // Latch pin for shift register
const int clockPin = 25;    // Clock pin for shift register
const int dataPin = 27;     // Data pin for shift register

// Pin configuration for keypad
const byte ROW_NUM = 4;     // four rows
const byte COLUMN_NUM = 3;  // three columns
char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte pin_rows[ROW_NUM] = { 12, 14, 19, 22 };  //Rows

byte pin_column[COLUMN_NUM] = { 17, 16, 4 };  // Columns

Keypad customKeypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);


// Candidate votes and enabled status
int votes[6] = { 0, 0, 0, 0, 0, 0 };                                 // Store votes for each candidate
bool enabledCandidates[6] = { true, true, true, true, true, true };  // Candidate enable/disable
int maxVoters = 100;
const int default_maxVoters = 100;  // Default maximum voters

#define EEPROM_SIZE 512
const int buzzer_pin = 32;

// Function prototypes
void showAdminMenu();
void showVotingMenu();
void showResetMenu();
void enableCandidates();
void showResults();
void setMaxVoters();
void resetEEPROM();

// Password function prototype
void showPasswordBox(void (*callbackFunction)());


void setup() {

  EEPROM.begin(EEPROM_SIZE);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(buzzer_pin, OUTPUT);
  Serial.begin(115200);
  tft.init();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  delay(500);
  tft.fillRect(0, 0, 160, 20, TFT_BLACK);

  tft.pushImage(0, 0, 160, 128, college_logo);
  init_tone();
  delay(2000);
  tft.pushImage(0, 0, 160, 128, yogesh);
  delay(2000);

  knightRiderEffect(150);


  updateLEDs();
  // Read max voters and candidates' enabled status from EEPROM
  maxVoters = readIntFromEEPROM(200);  // 200 and 201


  // 100 to 105 condidate en/di
  for (int i = 0; i < 6; i++) {
    enabledCandidates[i] = EEPROM.read(i + 100);

    votes[i] = EEPROM.read(i + 300);
    Serial.println("candidates " + (String)i + " status and votes");
    Serial.println(enabledCandidates[i]);
    Serial.println(votes[i]);
  }


  all_votes_count();

  maxVoters = readIntFromEEPROM(200);
  Serial.print("max vote count : ");
  Serial.println(maxVoters);


  // Display initial menu
  showResetMenu();
}

void loop() {

  // Main loop for keypad reading and menu navigation
  char key = getDebouncedKey();  // Call the debounce function
  if (key) {
    if (key == '1') {
      showPasswordBox(showAdminMenu);
    } else if (key == '2') {
      showPasswordBox(showVotingMenu);
    } else if (key == '*') {
      showResetMenu();
    }
  }
}


void showResetMenu() {
  // Clear the screen and set a black background
  tft.fillScreen(TFT_BLACK);

  // Create a solid background color for the entire menu area
  tft.fillRect(0, 0, 160, 30, TFT_BLACK);  // Header block
  tft.drawRect(0, 0, 160, 30, TFT_WHITE);  // Border around the header

  // Set the title text in the header
  tft.setCursor(25, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("Main Menu");

  // Draw a separator line for visual structure
  tft.drawLine(0, 30, 160, 30, TFT_WHITE);

  // Create colored background blocks for each option
  tft.fillRect(15, 40, 130, 25, TFT_GREEN);  // Admin Mode background
  tft.drawRect(15, 40, 130, 25, TFT_WHITE);  // Border for Admin Mode
  tft.setCursor(20, 45);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.println("1: Admin Mode");

  tft.fillRect(15, 70, 130, 25, TFT_GREEN);  // Voting Mode background
  tft.drawRect(15, 70, 130, 25, TFT_WHITE);  // Border for Voting Mode
  tft.setCursor(20, 75);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.println("2: Voting Mode");

  //tft.fillRect(15, 100, 130, 25, TFT_GREEN); // Select background
  //tft.drawRect(15, 100, 130, 25, TFT_WHITE);  // Border for Select option
  tft.setCursor(10, 105);
  tft.setTextColor(TFT_DARKGREY);
  tft.setTextSize(1);
  tft.println("*: Select   #: Back/Clr");

  // Set default text color for all other functions
  tft.setTextColor(TFT_BLACK);

  // Interaction loop with keypad input
  bool done = false;
  while (!done) {
    char key = getDebouncedKey();  // Call the debounce function

    if (key == '1') {
      // Add button effect when selecting 'Admin Mode'
      tft.fillRect(15, 40, 130, 25, TFT_WHITE);  // Highlight selected option
      tft.setCursor(20, 45);
      tft.setTextColor(TFT_BLACK);
      tft.setTextSize(1);
      tft.println("1: Admin Mode");
      delay(200);                      // Brief pause for effect
      showPasswordBox(showAdminMenu);  // Show password box before Admin Mode
      done = true;
    } else if (key == '2') {
      // Add button effect when selecting 'Voting Mode'
      tft.fillRect(15, 70, 130, 25, TFT_WHITE);  // Highlight selected option
      tft.setCursor(20, 75);
      tft.setTextColor(TFT_BLACK);
      tft.setTextSize(1);
      tft.println("2: Voting Mode");
      delay(200);                       // Brief pause for effect
      showPasswordBox(showVotingMenu);  // Show password box before Voting Mode
      done = true;
    } else if (key == '*') {
      // Add button effect when selecting 'Select'
      tft.fillRect(15, 100, 130, 25, TFT_WHITE);  // Highlight selected option
      tft.setCursor(20, 105);
      tft.setTextColor(TFT_BLACK);
      tft.setTextSize(1);
      tft.println("*: Select");
      delay(200);   // Brief pause for effect
      done = true;  // Exit the menu
    }
  }
  tft.setTextColor(TFT_WHITE);
}


void showAdminMenu() {
  tft.setTextColor(TFT_WHITE);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(2, 5);
  tft.setTextColor(TFT_GREEN);

  tft.println("Admin Mode:");
  tft.setTextColor(TFT_WHITE);

  tft.setCursor(2, 25);
  tft.println("1: Enable Candidates");
  tft.setCursor(2, 45);
  tft.println("2: Show Results");
  tft.setCursor(2, 65);
  tft.println("3: Set Max Voters");
  tft.setCursor(2, 85);
  tft.println("4: Reset EEPROM");
  tft.setCursor(2, 105);
  tft.println("#: Exit");

  bool done = false;
  while (!done) {
    char key = getDebouncedKey();  // Call the debounce function
    if (key == '1') {
      enableCandidates();
    } else if (key == '2') {
      showResults();
    } else if (key == '3') {
      setMaxVoters();
    } else if (key == '4') {
      showPasswordBox(resetEEPROM);
    } else if (key == '#') {
      done = true;
    }
  }
  showResetMenu();
}



void enableCandidates() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(2, 5);
  tft.println("Enable Candidates:");
  tft.setCursor(2, 25);
  tft.println("1-6: Toggle candidates");
  tft.setCursor(2, 45);
  tft.println("#: Exit");
  tft.setTextColor(TFT_RED);
  tft.setCursor(2, 65);
  tft.println("Please Reset the board ");
  tft.setCursor(2, 75);
  tft.println("after all selection ");
  tft.setTextColor(TFT_WHITE);



  bool done = false;
  while (!done) {
    char key = getDebouncedKey();  // Call the debounce function
    if (key >= '1' && key <= '6') {
      int candidateIndex = key - '1';                                          // Convert char to index
      enabledCandidates[candidateIndex] = !enabledCandidates[candidateIndex];  // Toggle candidate state

      updateLEDs();

      // Save the updated state to EEPROM
      // Store 1 for enabled and 0 for disabled
      EEPROM.write(100 + candidateIndex, enabledCandidates[candidateIndex] ? 1 : 0);
      EEPROM.commit();  // Commit changes to EEPROM

      // Update display
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(2, 5);
      tft.println("Enable Candidates:");
      tft.setCursor(2, 25);
      tft.println("1-6: Toggle candidates");
      tft.setCursor(2, 45);
      tft.println("#: Exit");
      tft.setCursor(2, 65);
      tft.print("Candidate ");
      tft.print(candidateIndex + 1);
      tft.println(enabledCandidates[candidateIndex] ? " Enabled" : " Disabled");
      updateLEDs();
    } else if (key == '#') {
      done = true;  // Exit on '#'
    }
  }
  showAdminMenu();  // Return to Admin menu after exiting
}


void showResults() {
  tft.fillScreen(TFT_BLACK);

  // Display header
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 5);
  tft.println("EVM Results");

  // Divider line under header
  tft.drawLine(0, 20, 160, 20, TFT_CYAN);

  // Variables for results
  int maxVotes = 0;
  int winner = -1;
  int totalVotes = readIntFromEEPROM(200);  // Total eligible votes
  int totalVotesCast = 0;                   // Count of all votes cast

  // Determine max votes and total votes cast
  for (int i = 0; i < 6; i++) {
    int candidateVotes = readIntFromEEPROM(300 + (i * 2));
    totalVotesCast += candidateVotes;
    if (candidateVotes > maxVotes) {
      maxVotes = candidateVotes;
      winner = i;
    }
  }

  // Check for ties
  bool tie = false;
  for (int i = 0; i < 6; i++) {
    if (readIntFromEEPROM(300 + (i * 2)) == maxVotes && i != winner) {
      tie = true;
    }
  }

  // Display results for candidates
  tft.setTextSize(1);

  // Left column: C1, C2, C3
  for (int i = 0; i < 3; i++) {
    tft.setCursor(2, 30 + (i * 20));  // Adjust Y position for each candidate
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("C");
    tft.print(i + 1);
    tft.print(": ");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print(readIntFromEEPROM(300 + (i * 2)));  // Show votes for C1, C2, C3
  }

  // Right column: C4, C5, C6
  for (int i = 3; i < 6; i++) {
    tft.setCursor(80, 30 + ((i - 3) * 20));  // Adjust X and Y for right side
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("C");
    tft.print(i + 1);
    tft.print(": ");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print(readIntFromEEPROM(300 + (i * 2)));  // Show votes for C4, C5, C6
  }

  // Bottom section
  tft.drawLine(0, 90, 160, 90, TFT_CYAN);  // Divider above bottom section

  // Voting percentage
  tft.setCursor(2, 95);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.print("Votes: ");
  tft.print(totalVotesCast);
  tft.print("/");
  tft.print(totalVotes);
  tft.print(" (");
  tft.print((totalVotesCast * 100) / totalVotes);
  tft.print("%)");

  // Winner
  tft.setCursor(2, 110);
  if (tie) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.print("Winners: ");
    for (int i = 0; i < 6; i++) {
      if (readIntFromEEPROM(300 + (i * 2)) == maxVotes) {
        tft.print("C");
        tft.print(i + 1);
        tft.print(" ");
      }
    }
  } else {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("Winner: C");
    tft.print(winner + 1);
  }

  // Instructions for navigation
  tft.setCursor(2, 120);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("Press * for Graph");
  tft.println("Press # to Exit");

  // Wait for user input
  while (true) {
    char key = getDebouncedKey();  // Call the debounce function
    if (key == '*') {
      showGraph(80);  // Show graph
      return;
    } else if (key == '#') {
      return;  // Exit to main menu
    }
  }
}



void showGraph(int graphBaseY) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(2, 5);
  tft.println("Vote Graph:");

  int totalVotes = readIntFromEEPROM(200);  // Max voters
  int barWidth = 20;                        // Width of each bar
  int barSpacing = 5;                       // Spacing between bars
  int startX = 10;

  // Adjust maximum height dynamically based on `graphBaseY`
  int maxHeight = graphBaseY - 10;  // Maximum height for the bars, keeping some margin at the top

  // Draw horizontal grid lines
  int gridLines = 4;  // Number of grid lines
  for (int i = 0; i <= gridLines; i++) {
    int y = graphBaseY - (i * maxHeight / gridLines);
    tft.drawLine(2, y, 158, y, TFT_DARKGREY);
  }

  // Find the maximum votes for scaling the graph
  int maxVotes = 0;
  for (int i = 0; i < 6; i++) {
    int candidateVotes = readIntFromEEPROM(300 + (i * 2));
    if (candidateVotes > maxVotes) maxVotes = candidateVotes;
  }

  // Draw bars for candidates
  for (int i = 0; i < 6; i++) {
    int candidateVotes = readIntFromEEPROM(300 + (i * 2));
    int barHeight = (maxVotes == 0) ? 0 : (candidateVotes * maxHeight / maxVotes);

    int x1 = startX + i * (barWidth + barSpacing);
    int y1 = graphBaseY - barHeight;

    // Highlight winner bars in green
    bool isWinner = (candidateVotes == maxVotes && maxVotes > 0);
    tft.fillRect(x1, y1, barWidth, barHeight, isWinner ? TFT_GREEN : TFT_BLUE);
    tft.drawRect(x1, y1, barWidth, barHeight, TFT_WHITE);  // Outline

    // Display candidate labels below the bars
    tft.setCursor(x1, graphBaseY + 5);
    tft.print("C");
    tft.print(i + 1);
  }

  tft.setCursor(2, 5);
  tft.println("Vote Graph:");

  // Display return option
  tft.setCursor(2, 115);
  tft.print("Press # to Return");

  // Handle navigation
  while (true) {
    char key = getDebouncedKey();  // Call the debounce function
    if (key == '#') {
      showResults();  // Return to results summary
      return;
    }
  }
}


void setMaxVoters() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(2, 5);
  int mvx = readIntFromEEPROM(200);  // Read current max voters from EEPROM
  tft.printf("Saved Max Voters: %d\n", mvx);
  tft.setCursor(2, 25);
  tft.println("Set Max Voters:");
  tft.setCursor(2, 45);
  tft.println("Enter max voters:");

  int tempMaxVoters = 0;
  bool done = false;

  while (!done) {
    char key = getDebouncedKey();  // Call the debounce function
    if (key >= '0' && key <= '9') {
      // Build the number from digits
      tempMaxVoters = tempMaxVoters * 10 + (key - '0');

      // Clear previous display line
      tft.fillRect(2, 65, tft.width() - 4, 20, TFT_BLACK);  // Clear area for new number
      tft.setCursor(2, 65);

      // Display current input value
      if (tempMaxVoters > 0 && tempMaxVoters <= 65535) {
        tft.print(tempMaxVoters);
      } else {
        // If out of range, show max limit
        tft.print("65535");
      }
    } else if (key == '#') {
      // Finalize the input when '#' is pressed
      if (tempMaxVoters >= 1 && tempMaxVoters <= 65535) {
        maxVoters = tempMaxVoters;  // Set max voters to input value
      } else {
        maxVoters = 65535;  // Default to maximum allowable value
      }

      writeIntToEEPROM(200, maxVoters);  // Write new max voters to EEPROM

      // Show saved confirmation
      tft.fillScreen(TFT_BLACK);  // Clear screen for confirmation
      tft.setCursor(2, 85);
      tft.println("Saved!");
      delay(2000);  // Wait before returning
      done = true;  // Exit loop
    }
  }
  showAdminMenu();  // Return to Admin menu after exiting
}



void resetEEPROM() {

  for (int i = 0; i < 6; i++) {
    EEPROM.write(i + 100, 1);            // Reset enabled candidates to true
    votes[i] = 0;                        // Reset votes
    writeIntToEEPROM(300 + (i * 2), 0);  // set votes to 0
  }
  EEPROM.commit();

  writeIntToEEPROM(200, default_maxVoters);  // set default max voter value
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, 40);
  tft.println("EEPROM");
  tft.setCursor(40, 60);

  tft.println("Reset!");

  eep_rst_tone();  // eep rst music

  delay(2000);
  tft.setTextSize(1);

  showAdminMenu();
}


void showVotingMenu() {
  updateLEDs();  // Update LEDs based on candidate status

  // Initial screen setup
  tft.fillScreen(TFT_YELLOW);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(10, 5);
  tft.println("Voting Booth");

  // Draw decorative header line
  tft.drawLine(0, 22, 160, 22, TFT_RED);

  // Display instructions
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.setTextColor(TFT_BLUE);
  tft.println("1-6: Vote for candidates");
  tft.setCursor(10, 50);
  tft.println("*: Unlock voting");

  bool done = false;
  bool inputLocked = false;  // Lock input after one vote

  while (!done) {
    char key = getDebouncedKey();      // Get debounced key input
    int avc = all_votes_count();       // Get total votes
    int mvc = readIntFromEEPROM(200);  // Get max voters allowed from EEPROM

    if (key >= '1' && key <= '6' && !inputLocked) {
      int candidateIndex = key - '1';                     // Convert key input to candidate index
      int candidateAddress = 300 + (candidateIndex * 2);  // EEPROM address for the candidate

      // Check if candidate is enabled and total votes are within limit
      if (enabledCandidates[candidateIndex] && (avc < mvc)) {
        // Read previous vote count from EEPROM
        int currentVote = readIntFromEEPROM(candidateAddress);

        // Increment the vote count for this candidate
        currentVote++;

        // Write the updated vote count back to EEPROM
        writeIntToEEPROM(candidateAddress, currentVote);

        // Verify if the EEPROM write was successful
        int savedValue = readIntFromEEPROM(candidateAddress);

        if (savedValue != currentVote) {
          // EEPROM write verification failed
          tft.fillScreen(TFT_RED);
          tft.setTextColor(TFT_WHITE);
          tft.setTextSize(2);
          tft.setCursor(50, 45);
          tft.print("Error");
          tft.setCursor(10, 65);
          tft.print("Vote Not Saved");
          delay(2000);  // Wait for user to see the error
          continue;     // Retry voting process
        }

        // Display confirmation animation
        for (int i = 0; i < 3; i++) {
          tft.fillScreen(i % 2 == 0 ? TFT_GREEN : TFT_BLACK);
          delay(200);
        }
        tft.fillScreen(TFT_GREEN);

        // Show vote confirmation message
        tft.setTextSize(3);
        tft.setTextColor(TFT_BLACK);
        tft.setCursor(40, 30);
        tft.print("VOTE");
        tft.setCursor(10, 60);
        tft.print("RECORDED");
        buzz();

        // Display candidate information
        tft.setTextSize(2);
        tft.setTextColor(TFT_RED);
        tft.fillScreen(TFT_YELLOW);
        tft.setCursor(25, 20);
        tft.print("Saved for");
        if ((candidateIndex + 1) == 6) {
          tft.setTextSize(3);
          tft.setCursor(50, 45);
          tft.print("NOTA");  // None of the Above
        } else {
          tft.setTextSize(3);
          tft.setCursor(60, 45);
          tft.print("C");
          tft.print(candidateIndex + 1);  // Candidate number
        }

        delay(2000);  // Allow time for confirmation display

        inputLocked = true;  // Lock input after one vote
        voting_screen();     // Return to voting screen
        tft.setCursor(10, 70);
        tft.setTextColor(TFT_DARKGREY);
        tft.println("Press '*' to unlock");
      } else {
        // Error handling for invalid conditions
        tft.fillScreen(TFT_RED);
        tft.setTextSize(2);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(20, 40);
        if (!enabledCandidates[candidateIndex]) {
          tft.println("CANDIDATE");
          tft.setCursor(20, 70);
          tft.println("DISABLED!");
        } else {
          tft.println("VOTE BOX");
          tft.setCursor(10, 70);
          tft.println("IS FULL!");
        }
        delay(2000);
        voting_screen();  // Return to voting screen
      }
    } else if (key == '*') {
      // Unlock input if '*' is pressed
      inputLocked = false;
      voting_screen();
      tft.setCursor(10, 70);
      tft.setTextColor(TFT_DARKGREY);
      tft.println("Press '*' to unlock");
    }
  }
}




// Function to write a 16-bit integer to EEPROM
void writeIntToEEPROM(int address, int value) {
  EEPROM.write(address, value & 0xFF);      // Lower byte
  EEPROM.write(address + 1, (value >> 8));  // Upper byte
  EEPROM.commit();                          // Commit changes
  delay(10);
}




// Function to read a 16-bit integer from EEPROM
int readIntFromEEPROM(int address) {
  int lowerByte = EEPROM.read(address);      // Read lower byte
  int upperByte = EEPROM.read(address + 1);  // Read upper byte
  return (upperByte << 8) | lowerByte;       // Combine bytes into a single integer
}

int all_votes_count() {
  int vote_t = 0;  // Initialize vote_t to 0

  for (int i = 0; i < 6; i++) {
    // Read each candidate's votes from EEPROM
    int votes = readIntFromEEPROM(300 + (i * 2));  // Each vote takes 2 bytes
    vote_t += votes;                               // Accumulate votes
  }

  Serial.print("Total votes: ");  // Print total votes
  Serial.println(vote_t);

  return vote_t;  // Return the total votes
}


void updateLEDs() {
  byte ledState = 0;  // Initialize all LEDs to off

  // Read enabled candidates directly from EEPROM and set LED states
  for (int i = 0; i < 6; i++) {
    // Read the enabled candidate status from EEPROM
    bool isEnabled = EEPROM.read(i + 100);  // Read from addresses 100 to 105

    if (isEnabled) {
      ledState |= (1 << i);  // Set the corresponding bit if candidate is enabled
    }
  }


  Serial.println("===led eeprom candidate data====");

  Serial.println(ledState, BIN);

  // Send the ledState to the shift register
  digitalWrite(latchPin, LOW);                      // Prepare to send data
  shiftOut(dataPin, clockPin, MSBFIRST, ledState);  // Shift out the byte to the register
  digitalWrite(latchPin, HIGH);                     // Update the output pins of the shift register
  delay(50);
}

void buzz() {
  tone(buzzer_pin, 1000);  // Play 1000 Hz tone
  delay(1000);             // Wait for 1 second

  noTone(buzzer_pin);  // Stop the tone
  delay(100);          // Wait for half a second
}

void eep_rst_tone() {
  for (int i = 0; i < 10; i++) {
    tone(buzzer_pin, i * 500);  // Play 1000 Hz tone
    delay(80);                  // Wait for 1 second
    noTone(buzzer_pin);         // Stop the tone
    delay(80);                  // Wait for half a second
  }
}

void access_tone() {
  for (int i = 0; i < 3; i++) {
    tone(buzzer_pin, i * 1200);  // Play 1000 Hz tone
    delay(300);                  // Wait for 1 second
    noTone(buzzer_pin);          // Stop the tone
    delay(100);                  // Wait for half a second
  }
}

void init_tone() {
  for (int i = 0; i < 2; i++) {
    tone(buzzer_pin, 2000);  // Play 1000 Hz tone
    delay(500);              // Wait for 1 second
    noTone(buzzer_pin);      // Stop the tone
    delay(200);              // Wait for half a second
  }
  tone(buzzer_pin, 1000);  // Play 1000 Hz tone
  delay(800);              // Wait for 1 second
  noTone(buzzer_pin);      // Stop the tone
  delay(200);
}

void incorrect_pass_tone() {
  for (int i = 0; i < 3; i++) {
    tone(buzzer_pin, 150);  // Play 1000 Hz tone
    delay(500);             // Wait for 1 second
    noTone(buzzer_pin);     // Stop the tone
    delay(100);             // Wait for half a second
  }
}


void voting_screen() {
  // Return to voting screen
  tft.fillScreen(TFT_YELLOW);
  tft.setCursor(10, 5);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  tft.println("Voting Booth");
  tft.drawLine(0, 22, 160, 22, TFT_RED);
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.setTextColor(TFT_BLUE);
  tft.println("1-6: Vote for candidates");
  tft.setCursor(10, 50);
  tft.println("*: Unlock voting");
}



const String correctPassword = "1101";  // Define your correct password here

void showPasswordBox(void (*callbackFunction)()) {
  tft.setTextColor(TFT_WHITE);

  // Draw a rectangle for the password box
  int boxX = 10;
  int boxY = 30;
  int boxWidth = 140;
  int boxHeight = 60;
  tft.fillScreen(TFT_BLACK);

  // Draw green border
  tft.fillRect(boxX, boxY, boxWidth, boxHeight, TFT_BLACK);  // Fill with black
  tft.drawRect(boxX, boxY, boxWidth, boxHeight, TFT_GREEN);  // Draw green border

  tft.setCursor(15, 40);
  tft.println("Enter Password:");

  tft.setCursor(10, 105);
  tft.setTextColor(TFT_DARKGREY);
  tft.setTextSize(1);
  tft.println("*: Select   #: Back/Clr");


  String enteredPassword = "";  // String to hold the entered password
  bool done = false;

  while (!done) {
    char key = getDebouncedKey();  // Call the debounce function  Get key press from keypad

    if (key) {
      if (key == '*') {                       // Use '*' to confirm the entered password
        if (enteredPassword.length() == 4) {  // Check for an exact length of eight digits
          if (enteredPassword == correctPassword) {
            tft.fillScreen(TFT_GREEN);  // Clear screen to indicate success
            tft.setTextColor(TFT_WHITE);

            tft.setCursor(35, 35);
            tft.println("Access Granted!");
            access_tone();
            delay(500);   // Show success message for 2 seconds
            done = true;  // Exit loop

            callbackFunction();  // Call the passed function after successful access
          } else {
            // Clear and show error message for incorrect password
            tft.fillScreen(TFT_RED);
            tft.setCursor(45, 40);
            tft.setTextColor(TFT_WHITE);

            tft.println("Incorrect \n\n        Password!");
            incorrect_pass_tone();
            delay(1000);           // Show error message for 2 seconds
            enteredPassword = "";  // Reset entered password
          }
        } else {
          // Show error message for incorrect length
          tft.fillScreen(TFT_RED);

          tft.setCursor(40, 40);
          tft.setTextColor(TFT_WHITE);
          tft.println("Password must be \n\n         4 digits!\n\n\n         press '#'");
          incorrect_pass_tone();

          delay(1000);
          enteredPassword = "";
        }
      } else if (key == '#') {  // Use '#' to clear the input
        enteredPassword = "";
        tft.fillRect(boxX, boxY, boxWidth, boxHeight, TFT_BLACK);  // Clear password box
        tft.drawRect(boxX, boxY, boxWidth, boxHeight, TFT_GREEN);  // Redraw border
        tft.setCursor(15, 40);
        tft.println("Enter Password:");
      } else {
        // Append key to entered password if it's a digit and length is less than eight
        if (enteredPassword.length() < 8 && key >= '0' && key <= '6') {
          enteredPassword += key;

          // Display masked input as asterisks
          String maskedInput = "";
          for (int i = 0; i < enteredPassword.length(); i++) {
            maskedInput += '*';
          }

          tft.setCursor(15, 60);
          tft.print("Entered: ");
          tft.print(maskedInput);
        }
      }
    }
  }
}



void knightRiderEffect(int del) {
  for (int i = 0; i < 6; i++) {                     // Move LED from left to right
    digitalWrite(latchPin, LOW);                    // Begin data transfer
    shiftOut(dataPin, clockPin, MSBFIRST, 1 << i);  // Shift out the current LED pattern
    digitalWrite(latchPin, HIGH);                   // Latch the data
    tone(buzzer_pin, 1000 + (i * 200));
    delay(del);  // Delay for visibility
    noTone(buzzer_pin);
  }
  for (int i = 4; i >= 0; i--) {  // Move LED from right to left
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 1 << i);
    digitalWrite(latchPin, HIGH);
    tone(buzzer_pin, 1000 + (i * 200));  // Play a tone with a frequency based on the LED index
    delay(del);
    noTone(buzzer_pin);
  }
}



// Debounce variables
unsigned long lastDebounceTime = 0;      // Last time the key state changed
const unsigned long debounceDelay = 50;  // Debounce time in milliseconds
char lastKey = '\0';                     // Store the last key pressed

// Debounce function
// Debounce function
char getDebouncedKey() {
  char currentKey = customKeypad.getKey();  // Get the current key press

  // Check if a key is pressed and debounce logic
  if (currentKey != '\0') {       // Only process if a key is actually pressed
    if (currentKey != lastKey) {  // If the key is different from the last key
      unsigned long now = millis();
      if (now - lastDebounceTime > debounceDelay) {  // Check debounce time
        lastKey = currentKey;                        // Update the last key pressed
        lastDebounceTime = now;                      // Reset debounce timer
        return currentKey;                           // Return the debounced key
      }
    }
  } else {
    lastKey = '\0';  // Reset lastKey when no key is pressed
  }
  return '\0';  // Return '\0' if no valid key press is detected
}
