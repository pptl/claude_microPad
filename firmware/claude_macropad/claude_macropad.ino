/*
  Claude Ghost Macro Pad V2.0
  Arduino (Earle Philhower arduino-pico) + Adafruit TinyUSB

  Board:     Generic RP2040 / YD-RP2040
  USB Stack: Adafruit TinyUSB  (Tools > USB Stack)

  GPIO:
    Buttons (INPUT_PULLUP, active LOW):
      GP0  Terminate      -> Ctrl+C
      GP1  ChgPermMode    -> Shift+Tab
      GP2  Explain        -> Ctrl+E
      GP3  UP             -> Arrow Up
      GP4  Enter          -> Enter
      GP5  ShowTodo       -> Ctrl+T
      GP6  Slash          -> /
      GP7  DOWN           -> Arrow Down
      GP8  SwitchWindow   -> Win+Tab
      GP9  Encoder SW     -> ESC

    Encoder:
      GP10 CLK
      GP11 DT             -> scroll = Up/Down arrow

    LED1 left eye  (common-anode: LOW = ON)
      GP12 R1, GP13 G1, GP14 B1

    LED2 right eye (common-anode: LOW = ON)
      GP16 R2, GP17 G2, GP18 B2

  Serial protocol (USB CDC, 9600 baud):
    'T' = Thinking  -> yellow
    'C' = Completed -> green
    'E' = Error     -> red
    'I' = Idle      -> off
*/

#include <Adafruit_TinyUSB.h>
#include <Keyboard.h>

// ---------------------------------------------------------------------------
// LED pins
// ---------------------------------------------------------------------------
const int LED_PINS[6] = {12, 13, 14, 16, 17, 18}; // R1 G1 B1 R2 G2 B2

// Common-anode: LOW = ON, HIGH = OFF
// set_leds(r1, g1, b1, r2, g2, b2) — pass true to light a channel
void setLeds(bool r1, bool g1, bool b1, bool r2, bool g2, bool b2) {
  bool vals[6] = {r1, g1, b1, r2, g2, b2};
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_PINS[i], vals[i] ? LOW : HIGH);
  }
}

void applyState(char cmd) {
  switch (cmd) {
    case 'T': setLeds(0,0,1, 0,0,1); break; // Blue  (Thinking)
    case 'C': setLeds(0,1,0, 0,1,0); break; // Green (Completed)
    case 'E': setLeds(1,0,0, 1,0,0); break; // Red   (Error)
    case 'I': setLeds(0,1,0, 0,1,0); break; // Green (Idle)
  }
}

// ---------------------------------------------------------------------------
// Button pins & key mappings
// ---------------------------------------------------------------------------
const int BTN_PINS[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
bool prevBtn[10];

// Each entry: up to 3 keys (use 0 as "no key")
const uint8_t BTN_KEYS[10][3] = {
  {KEY_LEFT_CTRL,  'c',           0},            // GP0  Ctrl+C
  {KEY_LEFT_SHIFT, KEY_TAB,       0},            // GP1  Shift+Tab
  {KEY_LEFT_CTRL,  'e',           0},            // GP2  Ctrl+E
  {KEY_UP_ARROW,   0,             0},            // GP3  Up
  {KEY_RETURN,     0,             0},            // GP4  Enter
  {KEY_LEFT_CTRL,  't',           0},            // GP5  Ctrl+T
  {'/',            0,             0},            // GP6  /
  {KEY_DOWN_ARROW, 0,             0},            // GP7  Down
  {KEY_LEFT_GUI,   KEY_TAB,       0},            // GP8  Win+Tab
  {KEY_ESC,        0,             0},            // GP9  ESC (encoder SW)
};

// ---------------------------------------------------------------------------
// Encoder pins
// ---------------------------------------------------------------------------
const int ENC_CLK = 10;
const int ENC_DT  = 11;
bool lastClk;

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
void setup() {
  // LED outputs
  for (int i = 0; i < 6; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], HIGH); // all OFF at start
  }

  // Button inputs
  for (int i = 0; i < 10; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
    prevBtn[i] = HIGH;
  }

  // Encoder inputs
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT,  INPUT_PULLUP);
  lastClk = digitalRead(ENC_CLK);

  // USB
  Serial.begin(9600);   // CDC serial (for LED commands from host)
  Keyboard.begin();

  // Idle state on boot
  applyState('I');
}

// ---------------------------------------------------------------------------
// loop
// ---------------------------------------------------------------------------
void loop() {
  // --- Serial command ---
  if (Serial.available()) {
    char cmd = (char)Serial.read();
    applyState(cmd);
  }

  // --- Buttons ---
  for (int i = 0; i < 10; i++) {
    bool cur = digitalRead(BTN_PINS[i]);
    if (cur == LOW && prevBtn[i] == HIGH) {       // press
      for (int k = 0; k < 3; k++) {
        if (BTN_KEYS[i][k]) Keyboard.press(BTN_KEYS[i][k]);
      }
    } else if (cur == HIGH && prevBtn[i] == LOW) { // release
      Keyboard.releaseAll();
    }
    prevBtn[i] = cur;
  }

  // --- Encoder ---
  bool clk = digitalRead(ENC_CLK);
  if (clk != lastClk) {
    if (digitalRead(ENC_DT) != clk) {
      Keyboard.press(KEY_UP_ARROW);
    } else {
      Keyboard.press(KEY_DOWN_ARROW);
    }
    Keyboard.releaseAll();
  }
  lastClk = clk;

  delay(5); // 5ms debounce
}
