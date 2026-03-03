# 🎯 High-Precision Dual-Player Reaction Time Measurement System

> **COE 381 — Microprocessors | Group 7 | Department of Computer Engineering | KNUST**

---

## 📋 Project Information

| Field | Details |
|---|---|
| **Project Title** | High-Precision Dual-Player Reaction Time Measurement System |
| **Course** | Microprocessors |
| **Course Code** | COE 381 |
| **Institution** | Kwame Nkrumah University of Science and Technology (KNUST) |
| **Department** | Department of Computer Engineering |
| **Supervisor** | Dr. Eliel Keelson |
| **Group** | Group 7 |
| **Submission Date** | March 29, 2026 |

---

## 👥 Group Members

| # | Full Name | Index Number |
|---|---|---|
| 1 | Asiedu Seth Osei | 7265823 |
| 2 | Frimpong Roddy | 7271123 |
| 3 | Jessica Oforiwaa Anim | 7263623 |
| 4 | Asante Samuel Osei | 7265223 |
| 5 | Bosomtwi Akosua Serwaa Maame | 7268523 |
| 6 | Daniel Kuma Gyebi | 7272223 |
| 7 | Samuella Andoh Bannerman | 7263423 |
| 8 | Kamil Khadija Ndaiya | 7273323 |
| 9 | Frimpong Solomon Junior | 7271223 |

---

## 📌 Project Summary

This project designs and implements a competitive **dual-player reaction time measurement system** using the **ESP32 microcontroller**, simulated on the **Wokwi** online embedded systems platform.

A randomised stimulus LED activates after a variable delay of 2–5 seconds. Both players respond simultaneously by pressing their dedicated push buttons. The system:

- Measures each player's reaction time in **milliseconds** using interrupt-driven GPIO detection
- Determines and declares the **winner** based on comparative timestamp analysis
- Detects and penalises **false starts** before the stimulus activates
- Displays all results on a **16x2 I2C LCD display**
- Provides distinct **audio feedback** through a passive buzzer for every game event
- Maintains a **running score tally** across multiple rounds

---

## 🛠️ Hardware Components

| Component | Specification | Role |
|---|---|---|
| ESP32 DevKit V1 | Dual-core, 240 MHz, 3.3V GPIO | Main microcontroller |
| 16x2 LCD Display (I2C) | PCF8574 backpack, address 0x27 | Displays times and winner |
| Stimulus LED | 5mm Red, Vf ≈ 2.0V | Visual GO signal for both players |
| Push Button × 2 | SPST Tactile momentary | Player 1 and Player 2 inputs |
| Resistor (220Ω) | 1/4W carbon film | LED current limiting |
| Passive Buzzer | 5V, direct GPIO drive | Audio event feedback |
| Breadboard | 830-point solderless | Circuit assembly |
| Jumper Wires | Male-to-male, assorted | Component connections |

---

## 🔌 Pin Connection Table

| ESP32 GPIO | Connected To | Direction | Function |
|---|---|---|---|
| GPIO 15 | Stimulus LED (via 220Ω) | OUTPUT | Visual stimulus signal |
| GPIO 4 | Player 1 Button | INPUT | P1 reaction press detection |
| GPIO 5 | Player 2 Button | INPUT | P2 reaction press detection |
| GPIO 18 | Buzzer (+) | OUTPUT | Audio feedback output |
| GPIO 21 (SDA) | LCD SDA | I2C Data | Display data line |
| GPIO 22 (SCL) | LCD SCL | I2C Clock | Display clock line |
| 3V3 | LCD VCC | POWER | LCD supply voltage |
| GND | LCD GND, Buttons, LED, Buzzer | GROUND | Common ground reference |

---

## 🔁 System State Machine

The firmware operates as a **5-state Finite State Machine (FSM)**:

```
                    ┌─────────────┐
         Power ON → │  STATE_IDLE  │ ← Any button held to start
                    └──────┬──────┘
                           │
                    ┌──────▼──────┐
                    │STATE_WAITING│ ← Random 2–5 second delay
                    └──────┬──────┘
                    │             │
            Button pressed    Delay elapsed
            too early              │
                    │      ┌───────▼────────┐
                    │      │STATE_STIMULUS  │ ← LED ON + 3 beeps
                    │      └───────┬────────┘
                    │              │ Button(s) pressed
                    │      ┌───────▼────────┐
                    │      │ STATE_RESULT   │ ← Times shown + winner
                    │      └───────┬────────┘
                    │              │ Next round
                    ▼              │
             ┌──────────┐         │
             │STATE_FAULT│         │
             └──────┬───┘         │
                    │             │
                    └─────────────┘
                    Both return to STATE_WAITING
```

---

## 🔊 Buzzer Feedback Reference

| Event | Pattern | Duration |
|---|---|---|
| System startup | 1 single beep | 150ms |
| Round starting | 2 quick beeps | 60ms each |
| LED stimulus activates | 3 sharp beeps — GO! | 100ms each, 80ms gap |
| Valid win | 1 long beep | 600ms |
| Draw result | 4 rapid beeps | 80ms each, 60ms gap |
| False start detected | 1 long alarm tone | 1200ms |
| Round restarting | 2 soft beeps | 60ms each |

---

## 💻 Software & Libraries

| Tool / Library | Version | Purpose |
|---|---|---|
| Arduino Framework | Latest | Firmware development language |
| Wokwi Simulator | Online | Circuit simulation platform |
| LiquidCrystal_I2C | 1.1.2 | I2C LCD display control |
| Wire.h | Built-in | I2C two-wire communication |
| millis() | Built-in | Millisecond timer for reaction time |
| attachInterrupt() | Built-in | GPIO interrupt for button detection |

---

## 🗂️ Repository Structure

```
coe_group07/
│
├── README.md                          ← You are here
│
├── code/
│   └── reaction_time_game.ino         ← Complete ESP32 Arduino firmware
│
├── simulation/
│   ├── wokwi_link.txt                 ← Direct link to live Wokwi simulation
│   └── circuit_screenshot.png         ← Screenshot of the complete wired circuit
│
├── report/
│   └── Group7_Reaction_Time_Report.pdf ← Full academic project report
│
└── presentation/
    ├── Slides.pdf                      ← Presentation slide deck
    └── Coe_group7_presentation.mp4     ← Video presentation (max 15 minutes)
```

---

## 🚀 How to Run the Simulation

**Option A — Wokwi Online (Recommended)**

1. Open the link inside `simulation/wokwi_link.txt`
2. The complete circuit opens directly in your browser
3. Click the green **Play ▶** button to start the simulation
4. The LCD will display: `GROUP 7 GAME — Press Any Btn`
5. Click either button on screen to begin a round
6. Wait for the LED to turn ON and the buzzer to sound three beeps
7. Click both buttons quickly to simulate Player 1 and Player 2 reacting
8. Read the reaction times and winner on the LCD display

**Option B — Arduino IDE (Physical Hardware)**

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install the **ESP32 board package** via Boards Manager
3. Install the **LiquidCrystal_I2C** library via Library Manager
4. Open `code/reaction_time_game.ino`
5. Connect your ESP32 via USB
6. Select the correct board: `ESP32 Dev Module`
7. Select the correct COM port
8. Click **Upload**
9. Wire the circuit as shown in the pin table above

---

## 📊 How the Reaction Time is Measured

```
Timeline:
──────────────────────────────────────────────────────────►
         │                    │              │
    LED turns ON          P1 presses     P2 presses
    T_start = millis()    p1Time=millis() p2Time=millis()

Calculation:
    P1 Reaction Time = p1Time - T_start  (milliseconds)
    P2 Reaction Time = p2Time - T_start  (milliseconds)
    Winner           = player with smaller reaction time
```

---

## 🧪 Test Scenarios Covered

| Scenario | Expected Behaviour |
|---|---|
| Normal round — P1 faster | P1 time shown, P2 time shown, P1 declared winner, win beep |
| Normal round — P2 faster | P2 declared winner, win beep |
| Both press simultaneously | Draw declared, 4 rapid beeps |
| P1 presses before LED | FALSE START — PLAYER 1 displayed, alarm beep |
| P2 presses before LED | FALSE START — PLAYER 2 displayed, alarm beep |
| Neither player presses | Round voided after 5-second timeout |
| Only P1 presses | P1 wins by default, P2 shows No Response |
| Only P2 presses | P2 wins by default, P1 shows No Response |

---

## ⚠️ Important Notes

- All buttons use **active-low** logic — a press reads as `LOW` on the input pin
- Internal **pull-up resistors** are enabled in firmware — no external pull-up components needed
- The **volatile** keyword is applied to all variables shared between ISR and main loop
- Both ISR functions use the **IRAM_ATTR** attribute for lowest-latency interrupt execution
- The random delay uses `random(2000, 5001)` — never the same delay twice

---

## 📁 Submission Checklist

- [x] Source code uploaded to `code/` folder
- [ ] Wokwi simulation link added to `simulation/wokwi_link.txt`
- [ ] Circuit screenshot saved to `simulation/circuit_screenshot.png`
- [x] Academic report uploaded to `report/` folder
- [ ] Presentation slides uploaded to `presentation/` folder
- [ ] Video presentation uploaded to `presentation/` folder
- [ ] Video named correctly: `Coe_group7_presentation.mp4`
- [ ] Video submitted to course submission link
- [ ] All group members' names and index numbers verified

---

## 📚 References

1. Espressif Systems. (2023). *ESP32 Technical Reference Manual*. Espressif Systems.
2. Arduino. (2024). *Arduino Language Reference*. Arduino LLC. https://www.arduino.cc/reference/en/
3. Wokwi. (2024). *Wokwi ESP32 Simulation Documentation*. https://docs.wokwi.com
4. Donders, F. C. (1868). On the speed of mental processes. *Acta Psychologica*, 30, 412–431.
5. Mazidi, M. A., Naimi, S., & Naimi, S. (2017). *The AVR Microcontroller and Embedded Systems*. Pearson.

---

<div align="center">

**Group 7 | COE 381 — Microprocessors | KNUST**
**Department of Computer Engineering**
**Supervisor: Dr. Elliel Keelson**
**© 2026 — All work is original. Plagiarism policy strictly observed.**

</div>
