
# 🗳️ ESP32-Based Electronic Voting Machine (EVM)

A smart and secure **Electronic Voting Machine** designed as part of my **Diploma in Electronics Engineering** at **Government Polytechnic Ghaziabad**.  
The system is built using an **ESP32 microcontroller**, a **1.8-inch TFT display**, and a **4×3 matrix keypad**, offering a modern, user-friendly, and tamper-resistant voting interface.

---

## 🔧 Features

- ✅ **Touch-free keypad-based voting system**
- 📺 **1.8-inch TFT LCD display** (128×160) with dynamic UI using `TFT_eSPI`
- 🧠 **ESP32 microcontroller**: Wi-Fi capable, high-performance MCU
- 💾 **EEPROM storage** for saving:
  - Candidate data
  - Votes cast
  - Admin settings (like max voter limit)
- 🎛️ **4x3 Matrix Keypad**:
  - Buttons `1`–`6`: Voting + Admin controls
  - Buttons `7`, `8`, `9`, `*`, `0`, `#`: Reserved for Admin tasks (view results, reset EEPROM, etc.)
- 🔒 **Two functional segments** for security:  
  - **Voting segment**  
  - **Admin segment**
- 🔈 **Passive Buzzer** for audio feedback:
  - Vote confirmation
  - EEPROM reset
  - Results announcement
- 💡 **74HC595 Shift Register** to control multiple LED indicators for candidate status (enabled/disabled), using fewer GPIO pins

---

## 🛠️ Libraries Used

- **TFT_eSPI** – for TFT LCD UI rendering
- **Keypad** – for scanning and debouncing keypad input
- **EEPROM** – for non-volatile storage of votes and settings

---

## 📦 Hardware Components

| Component            | Quantity | Description                                  |
|----------------------|----------|----------------------------------------------|
| ESP32 Dev Board      | 1        | Main controller with Wi-Fi and GPIOs         |
| 1.8" TFT LCD (ST7735)| 1        | 128×160 SPI-based color display              |
| 4x3 Matrix Keypad    | 1        | For voter/admin input                        |
| EEPROM (internal)    | 1        | Data storage (votes, config)                 |
| 74HC595 Shift Reg.   | 1        | Controls LEDs for candidate status           |
| LEDs                 | 6+       | Indicate enabled/disabled candidates         |
| Passive Buzzer       | 1        | Audio feedback during interactions           |
| Power Supply (5V)    | 1        | USB or external                              |

---

## 📷 Screenshots

<h3>🖼️ UI Screenshot</h3>
<p align="center">
  <img src="images/ui-vote-screen.png" alt="UI Screenshot" width="400"/>
</p>

<h3>🔌 Circuit Diagram</h3>
<p align="center">
  <img src="images/circuit_diagram.jpg" alt="Circuit Diagram" width="450"/>
</p>


---

## 🧠 Educational Objectives

- Understand microcontroller-based embedded design
- Practice real-world use of EEPROM, keypads, and display modules
- Implement secure input systems and persistent memory
- Simulate real-time electronic voting with multiple states and feedback

---

## 🏫 Submitted As

> **Minor Project (Diploma Final Year 5th semester)**  
> 🏛️ *Department of Electronics Engineering*  
> 📍 *Government Polytechnic Ghaziabad UP*  
> 🎓 *Academic Year: 2024–2025*

---



=======
# 🗳️ ESP32-Based Electronic Voting Machine (EVM)

A smart and secure **Electronic Voting Machine** designed as part of my **Diploma in Electronics Engineering** at **Government Polytechnic Ghaziabad**.  
The system is built using an **ESP32 microcontroller**, a **1.8-inch TFT display**, and a **4×3 matrix keypad**, offering a modern, user-friendly, and tamper-resistant voting interface.

---

## 🔧 Features

- ✅ **Touch-free keypad-based voting system**
- 📺 **1.8-inch TFT LCD display** (128×160) with dynamic UI using `TFT_eSPI`
- 🧠 **ESP32 microcontroller**: Wi-Fi capable, high-performance MCU
- 💾 **EEPROM storage** for saving:
  - Candidate data
  - Votes cast
  - Admin settings (like max voter limit)
- 🎛️ **4x3 Matrix Keypad**:
  - Buttons `1`–`6`: Voting + Admin controls
  - Buttons `7`, `8`, `9`, `*`, `0`, `#`: Reserved for Admin tasks (view results, reset EEPROM, etc.)
- 🔒 **Two functional segments** for security:  
  - **Voting segment**  
  - **Admin segment**
- 🔈 **Passive Buzzer** for audio feedback:
  - Vote confirmation
  - EEPROM reset
  - Results announcement
- 💡 **74HC595 Shift Register** to control multiple LED indicators for candidate status (enabled/disabled), using fewer GPIO pins

---

## 🛠️ Libraries Used

- **TFT_eSPI** – for TFT LCD UI rendering
- **Keypad** – for scanning and debouncing keypad input
- **EEPROM** – for non-volatile storage of votes and settings

---

## 📦 Hardware Components

| Component            | Quantity | Description                                 |
|----------------------|----------|----------------------------------------------|
| ESP32 Dev Board      | 1        | Main controller with Wi-Fi and GPIOs         |
| 1.8" TFT LCD (ST7735)| 1        | 128×160 SPI-based color display              |
| 4x3 Matrix Keypad    | 1        | For voter/admin input                        |
| EEPROM (internal)    | 1        | Data storage (votes, config)                 |
| 74HC595 Shift Reg.   | 1        | Controls LEDs for candidate status           |
| LEDs                 | 6+       | Indicate enabled/disabled candidates         |
| Passive Buzzer       | 1        | Audio feedback during interactions           |
| Power Supply (5V)    | 1        | USB or external                              |

---

## 📷 Screenshots

*(You can include UI screenshots and wiring diagrams here)*  
📌 `images/ui-vote-screen.png`  
📌 `images/circuit-diagram.png`

---

## 🧠 Educational Objectives

- Understand microcontroller-based embedded design
- Practice real-world use of EEPROM, keypads, and display modules
- Implement secure input systems and persistent memory
- Simulate real-time electronic voting with multiple states and feedback

---

## 🏫 Submitted As

> **Minor Project (Diploma Final Year 5th semester)**  
> 🏛️ *Department of Electronics Engineering*  
> 📍 *Government Polytechnic Ghaziabad UP*  
> 🎓 *Academic Year: 2024–2025*

---

