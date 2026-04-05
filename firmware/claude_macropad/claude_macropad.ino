/*
  Claude Ghost Macro Pad V1.0 
  適用於 YD-RP2040 (IDE 設定請選 Raspberry Pi Pico)
*/

#include <Adafruit_TinyUSB.h>

// --- 1. 橋接按鍵定義 (將舊代碼轉向 TinyUSB) ---
#define KEY_LEFT_CTRL   HID_KEY_CONTROL_LEFT
#define KEY_LEFT_SHIFT  HID_KEY_SHIFT_LEFT
#define KEY_LEFT_GUI    HID_KEY_GUI_LEFT
#define KEY_LEFT_ALT    HID_KEY_ALT_LEFT
#define KEY_UP_ARROW    HID_KEY_ARROW_UP
#define KEY_DOWN_ARROW  HID_KEY_ARROW_DOWN
#define KEY_RETURN      HID_KEY_RETURN
#define KEY_ESC         HID_KEY_ESCAPE
#define KEY_TAB         HID_KEY_TAB

// --- 2. 手動建立 Keyboard 模擬物件 ---
uint8_t const desc_hid_report[] = { TUD_HID_REPORT_DESC_KEYBOARD() };
Adafruit_USBD_HID usb_hid;

class KeyboardEmulator {
public:
  void begin() {
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
  }

  // 一次送出最多 3 個按鍵的完整組合報告（同時處理 modifier + 一般鍵）
  void sendCombo(const uint8_t keys[3]) {
    uint8_t mod = 0;
    uint8_t keycodes[6] = {0};
    int ki = 0;
    for (int k = 0; k < 3; k++) {
      if (keys[k] == 0) continue;
      if (keys[k] >= 0xE0 && keys[k] <= 0xE7) {
        mod |= (1 << (keys[k] - 0xE0));  // modifier key
      } else {
        if (ki < 6) keycodes[ki++] = keys[k];
      }
    }
    usb_hid.keyboardReport(0, mod, keycodes);
  }

  void releaseAll() { usb_hid.keyboardRelease(0); }
} Keyboard;

// --- 3. 硬體腳位定義 ---
const int LED_PINS[6] = {12, 13, 14, 16, 17, 18}; // R1 G1 B1 R2 G2 B2
const int BTN_PINS[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const int ENC_CLK = 10;
const int ENC_DT  = 11;

const uint8_t BTN_KEYS[10][3] = {
  {KEY_LEFT_CTRL,  'c',           0}, // GP0: Ctrl+C
  {KEY_LEFT_SHIFT, KEY_TAB,       0}, // GP1: Shift+Tab
  {KEY_LEFT_CTRL,  'e',           0}, // GP2: Ctrl+E
  {KEY_UP_ARROW,   0,             0}, // GP3: Up
  {KEY_RETURN,     0,             0}, // GP4: Enter
  {KEY_LEFT_CTRL,  't',           0}, // GP5: Ctrl+T
  {HID_KEY_SLASH,  0,             0}, // GP6: /
  {KEY_DOWN_ARROW, 0,             0}, // GP7: Down
  {KEY_LEFT_GUI,   KEY_TAB,       0}, // GP8: Win+Tab
  {KEY_ESC,        0,             0}  // GP9: ESC
};

// --- 4. Debounce 狀態 ---
const unsigned long DEBOUNCE_MS = 20;

bool rawBtn[10];       // 目前讀到的原始電位
bool stableBtn[10];    // 經過 debounce 後的穩定狀態
unsigned long debounceTimer[10];

// 旋鈕 debounce
bool lastClk;
unsigned long encTimer = 0;
const unsigned long ENC_DEBOUNCE_MS = 5;

// --- 5. LED ---
void setLeds(bool r1, bool g1, bool b1, bool r2, bool g2, bool b2) {
  bool v[6] = {r1, g1, b1, r2, g2, b2};
  for (int i = 0; i < 6; i++) digitalWrite(LED_PINS[i], v[i] ? LOW : HIGH);
}

void applyState(char cmd) {
  switch (cmd) {
    case 'T': setLeds(0,0,1, 0,0,1); break; // Blue  (Thinking)
    case 'C': setLeds(0,1,0, 0,1,0); break; // Green (Completed)
    case 'E': setLeds(1,0,0, 1,0,0); break; // Red   (Error)
    case 'I': setLeds(0,1,0, 0,1,0); break; // Green (Idle)
  }
}

// --- 6. Setup ---
void setup() {
  for (int i = 0; i < 6; i++) { pinMode(LED_PINS[i], OUTPUT); digitalWrite(LED_PINS[i], HIGH); }

  for (int i = 0; i < 10; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
    rawBtn[i]    = HIGH;
    stableBtn[i] = HIGH;
    debounceTimer[i] = 0;
  }

  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT,  INPUT_PULLUP);
  lastClk = digitalRead(ENC_CLK);

  Keyboard.begin();
  Serial.begin(9600);

  // 啟動成功的視覺反饋：閃爍三次藍燈
  for (int i = 0; i < 3; i++) {
    setLeds(0,0,1, 0,0,1); delay(100);
    setLeds(0,0,0, 0,0,0); delay(100);
  }
  applyState('I');
}

// --- 7. Loop ---
void loop() {
  unsigned long now = millis();

  // 處理序列埠指令
  if (Serial.available()) {
    char cmd = (char)Serial.read();
    applyState(cmd);
  }

  // 處理按鈕（時間基礎 debounce）
  for (int i = 0; i < 10; i++) {
    bool raw = digitalRead(BTN_PINS[i]);

    // 偵測到電位變化，重設計時器
    if (raw != rawBtn[i]) {
      rawBtn[i] = raw;
      debounceTimer[i] = now;
    }

    // 超過穩定時間後，更新穩定狀態並觸發動作
    if ((now - debounceTimer[i]) >= DEBOUNCE_MS && raw != stableBtn[i]) {
      stableBtn[i] = raw;
      if (raw == LOW) {                    // 按下
        Keyboard.sendCombo(BTN_KEYS[i]);
      } else {                             // 放開
        Keyboard.releaseAll();
      }
    }
  }

  // 處理旋鈕（加入最小間隔防止連觸）
  bool clk = digitalRead(ENC_CLK);
  if (clk != lastClk && (now - encTimer) >= ENC_DEBOUNCE_MS) {
    if (digitalRead(ENC_DT) != clk) {
      uint8_t up[3] = {KEY_UP_ARROW, 0, 0};
      Keyboard.sendCombo(up);
    } else {
      uint8_t dn[3] = {KEY_DOWN_ARROW, 0, 0};
      Keyboard.sendCombo(dn);
    }
    Keyboard.releaseAll();
    encTimer = now;
  }
  lastClk = clk;
}
