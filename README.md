# Claude Ghost Macro Pad 專案計畫書 (V2.0 雙層鍵帽版)

這是一台專為 **Claude Code** 開發流程優化的左手專用巨集鍵盤。本版本採用 **雙層透明鍵帽 (Relegendable Keycaps)** 方案，透過紙條替換實現快速客製化，無需轉印與噴漆。

外殼採鬼魂造型，橙色半透明壓克力，頂部兩顆 RGB LED 作為「鬼眼」狀態燈。

---

## 🛠️ 一、 核心硬體採購清單

| 項目 | 購買規格 | 關鍵規格說明 | 備註 |
| :--- | :--- | :--- | :--- |
| **控制器** | **YD-RP2040** | Raspberry Pi Pico 相容 / Type-C 介面 | 核心開發板 |
| **指示燈** | **5mm RGB LED × 2** | 共陽極 / 四腳 / 透明 | 用於鬼魂眼睛，低電位觸發 |
| **機械軸** | **靜音系列** | 高特青檸軸 V3 / TTC 靜音紅 V3 | 靜音開發專用 |
| **鍵帽** | **雙層透明鍵帽** | 1U Relegendable Keycaps（共 9 顆） | 可拆卸頂蓋，內放紙條 |
| **編碼器** | **EC11 旋鈕** | D 型半軸 / 柄長 15mm / 帶開關 | 30 脈衝版本 |
| **旋鈕帽** | **大型全鋁** | 35×16mm 黑色滾花噴砂 | 直徑 35mm |
| **線材** | **30 AWG 矽膠線** | 超軟、多色套裝 | 內部焊接理線專用 |
| **螺絲** | **M2 × 6mm 平頭螺絲** | 平頭（沉頭）/ 不鏽鋼或黑色 | 上下蓋固定用 |
| **止滑貼片** | **自黏橡膠腳墊 × 6** | 圓形或方形均可 | 底部墊高及止滑 |

---

## ⌨️ 二、 鍵盤佈局

按鍵位置示意圖（與 `keymap.h` 索引對應）：

```
[ 8 ]                                        [ 9 ] ← 旋鈕按下
         [ 0 ]    [ 1 ]    [ 2 ]    [ 3 ]
         [ 4 ]    [ 5 ]    [ 6 ]    [ 7 ]
```

全按鍵均為 1U 規格，目前預設按鍵功能如下：

| 索引 | 位置 | 功能 | 輸出 |
| :--- | :--- | :--- | :--- |
| **[8]** | 左上角獨立鍵 | ESC | ESC |
| **[9]** | 右上角旋鈕按下 | Git Push | `/gitPush` + Enter |
| **[0]** | 矩陣上排 1 | Slash Command | `/` |
| **[1]** | 矩陣上排 2 | Change Permission Mode | Shift+Tab |
| **[2]** | 矩陣上排 3 | Switch Window | Win+Tab |
| **[3]** | 矩陣上排 4 | Enter | Enter |
| **[4]** | 矩陣下排 1 | Git Commit | `/gitCommit` + Enter |
| **[5]** | 矩陣下排 2 | Clear 對話 | `/clear` + Enter |
| **[6]** | 矩陣下排 3 | Compact 對話 | `/compact` + Enter |
| **[7]** | 矩陣下排 4 | Allow | `allow` + Enter |
| — | 旋鈕旋轉 | 捲動 | ↑ / ↓ |

---

## 🔌 三、 電路設計

### WOKWI 可查看電路設計圖

將 `diagram.json` 的內容全部貼到 [WOKWI](https://wokwi.com) 上可以查看整體電路設計圖。

### GPIO 接腳對照（與韌體一致）

| GPIO | 功能 |
| :--- | :--- |
| GP0 | 按鍵 [8] — ESC |
| GP2 | LED1 R（左眼）|
| GP3 | LED1 G（左眼）|
| GP4 | LED1 B（左眼）|
| GP6 | 按鍵 [0] — Slash (`/`) |
| GP7 | 按鍵 [1] — Shift+Tab |
| GP8 | 按鍵 [4] — /gitPush |
| GP9 | 按鍵 [5] — claude |
| GP16 | 按鍵 [2] — Win+Tab |
| GP17 | 按鍵 [3] — Enter |
| GP18 | 按鍵 [7] — /compact |
| GP19 | 按鍵 [6] — /clear |
| GP20 | Encoder CLK |
| GP21 | Encoder DT |
| GP22 | 按鍵 [9] — Encoder SW (ESC) |
| GP26 | LED2 B（右眼）|
| GP27 | LED2 G（右眼）|
| GP28 | LED2 R（右眼）|

### LED 接線（共陽極）

- 最長腳（COM）→ 3.3V
- R、G 腳各串 **220Ω** 電阻後接 GPIO
- B 腳串 **68Ω** 電阻後接 GPIO（藍光 Vf 較高，需降低電阻）
- 軟體邏輯：`LOW` = 亮，`HIGH` = 滅

### 去耦電容（實體焊接）

手邊有 3 顆 **10µF** 電容，分別焊接於：
- LED1 電源入口旁（3.3V ↔ GND）
- LED2 電源入口旁（3.3V ↔ GND）
- 編碼器旁（3.3V ↔ GND）

防止 LED 切換時的 Ground Bounce 干擾按鍵與編碼器訊號。

### 關鍵尺寸限制

旋鈕中心至 Enter 鍵中心距離必須 **> 27mm**，避免 35mm 旋鈕帽碰撞。

---

## 💡 四、 LED 狀態指示

兩顆鬼眼 LED 即時反映 Claude Code 的工作狀態：

| 狀態 | 顏色 | 說明 |
| :--- | :--- | :--- |
| Idle（待機）| 🟢 綠色 | 開機預設，無操作 |
| Thinking（思考中）| 🔵 藍色 | Claude 正在執行工具 |
| Completed（完成）| 🟢 綠色 | 任務完成或回覆結束 |
| Error（問題）| 🔴 紅色 | 工具失敗、權限拒絕、API 錯誤 |

---

## 💻 五、 安裝方法與步驟

### 步驟一：燒錄韌體（Arduino IDE）

1. 安裝 **Arduino IDE**（1.8 或 2.x）

2. 開啟 Preferences，在 **Additional Boards Manager URLs** 加入：
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```

3. 至 **Tools → Board → Boards Manager**，搜尋並安裝 `Raspberry Pi RP2040 Boards`

4. 至 **Sketch → Include Library → Manage Libraries**，搜尋並安裝 `Adafruit TinyUSB Library`

5. **Tools 設定：**
   - Board：`Raspberry Pi Pico`
   - USB Stack：`Adafruit TinyUSB`

6. 開啟 `firmware/claude_macropad/claude_macropad.ino`

7. **燒錄方式：** 按住 YD-RP2040 上的 **BOOT** 按鈕後插上 USB，放開後裝置會顯示為隨身碟，在 Arduino IDE 按 **Upload** 即可。

8. 燒錄成功後，鬼眼 LED 會閃爍三次藍光，然後轉為綠色待機。

### 步驟二：安裝 Python 依賴

```bash
pip install pyserial
```

### 步驟三：設定 COM Port

1. 插上 Macro Pad，開啟 **裝置管理員 → 連接埠 (COM 和 LPT)**，找到 RP2040 對應的 COM 編號（例如 COM4）

2. 修改 `hooks/led_hook.py` 第 19 行：
   ```python
   PORT = "COM4"  # 依實際裝置調整
   ```

### 步驟四：設定 Claude Code Hooks

將以下內容加入 `~/.claude/settings.json`（注意路徑改為本機實際路徑）：

```json
{
  "hooks": {
    "PreToolUse":         [{"matcher": "", "hooks": [{"type": "command", "command": "python d:\\projects\\claude_microPad\\hooks\\led_hook.py"}]}],
    "PostToolUse":        [{"matcher": "", "hooks": [{"type": "command", "command": "python d:\\projects\\claude_microPad\\hooks\\led_hook.py"}]}],
    "Stop":               [{"matcher": "", "hooks": [{"type": "command", "command": "python d:\\projects\\claude_microPad\\hooks\\led_hook.py"}]}],
    "PostToolUseFailure": [{"matcher": "", "hooks": [{"type": "command", "command": "python d:\\projects\\claude_microPad\\hooks\\led_hook.py"}]}],
    "StopFailure":        [{"matcher": "", "hooks": [{"type": "command", "command": "python d:\\projects\\claude_microPad\\hooks\\led_hook.py"}]}],
    "PermissionDenied":   [{"matcher": "", "hooks": [{"type": "command", "command": "python d:\\projects\\claude_microPad\\hooks\\led_hook.py"}]}]
  }
}
```

---

## 🎛️ 六、 自訂按鍵（keymap.h）

所有按鍵功能集中定義在 `firmware/claude_macropad/keymap.h`，**只需編輯此檔案，重新上傳韌體即可生效**，不需改動主程式。

### 按鍵索引對照

```
[ 8 ]                                        [ 9 ] ← 旋鈕按下
         [ 0 ]    [ 1 ]    [ 2 ]    [ 3 ]
         [ 4 ]    [ 5 ]    [ 6 ]    [ 7 ]
```

### 兩種按鍵模式

**模式一：組合鍵（BTN_KEYS）** — 送出單次按鍵或組合鍵

```c
const uint8_t BTN_KEYS[10][3] = {
  {SLASH, 0,      0},  // [0] /
  {SHIFT, TAB,    0},  // [1] Shift+Tab
  {WIN,   TAB,    0},  // [2] Win+Tab
  {ENTER, 0,      0},  // [3] Enter
  ...
};
```

**模式二：文字巨集（BTN_MACROS）** — 逐字輸出一串字元

```c
const char* BTN_MACROS[10] = {
  "",               // [0] 空字串 → 使用 BTN_KEYS
  "",               // [1] 空字串 → 使用 BTN_KEYS
  "/gitCommit\n\n", // [4] 輸出 /gitCommit 後按兩次 Enter
  "/clear\n\n",     // [5] 輸出 /clear 後按兩次 Enter
  "/gitPush\n\n",   // [9] 輸出 /gitPush 後按兩次 Enter（旋鈕）
  ...
};
```

> 若 `BTN_MACROS[i]` 非空字串，優先執行巨集，忽略 `BTN_KEYS[i]`。

### 可用按鍵常數

| 類別 | 常數 | 說明 |
| :--- | :--- | :--- |
| 修飾鍵 | `CTRL` `SHIFT` `ALT` `WIN` | Ctrl / Shift / Alt / Win |
| 方向鍵 | `UP` `DOWN` `LEFT` `RIGHT` | 上 / 下 / 左 / 右 |
| 特殊鍵 | `ENTER` `ESC` `TAB` `SPACE` `DEL` `BACKSPACE` `SLASH` | 常用功能鍵 |
| 字母鍵 | `L('a')` ~ `L('z')` | 例如 `L('e')` = E 鍵 |
| 數字鍵 | `N0` ~ `N9` | 例如 `N5` = 數字 5 |

> ⚠️ 不可直接用 `'c'` 或 `5`，ASCII 碼 ≠ USB HID 按鍵碼。

### 範例：改成自己的快捷鍵

```c
// 把 [3] 改成 Ctrl+Shift+P（命令選單）
{CTRL, SHIFT, L('p')},  // [3]

// 把 [6] 改成輸出自訂命令並送出
"/mycommand\n\n",  // [6] BTN_MACROS

// 把 [7] 改成單獨數字鍵
{N5, 0, 0},  // [7] BTN_KEYS（記得把 BTN_MACROS[7] 清空為 ""）
```

修改後，在 Arduino IDE 重新上傳韌體即可。

---

## ⚠️ 七、 製作提醒

1. **旋鈕高度差：** 雙層鍵帽比一般鍵帽略高，若旋鈕帽太矮難以轉動，可在旋鈕底部墊入 1–2mm 墊圈。
2. **標籤防褪色：** 有透明蓋保護，但長時間日光照射仍會褪色，建議多印幾份備用。
3. **線路固定：** 外殼半透明，內部焊接建議使用熱縮套管或熱熔膠固定，讓內視效果整齊。
4. **標籤尺寸：** 14mm × 14mm，上方功能名稱，下方小字快捷鍵，建議使用相片紙或名片紙。
5. **中文輸入法注意：** 巨集中的 `\n` 在開啟中文輸入法時可能被吃掉一個，建議使用 `\n\n` 確保指令送出。

---

## 📁 八、 專案檔案結構

```
claude_microPad/
├── README.md                          # 本文件
├── CLAUDE.md                          # Claude Code 專案指引
├── diagram.json                       # Wokwi 電路模擬圖
├── 按鈕佈局.drawio                     # 按鍵佈局設計圖（Draw.io）
├── 按鈕佈局.jpg                        # 按鍵佈局匯出圖
├── firmware/
│   └── claude_macropad/
│       ├── claude_macropad.ino        # Arduino 韌體主程式（勿需修改）
│       └── keymap.h                   # ✏️ 按鍵自訂設定（在這裡修改）
└── hooks/
    └── led_hook.py                    # Claude Code Hook 腳本（設定 COM port）
```

---

---

## 🤖 九、 自訂 Slash Command — `/gitPush`

本專案內建一個自訂 Claude Code 指令 `/gitPush`，對應鍵盤上的 **[4]** 號按鍵。

### 功能說明

按下按鍵後，Claude Code 會自動：

1. 收集所有未提交的 git 變更（`git status`、`git diff`、`git log`）
2. 產出一份**繁體中文**的變更摘要，並建議英文 commit 訊息
3. 顯示目前 git 帳號，**等待用戶確認**後才執行寫入
4. 逐一 `git add` 相關檔案 → `git commit` → `git push origin main`
5. 回報最新 commit hash

> 收到確認之前，不會執行任何 git 寫入操作。

### 指令檔案位置

```
.claude/commands/gitPush.md   ← 本專案專屬（僅此專案有效）
```

### 改為全域使用（所有專案都能用）

Claude Code 的 Slash Command 有兩個存放位置：

| 位置 | 路徑 | 有效範圍 |
| :--- | :--- | :--- |
| **專案層級** | `<專案目錄>/.claude/commands/` | 僅限該專案 |
| **全域層級** | `~/.claude/commands/` | 所有專案 |

將指令升級為全域只需複製檔案：

**Windows（PowerShell）**
```powershell
Copy-Item "d:\projects\claude_microPad\.claude\commands\gitPush.md" "$env:USERPROFILE\.claude\commands\gitPush.md"
```

**macOS / Linux（Bash）**
```bash
cp /path/to/claude_microPad/.claude/commands/gitPush.md ~/.claude/commands/gitPush.md
```

複製完成後，在任何專案的 Claude Code 中輸入 `/gitPush` 或按下鍵盤 [4] 號鍵，即可使用。

---

*文件更新日期：2026-04-18*
