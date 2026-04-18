/*
  Claude Ghost Macro Pad V1.0 
  適用於 YD-RP2040 (IDE 設定請選 Raspberry Pi Pico)
*/

#include <Adafruit_TinyUSB.h>
#include "keymap.h"

// --- 1. 手動建立 Keyboard 模擬物件 ---
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
    if (!usb_hid.ready()) return;
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

  // 等待 USB 端點就緒後再送釋放報告，確保主機能收到 key-up
  void releaseAll() {
    unsigned long t = millis();
    while (!usb_hid.ready() && (millis() - t) < 20) delay(1);
    usb_hid.keyboardRelease(0);
  }

  // 逐字輸出字串（支援 a-z A-Z 0-9 / space \n -）
  void typeString(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
      uint8_t key = 0, mod = 0;
      char c = str[i];
      if      (c >= 'a' && c <= 'z') { key = HID_KEY_A + (c - 'a'); }
      else if (c >= 'A' && c <= 'Z') { key = HID_KEY_A + (c - 'A'); mod = 0x02; }
      else if (c >= '1' && c <= '9') { key = HID_KEY_1 + (c - '1'); }
      else if (c == '0')  { key = HID_KEY_0;      }
      else if (c == '\n') { delay(200); key = 0x28; }
      else if (c == ' ')  { key = HID_KEY_SPACE;   }
      else if (c == '/')  { key = HID_KEY_SLASH;   }
      else if (c == '-')  { key = HID_KEY_MINUS;   }
      if (key == 0) continue;
      unsigned long t = millis();
      while (!usb_hid.ready() && (millis() - t) < 100) delay(1);
      uint8_t keys[6] = {key, 0, 0, 0, 0, 0};
      if (!usb_hid.keyboardReport(0, mod, keys)) {
        delay(20);
        usb_hid.keyboardReport(0, mod, keys);
      }
      delay(30);
      releaseAll();
      delay(30);
    }
  }
} Keyboard;

// --- 3. 硬體腳位定義 ---
const int LED_PINS[6] = {2, 3, 4, 28, 27, 26}; // R1 G1 B1 R2 G2 B2
const int BTN_PINS[10] = {6, 7, 16, 17, 8, 9, 19, 18, 0, 22};
const int ENC_CLK = 20;
const int ENC_DT  = 21;

// --- 4. Debounce 狀態 ---
const unsigned long DEBOUNCE_MS = 20;

bool rawBtn[10];       // 目前讀到的原始電位
bool stableBtn[10];    // 經過 debounce 後的穩定狀態
unsigned long debounceTimer[10];

// 旋鈕 - 正交狀態機 + 穩定時間 debounce
// 新狀態必須保持穩定 ENC_STABLE_MS 毫秒才算有效轉換，防止彈跳累積
uint8_t encState   = 0;  // 最後確認的穩定狀態
uint8_t encPending = 0;  // 目前讀到、尚待確認的狀態
int8_t  encCount   = 0;
unsigned long encStableTimer = 0;
const unsigned long ENC_STABLE_MS  = 5;  // 信號穩定門檻（毫秒）
const int8_t        STEPS_PER_DETENT = 4; // EC11 標準值；若一格觸發兩次改成 2

// 正交解碼表：ENC_TABLE[舊狀態][新狀態] = +1 (CW) / -1 (CCW) / 0 (無效跳變)
const int8_t ENC_TABLE[4][4] = {
//  新: 00  01  10  11
    {  0, +1, -1,  0 }, // 舊 00
    { -1,  0,  0, +1 }, // 舊 01
    { +1,  0,  0, -1 }, // 舊 10
    {  0, -1, +1,  0 }, // 舊 11
};

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
  encState = encPending = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);

  Keyboard.begin();
  Serial.begin(9600);

  // 等待 USB 枚舉完成（最多 2 秒），確保動畫在 USB 穩定後執行
  unsigned long t0 = millis();
  while (!TinyUSBDevice.mounted() && (millis() - t0 < 2000)) delay(1);

  // 啟動成功的視覺反饋：閃爍三次藍燈（250ms 週期，肉眼清晰可見）
  for (int i = 0; i < 3; i++) {
    setLeds(0,0,1, 0,0,1); delay(250);
    setLeds(0,0,0, 0,0,0); delay(250);
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
        if (BTN_MACROS[i][0] != '\0') {
          Keyboard.typeString(BTN_MACROS[i]);
        } else {
          Keyboard.sendCombo(BTN_KEYS[i]);
        }
      } else {                             // 放開
        Keyboard.releaseAll();
      }
    }
  }

  // 處理旋鈕（正交狀態機 + 穩定時間 debounce）
  uint8_t rawEnc = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);

  if (rawEnc != encState) {
    if (rawEnc != encPending) {
      encPending = rawEnc;
      encStableTimer = now;
    }

    if ((now - encStableTimer) >= ENC_STABLE_MS) {
      int8_t movement = ENC_TABLE[encState][encPending];
      if (movement != 0) {
        encCount += movement;
      }
      encState = encPending;

      if (encCount >= STEPS_PER_DETENT) {
        uint8_t up[3] = {UP, 0, 0};
        Keyboard.sendCombo(up);
        Keyboard.releaseAll();
        encCount = 0;
      } else if (encCount <= -STEPS_PER_DETENT) {
        uint8_t dn[3] = {DOWN, 0, 0};
        Keyboard.sendCombo(dn);
        Keyboard.releaseAll();
        encCount = 0;
      }
    }
  }
}
