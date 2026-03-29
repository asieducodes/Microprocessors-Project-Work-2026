# High-Precision Dual-Player Reaction Time Measurement System

<div align="center">

![Project Banner](https://capsule-render.vercel.app/api?type=waving&color=0D1B2A&height=200&section=header&text=Reaction%20Time%20System&fontSize=40&fontColor=00B4D8&animation=fadeIn&fontAlignY=38&desc=Group%207%20%7C%20COE%20381%20%7C%20KNUST&descAlignY=58&descColor=F4A900)

</div>

<div align="center">

![Course](https://img.shields.io/badge/Course-COE%20381%20Microprocessors-0D1B2A?style=for-the-badge&labelColor=1B3A5C)
![Group](https://img.shields.io/badge/Group-7-F4A900?style=for-the-badge&labelColor=0D1B2A)
![Platform](https://img.shields.io/badge/Platform-ESP32%20%2B%20Wokwi-00B4D8?style=for-the-badge&logo=espressif&logoColor=white&labelColor=0D1B2A)
![Language](https://img.shields.io/badge/Language-Arduino%20C%2B%2B-2E6DA4?style=for-the-badge&logo=arduino&logoColor=white&labelColor=0D1B2A)
![Status](https://img.shields.io/badge/Status-Simulation%20Complete-2DC653?style=for-the-badge&labelColor=0D1B2A)
![Deadline](https://img.shields.io/badge/Deadline-March%2029%2C%202026-E63946?style=for-the-badge&labelColor=0D1B2A)

</div>

---

## Project Information

| | |
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

## Group Members

| No. | Full Name | Index Number |
|:---:|---|:---:|
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

## Project Summary

This project designs and implements a competitive dual-player reaction time measurement system using the **ESP32 microcontroller**, simulated on the **Wokwi** online embedded systems platform.

A randomised stimulus LED activates after a variable delay of 2 to 5 seconds. Both players respond simultaneously by pressing their dedicated push buttons. The system measures each player's response time in milliseconds using interrupt-driven GPIO detection, determines the winner through comparative timestamp analysis, detects and penalises false starts, and displays all results on a 16x2 I2C LCD display. A passive buzzer provides distinct audio feedback for every game event, and a running score tally is maintained across multiple rounds.

---

## Hardware Components

| Component | Specification | Role |
|---|---|---|
| ESP32 DevKit V1 | Dual-core, 240 MHz, 3.3V GPIO | Main microcontroller |
| 240x240 LCD Display (ST7789) |Uses SPI interface | Displays times and winner |
| Stimulus LED | 5mm Red, Vf 2.0V | Visual GO signal for both players |
| Push Button x2 | SPST Tactile momentary | Player 1 and Player 2 inputs |
| Resistor (220 ohm) | 1/4W carbon film | LED current limiting |
| Passive Buzzer | 3.3V, direct GPIO drive | Audio event feedback |
| Breadboard | 830-point solderless | Circuit assembly |
| Jumper Wires | Male-to-male, assorted | Component connections |

---

## Pin Connection Table

| ESP32 GPIO | Connected To | Direction | Function |
|:---:|---|:---:|---|
| GPIO 15 | Stimulus LED (via 220 ohm) | OUTPUT | Visual stimulus signal |
| GPIO 4 | Player 1 Button | INPUT | P1 reaction press detection |
| GPIO 5 | Player 2 Button | INPUT | P2 reaction press detection |
| GPIO 18 | Buzzer (+) | OUTPUT | Audio feedback output |
| GPIO 21 | LCD SDA | I2C Data | Display data line |
| GPIO 22 | LCD SCL | I2C Clock | Display clock line |
| 3V3 | LCD VCC | POWER | LCD supply voltage |
| GND | LCD GND, Buttons, LED, Buzzer | GROUND | Common ground reference |

---

## System State Machine

The firmware operates as a 5-state Finite State Machine. The system is always in exactly one state at any time, and specific events trigger transitions between states.

```
                         Power ON
                            │
                    ┌───────▼────────┐
                    │   STATE_IDLE   │  Waiting for any button press
                    └───────┬────────┘
                            │  Button pressed
                    ┌───────▼────────┐
                    │ STATE_WAITING  │  Random 2–5 second delay
                    └───────┬────────┘
                   │                │
         Button pressed          Delay elapsed
         too early                  │
                   │       ┌────────▼───────┐
                   │       │STATE_STIMULUS  │  LED ON + 3 buzzer beeps
                   │       └────────┬───────┘
                   │                │  Button(s) pressed or timeout
                   │       ┌────────▼───────┐
                   │       │ STATE_RESULT   │  Times computed, winner declared
                   │       └────────┬───────┘
                   │                │  Auto-advance to next round
                   ▼                │
          ┌────────────────┐        │
          │  STATE_FAULT   │        │
          └────────┬───────┘        │
                   │                │
                   └────────────────┘
                   Both return to STATE_WAITING
```

---

## Buzzer Feedback Reference

| Event | Pattern | Duration |
|---|---|:---:|
| System startup | 1 single confirmation beep | 150ms |
| Round starting | 2 quick soft beeps | 60ms each |
| LED stimulus activates | 3 sharp beeps — GO signal | 100ms each, 80ms gap |
| Valid win | 1 long celebratory beep | 600ms |
| Draw result | 4 rapid alternating beeps | 80ms each, 60ms gap |
| False start detected | 1 long alarm tone | 1200ms |
| Round restarting | 2 soft transition beeps | 60ms each |

---

## Software and Libraries

| Tool / Library | Version | Purpose |
|---|:---:|---|
| Arduino Framework | Latest | Firmware development language |
| Wokwi Simulator | Online | Circuit simulation platform |
| LiquidCrystal_I2C | 1.1.2 | I2C LCD display control |
| Wire.h | Built-in | I2C two-wire communication |
| millis() | Built-in | Millisecond timer for reaction time measurement |
| attachInterrupt() | Built-in | GPIO interrupt for instantaneous button detection |
| ESP-IDF Framework | v5.x | Hardware firmware development |
| CMake | Built-in | ESP-IDF project build system |

---

## Repository Structure

The repository contains two implementations — a Wokwi simulation
version and a full hardware implementation using ESP-IDF.
```
coe_group07/
│
├── README.md
│
├── code/
│   │
│   ├── reaction_time_game.ino                       ← Wokwi Arduino simulation
│   │
│   └── hardware/                          ← ESP-IDF hardware implementation
│       │
│       ├── .devcontainer/
│       │   ├── devcontainer.json
│       │   └── Dockerfile
│       │
│       ├── assets/
│       │   └── images/
│       │       └── waveforms/
│       │           ├── initialization_sequence.png
│       │           ├── moment_before_trigger_1.png
│       │           ├── moment_before_trigger_2.png
│       │           ├── trigger_2.png
│       │           └── trigger.png
│       │
│       ├── components/
│       │   ├── buzzer_module/
│       │   ├── font_module/
│       │   ├── fsm_states_handler/
│       │   ├── gpio_module/
│       │   ├── graphics_module/
│       │   ├── st7789/
│       │   └── timer_module/
│       │
│       ├── main/
│       │   ├── CMakeLists.txt
│       │   ├── main.c
│       │   └── README.md
│       │
│       ├── .clangd
│       ├── .gitignore
│       └── CMakeLists.txt
│
├── simulation/
│   ├── wokwi_link.txt
│   └── general_setup.png
│
├── report/
│   └── Group7_Reaction_Time_Report.pdf
│
└── presentation/
    ├── Group7_Presentation.pptx
    └── Coe_group7_presentation.mp4 (video in MEGA folder)
```
---

## How to Run the Simulation

**Option A — Wokwi Online (Recommended)**

1. Open the link inside `simulation/wokwi_link.txt`
2. The complete circuit opens directly in your browser — no installation needed
3. Click the green **Play** button to start the simulation
4. The LCD will display `GROUP 7 GAME — Press Any Btn`
5. Click either button on screen to begin a round
6. Wait for the LED to turn ON and the buzzer to sound three beeps
7. Click both buttons in quick succession to simulate both players reacting
8. Read the reaction times and winner declaration on the LCD display

**Option B — Arduino IDE (Physical Hardware)**

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install the ESP32 board package via Boards Manager
3. Install the `LiquidCrystal_I2C` library via Library Manager
4. Open `code/reaction_time_game.ino`
5. Connect your ESP32 board via USB-C
6. Select board: `ESP32 Dev Module`
7. Select the correct COM port
8. Click Upload
9. Wire the circuit according to the pin table above

---
## Hardware Implementation (ESP-IDF)

The hardware version is a complete ESP-IDF C implementation targeting
the ESP32-S3 with a ST7789 SPI TFT display. It is structured as a
modular ESP-IDF component project.

**Key differences from the simulation version:**

| | Simulation | Hardware |
|---|---|---|
| Framework | Arduino C++ | ESP-IDF (C) |
| Display | 16x2 I2C LCD | ST7789 SPI TFT |
| Build system | Arduino IDE | CMake |
| Entry point | .ino file | main.c |
| Architecture | Single file | Modular components |

**Component breakdown:**

| Component | Purpose |
|---|---|
| `buzzer_module` | Buzzer tone generation and event patterns |
| `font_module` | Font rendering for TFT display output |
| `fsm_states_handler` | Finite State Machine logic and state transitions |
| `gpio_module` | GPIO configuration, interrupts, and button handling |
| `graphics_module` | TFT graphics rendering and UI layout |
| `st7789` | Low-level SPI driver for the ST7789 display controller |
| `timer_module` | Hardware timer configuration and microsecond precision timing |

**To build and flash the hardware version:**

1. Install ESP-IDF v5.x — https://docs.espressif.com/projects/esp-idf
2. Open the `code/hardware/` folder in VS Code with the ESP-IDF extension
3. Run `idf.py build` to compile
4. Run `idf.py flash` to upload to the ESP32-S3
5. Run `idf.py monitor` to view serial output

## How Reaction Time is Measured

```
Timeline:
─────────────────────────────────────────────────────────►
        │                    │                │
   LED turns ON          P1 presses       P2 presses
   T_start = millis()    p1Time=millis()  p2Time=millis()

Calculation:
   P1 Reaction Time = p1Time  −  T_start   (milliseconds)
   P2 Reaction Time = p2Time  −  T_start   (milliseconds)
   Winner           = player with the smaller reaction time
```

---

## Test Scenarios

| Scenario | Expected Behaviour |
|---|---|
| Normal round — P1 faster | Both times shown, P1 declared winner, win beep plays |
| Normal round — P2 faster | Both times shown, P2 declared winner, win beep plays |
| Both press simultaneously | Draw declared, 4 rapid beeps play |
| P1 presses before LED activates | FALSE START PLAYER 1 shown, alarm beep plays |
| P2 presses before LED activates | FALSE START PLAYER 2 shown, alarm beep plays |
| Neither player presses | Round voided after 5-second timeout, silent |
| Only P1 presses | P1 wins by default, P2 shows No Response |
| Only P2 presses | P2 wins by default, P1 shows No Response |

---

## Important Technical Notes

> All buttons use **active-low** logic. A press reads as `LOW` because the button connects the pin to GND.

> Internal **pull-up resistors** are enabled via `INPUT_PULLUP` in firmware. No external pull-up components are needed.

> The **`volatile`** keyword is applied to all variables shared between ISR context and the main loop. This prevents the compiler from caching stale values in CPU registers.

> Both ISR functions carry the **`IRAM_ATTR`** attribute. This places interrupt code in the ESP32's fast internal SRAM, eliminating flash cache miss latency on ISR entry.

> The random delay uses `random(2000, 5001)` seeded from the ESP32's hardware RNG. The stimulus interval is never predictable.

---
## References

1. Espressif Systems. (2023). *ESP32 Technical Reference Manual*. Espressif Systems.
2. Arduino. (2024). *Arduino Language Reference*. Arduino LLC. https://www.arduino.cc/reference/en/
3. Wokwi. (2024). *Wokwi ESP32 Simulation Documentation*. https://docs.wokwi.com
4. Donders, F. C. (1868). On the speed of mental processes. *Acta Psychologica*, 30, 412–431.
5. Mazidi, M. A., Naimi, S., & Naimi, S. (2017). *The AVR Microcontroller and Embedded Systems*. Pearson.

---

<div align="center">

![Footer](https://capsule-render.vercel.app/api?type=waving&color=0D1B2A&height=120&section=footer&text=Group%207%20%7C%20COE%20381%20%7C%20KNUST&fontSize=18&fontColor=F4A900&animation=fadeIn&fontAlignY=65)

</div>

<div align="center">

**Department of Computer Engineering &nbsp;|&nbsp; Kwame Nkrumah University of Science and Technology**

**Supervisor: Dr. Eliel Keelson &nbsp;|&nbsp; Submission: March 29, 2026**

![Made with](https://img.shields.io/badge/Made%20with-Arduino%20C%2B%2B-00979D?style=flat-square&logo=arduino&logoColor=white)
![Simulated on](https://img.shields.io/badge/Simulated%20on-Wokwi-2DC653?style=flat-square)
![Institution](https://img.shields.io/badge/KNUST-Computer%20Engineering-1B3A5C?style=flat-square)
![Original Work](https://img.shields.io/badge/Work-100%25%20Original-F4A900?style=flat-square)

</div>
