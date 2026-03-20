/*
 * ============================================================
 * Project: Dual-Player Reaction Time Measurement System
 * Group:   Group 7
 * Course:  Microprocessors and Embedded Systems
 * ============================================================
 * Description:
 * A competitive two-player reaction time tester using ESP32.
 * A stimulus LED activates after a random delay accompanied
 * by three short beeps. Both players press their buttons as
 * fast as possible. The system measures each player's
 * response time in milliseconds, determines the winner,
 * detects false starts with a long warning beep, and
 * displays all results on a 16x2 I2C LCD display.
 * ============================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================================
// PIN DEFINITIONS
// ============================================================
#define LED_PIN        15   // Stimulus LED output pin
#define PLAYER1_BTN     4   // Player 1 button input pin
#define PLAYER2_BTN     5   // Player 2 button input pin
#define BUZZER_PIN      18  // Buzzer output pin

// ============================================================
// LCD INITIALISATION
// Address 0x27, 16 columns, 2 rows
// ============================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============================================================
// FSM STATE DEFINITIONS
// ============================================================
typedef enum {
  STATE_IDLE,         // System waiting to start
  STATE_WAITING,      // Random delay before LED activates
  STATE_STIMULUS,     // LED ON — players must press now
  STATE_RESULT,       // Times captured — display results
  STATE_FAULT         // False start detected
} GameState;

GameState currentState = STATE_IDLE;

// ============================================================
// GAME VARIABLES
// ============================================================
volatile bool p1Pressed       = false; // Player 1 press flag (ISR)
volatile bool p2Pressed       = false; // Player 2 press flag (ISR)
volatile unsigned long p1Time = 0;     // Player 1 timestamp (ms)
volatile unsigned long p2Time = 0;     // Player 2 timestamp (ms)

unsigned long stimulusTime    = 0;     // Time when LED turned ON
unsigned long p1Reaction      = 0;     // Player 1 reaction time result
unsigned long p2Reaction      = 0;     // Player 2 reaction time result

int p1Wins = 0;   // Player 1 win counter across session
int p2Wins = 0;   // Player 2 win counter across session

bool gameActive = false;  // Controls whether ISRs should record

// ============================================================
// BUZZER HELPER FUNCTIONS
// All sound feedback is handled through these functions
// ============================================================

/*
 * stimulusBeep()
 * Plays 3 short sharp beeps when the stimulus LED activates.
 * Signals both players to react immediately.
 * Pattern: BEEP-BEEP-BEEP (100ms each, 80ms gap)
 */
void stimulusBeep() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);                   // Beep ON for 100ms
    digitalWrite(BUZZER_PIN, LOW);
    delay(80);                    // Gap between beeps 80ms
  }
}

/*
 * winnerBeep()
 * Plays 1 long high beep to celebrate a valid winning press.
 * Duration: 600ms continuous tone
 */
void winnerBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(600);                     // Single long beep 600ms
  digitalWrite(BUZZER_PIN, LOW);
}

/*
 * falseStartBeep()
 * Plays 1 long continuous low tone to signal a false start.
 * Distinct from winner beep — longer and more alarming.
 * Duration: 1200ms continuous tone
 */
void falseStartBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1200);                    // Long alarming beep 1200ms
  digitalWrite(BUZZER_PIN, LOW);
}

/*
 * restartBeep()
 * Plays 2 quick soft beeps to signal round is restarting.
 * Pattern: beep-beep (short, 60ms each)
 */
void restartBeep() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(60);                    // Short beep 60ms
    digitalWrite(BUZZER_PIN, LOW);
    delay(60);                    // Short gap 60ms
  }
}

/*
 * drawBeep()
 * Plays 4 alternating rapid beeps for a draw result.
 * Pattern: beep-beep-beep-beep (80ms each)
 */
void drawBeep() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(80);
    digitalWrite(BUZZER_PIN, LOW);
    delay(60);
  }
}

// ============================================================
// INTERRUPT SERVICE ROUTINES
// Fire INSTANTLY when a button is pressed (FALLING edge).
// Only records timestamp and sets flag — no other logic here.
// ============================================================
void IRAM_ATTR player1ISR() {
  if (gameActive && !p1Pressed) {
    p1Pressed = true;
    p1Time    = millis();   // Capture exact press timestamp
  }
}

void IRAM_ATTR player2ISR() {
  if (gameActive && !p2Pressed) {
    p2Pressed = true;
    p2Time    = millis();   // Capture exact press timestamp
  }
}

// ============================================================
// SETUP — runs once when ESP32 powers on
// ============================================================
void setup() {
  Serial.begin(115200);

  // Configure output pins
  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN,    LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Configure button pins with internal pull-up resistors
  pinMode(PLAYER1_BTN, INPUT_PULLUP);
  pinMode(PLAYER2_BTN, INPUT_PULLUP);

  // Attach interrupts on FALLING edge (press = LOW)
  attachInterrupt(digitalPinToInterrupt(PLAYER1_BTN),
                  player1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(PLAYER2_BTN),
                  player2ISR, FALLING);

  // Initialise LCD
  lcd.init();
  lcd.backlight();

  // Startup beep — single short confirmation tone
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);

  // Welcome screen
  lcd.setCursor(0, 0);
  lcd.print("  GROUP 7 GAME  ");
  lcd.setCursor(0, 1);
  lcd.print(" Press Any Btn  ");

  Serial.println("System ready. Press any button to start.");
}

// ============================================================
// MAIN LOOP — FSM executes continuously
// ============================================================
void loop() {

  switch (currentState) {

    // --------------------------------------------------------
    case STATE_IDLE:
    // Waiting for either player to press to begin a round
    // --------------------------------------------------------
      if (digitalRead(PLAYER1_BTN) == LOW ||
          digitalRead(PLAYER2_BTN) == LOW) {

        delay(300);  // Debounce
        restartBeep();  // Two quick beeps — round starting

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  GET READY...  ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        delay(1000);

        currentState = STATE_WAITING;
      }
      break;

    // --------------------------------------------------------
    case STATE_WAITING:
    // Random delay 2–5 seconds — watch for false starts
    // --------------------------------------------------------
    {
      // Reset all flags and timestamps for fresh round
      p1Pressed  = false;
      p2Pressed  = false;
      p1Time     = 0;
      p2Time     = 0;
      gameActive = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  WAIT FOR IT...");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      // Generate true random delay 2000ms–5000ms
      unsigned long waitTime  = random(2000, 5001);
      unsigned long waitStart = millis();

      Serial.print("Random delay: ");
      Serial.print(waitTime);
      Serial.println("ms");

      // Monitor for false starts throughout entire delay period
      while (millis() - waitStart < waitTime) {

        // Player 1 false start detected
        if (digitalRead(PLAYER1_BTN) == LOW) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(" FALSE START!!  ");
          lcd.setCursor(0, 1);
          lcd.print("   PLAYER  1    ");
          falseStartBeep();  // Long alarming beep
          currentState = STATE_FAULT;
          return;
        }

        // Player 2 false start detected
        if (digitalRead(PLAYER2_BTN) == LOW) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(" FALSE START!!  ");
          lcd.setCursor(0, 1);
          lcd.print("   PLAYER  2    ");
          falseStartBeep();  // Long alarming beep
          currentState = STATE_FAULT;
          return;
        }

        delay(10);  // Small yield prevents watchdog timeout
      }

      // No false start — proceed to stimulus
      currentState = STATE_STIMULUS;
      break;
    }

    // --------------------------------------------------------
    case STATE_STIMULUS:
    // Activate LED + buzzer beeps, begin timing both players
    // --------------------------------------------------------
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(">>>> PRESS NOW! ");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      // Fire stimulus beeps FIRST then turn LED ON
      // so buzzer and LED activate together
      stimulusBeep();              // 3 sharp beeps — GO GO GO

      digitalWrite(LED_PIN, HIGH); // LED ON
      gameActive   = true;         // Enable ISR press recording
      stimulusTime = millis();     // Record LED activation time

      Serial.println("STIMULUS ON — recording responses...");

      // Wait for both players or until 5 second timeout
      while (millis() - stimulusTime < 5000) {
        if (p1Pressed && p2Pressed)  break;  // Both responded
        if (p1Pressed  && millis() - p1Time > 500) break;
        if (p2Pressed  && millis() - p2Time > 500) break;
        delay(1);
      }

      gameActive = false;           // Stop recording new presses
      digitalWrite(LED_PIN, LOW);  // LED OFF

      // Calculate reaction times from stimulus activation point
      p1Reaction = p1Pressed ? (p1Time - stimulusTime) : 0;
      p2Reaction = p2Pressed ? (p2Time - stimulusTime) : 0;

      currentState = STATE_RESULT;
      break;

    // --------------------------------------------------------
    case STATE_RESULT:
    // Evaluate results, play appropriate sound, show on LCD
    // --------------------------------------------------------
    {
      lcd.clear();

      // CASE 1: Both players pressed
      if (p1Pressed && p2Pressed) {

        // Display both times on row 1
        lcd.setCursor(0, 0);
        lcd.print("P1:");
        lcd.print(p1Reaction);
        lcd.print("ms P2:");
        lcd.print(p2Reaction);
        lcd.print("ms");

        // Determine and display winner on row 2
        lcd.setCursor(0, 1);
        if (p1Reaction < p2Reaction) {
          lcd.print("** P1  WINS! ** ");
          p1Wins++;
          winnerBeep();             // Celebratory long beep

        } else if (p2Reaction < p1Reaction) {
          lcd.print("** P2  WINS! ** ");
          p2Wins++;
          winnerBeep();             // Celebratory long beep

        } else {
          lcd.print("** IT'S A DRAW**");
          drawBeep();               // 4 rapid alternating beeps
        }

        Serial.print("P1: "); Serial.print(p1Reaction);
        Serial.print("ms | P2: "); Serial.print(p2Reaction);
        Serial.println("ms");
      }

      // CASE 2: Only Player 1 pressed
      else if (p1Pressed && !p2Pressed) {
        lcd.setCursor(0, 0);
        lcd.print("P1: ");
        lcd.print(p1Reaction);
        lcd.print("ms        ");
        lcd.setCursor(0, 1);
        lcd.print("P2: No Response ");
        p1Wins++;
        winnerBeep();               // P1 wins by default
      }

      // CASE 3: Only Player 2 pressed
      else if (p2Pressed && !p1Pressed) {
        lcd.setCursor(0, 0);
        lcd.print("P2: ");
        lcd.print(p2Reaction);
        lcd.print("ms        ");
        lcd.setCursor(0, 1);
        lcd.print("P1: No Response ");
        p2Wins++;
        winnerBeep();               // P2 wins by default
      }

      // CASE 4: Nobody pressed — timeout
      else {
        lcd.setCursor(0, 0);
        lcd.print("  NO RESPONSE   ");
        lcd.setCursor(0, 1);
        lcd.print("  Round Voided  ");
        // No beep for timeout — silent
      }

      delay(4000);  // Hold results on screen for 4 seconds

      // Show running score tally
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  SCORE TALLY:  ");
      lcd.setCursor(0, 1);
      lcd.print("P1:");
      lcd.print(p1Wins);
      lcd.print("   P2:");
      lcd.print(p2Wins);

      delay(2500);

      restartBeep();              // Two soft beeps — next round
      currentState = STATE_WAITING;
      break;
    }

    // --------------------------------------------------------
    case STATE_FAULT:
    // False start — hold fault display then restart round
    // --------------------------------------------------------
      delay(3000);              // Hold fault message 3 seconds

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  RESTARTING... ");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      restartBeep();            // Two soft beeps before restart
      delay(800);

      currentState = STATE_WAITING;  // Restart round cleanly
      break;
  }
}


