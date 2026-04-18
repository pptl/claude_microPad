// ╔══════════════════════════════════════════════════════════╗
// ║         Claude Ghost Macro Pad  按鍵設定                  ║
// ║  只需編輯這個檔案，儲存後重新上傳韌體即可生效                  ║
// ╚══════════════════════════════════════════════════════════╝
//
// 按鍵位置圖：
//
//   [ 8 ]                                        [ 9 ] ← 旋鈕按下
//            [ 0 ]    [ 1 ]    [ 2 ]    [ 3 ]
//            [ 4 ]    [ 5 ]    [ 6 ]    [ 7 ]
//
// ── 格式說明 ──────────────────────────────────────────────
//   每顆按鍵格式：{鍵A, 鍵B, 鍵C}  （最多 3 鍵組合，不用的填 0）
//
//   修飾鍵：CTRL（Ctrl）  SHIFT（Shift）  ALT（Alt）  WIN（Win 鍵）
//   方向鍵：UP  DOWNN  LEFT  RIGHT
//   特殊鍵：ENTER（Enter）  ESC  TAB  SPACE（空白）  DEL  BACKSPACE（Backspace）  SLASH（/）
//   字母鍵：用 L('a') ~ L('z')，例如 L('e')、L('t')、L('c')
//   數字鍵：用 N0 ~ N9，例如 N0、N5、N9
//
//   ⚠ 注意：不可直接用 'c' 或 '5'，ASCII 碼 ≠ USB HID 按鍵碼
//
// ── 範例 ──────────────────────────────────────────────────
//   Ctrl+C        → {CTRL, L('c'), 0}
//   Ctrl+Shift+P  → {CTRL, SHIFT, L('p')}
//   Win+Tab       → {WIN, TAB, 0}
//   數字 5        → {N5, 0, 0}
//   單獨 ESC      → {ESC, 0, 0}
// ─────────────────────────────────────────────────────────

#pragma once

// 修飾鍵
#define CTRL      HID_KEY_CONTROL_LEFT
#define SHIFT     HID_KEY_SHIFT_LEFT
#define ALT       HID_KEY_ALT_LEFT
#define WIN       HID_KEY_GUI_LEFT

// 方向鍵
#define UP        HID_KEY_ARROW_UP
#define DOWN     HID_KEY_ARROW_DOWN
#define LEFT      HID_KEY_ARROW_LEFT
#define RIGHT     HID_KEY_ARROW_RIGHT

// 特殊鍵
#define ENTER     0x28
#define ESC       HID_KEY_ESCAPE
#define TAB       HID_KEY_TAB
#define SPACE     HID_KEY_SPACE
#define DEL       HID_KEY_DELETE
#define BACKSPACE HID_KEY_BACKSPACE
#define SLASH     HID_KEY_SLASH

// 字母鍵（HID_KEY_A = 0x04，a-z 連續排列）
#define L(c)  ((uint8_t)(HID_KEY_A + ((c) - 'a')))

// 數字鍵（HID 規範中 0 在 9 之後，不連續，需個別定義）
#define N1  HID_KEY_1
#define N2  HID_KEY_2
#define N3  HID_KEY_3
#define N4  HID_KEY_4
#define N5  HID_KEY_5
#define N6  HID_KEY_6
#define N7  HID_KEY_7
#define N8  HID_KEY_8
#define N9  HID_KEY_9
#define N0  HID_KEY_0

// ══════════════════════════════════════════════════════════
//   在這裡修改每顆按鍵的功能
// ══════════════════════════════════════════════════════════
//
// 【BTN_KEYS】  單次組合鍵，例如 Ctrl+C、Win+Tab
// 【BTN_MACROS】文字巨集，逐字輸出一串字元
//   - 若 BTN_MACROS[i] 非空，優先執行巨集，忽略 BTN_KEYS[i]
//   - 支援字元：a-z A-Z 0-9 / 空白 Enter(\n) 減號(-)
//   - 範例："/compact\n" 輸出 /compact 後自動按 Enter
//     中文輸入法使用者建議用 "\n\n"，因為輸入法會吃掉一個 \n

const char* BTN_MACROS[10] = {
  "/clear\n\n",     // [0] /clear（清除對話）
  "/compact\n\n",   // [1] /compact（整理對話）
  "",               // [2] ↓ 使用 BTN_KEYS
  "",               // [3] ↓ 使用 BTN_KEYS
  "/gitPush\n\n",   // [4] /gitPush（推送 Git）
  "/gitCommit\n\n", // [5] /gitCommit（提交 Git）
  "",               // [6] ↓ 使用 BTN_KEYS
  "allow\n\n",      // [7] allow（允許操作）
  "",               // [8] ↓ 使用 BTN_KEYS
  "",               // [9] ↓ 使用 BTN_KEYS
};

const uint8_t BTN_KEYS[10][3] = {
  {0,     0,       0},  // [0] （巨集優先，此鍵不作用）
  {0,     0,       0},  // [1] （巨集優先，此鍵不作用）
  {WIN,   TAB,     0},  // [2] WIN+Tab
  {ENTER, 0,       0},  // [3] Enter（送出）
  {0,     0,       0},  // [4] （巨集優先，此鍵不作用）
  {0,     0,       0},  // [5] （巨集優先，此鍵不作用）
  {SHIFT, TAB,     0},  // [6] Shift+Tab（切換權限模式）
  {0,     0,       0},  // [7] （巨集優先，此鍵不作用）
  {ESC,   0,       0},  // [8] ESC
  {ENTER, 0,       0},  // [9] 旋鈕按下（Enter）
};
