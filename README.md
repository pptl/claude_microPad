# Claude Ghost Macro Pad 專案計畫書 (V2.0 雙層鍵帽版)

這是一台專為 **Claude Code** 開發流程優化的左手專用巨集鍵盤。本版本採用 **雙層透明鍵帽 (Relegendable Keycaps)** 方案，透過紙條替換實現快速客製化，無需轉印與噴漆。

外殼採鬼魂造型，橙色半透明壓克力，正面印有 **ANTHROPIC** 字樣，頂部兩顆 RGB LED 作為「鬼眼」狀態燈。

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

---

## ⌨️ 二、 鍵盤佈局

根據 2026-04-03 最新配置，全按鍵均為 1U 規格：

| 位置 | 功能 | 快捷鍵 |
| :--- | :--- | :--- |
| 左上角獨立鍵 | Terminate | Ctrl+C |
| 右上角旋鈕 | Knob / ESC | 旋轉 = 捲動 / 按壓 = ESC |
| 矩陣上排 1 | Change Permission Mode | Shift+Tab |
| 矩陣上排 2 | Explain | Ctrl+E |
| 矩陣上排 3 | UP | ↑ |
| 矩陣上排 4 | Enter | Enter |
| 矩陣下排 1 | Show Todo | Ctrl+T |
| 矩陣下排 2 | / (Slash Command) | / |
| 矩陣下排 3 | DOWN | ↓ |
| 矩陣下排 4 | Switch Window | Win+Tab |

---

## 🔌 三、 電路設計
### WOKWI可查看電路設計圖
將diagram.json的内容全部貼到WOKWI上可以查看整體電路設計圖

### GPIO 接腳對照

| GPIO | 功能 |
| :--- | :--- |
| GP0 | Terminate (Ctrl+C) |
| GP1 | Change Permission Mode (Shift+Tab) |
| GP2 | Explain (Ctrl+E) |
| GP3 | UP |
| GP4 | Enter |
| GP5 | Show Todo (Ctrl+T) |
| GP6 | / (Slash) |
| GP7 | DOWN |
| GP8 | Switch Window (Win+Tab) |
| GP9 | Encoder SW → ESC |
| GP10 | Encoder CLK |
| GP11 | Encoder DT |
| GP12 | LED1 R（左眼）|
| GP13 | LED1 G（左眼）|
| GP14 | LED1 B（左眼）|
| GP16 | LED2 R（右眼）|
| GP17 | LED2 G（02  200600右眼）|
| GP18 | LED2 B（右眼）|

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

## 💻 五、 韌體與軟體

### 韌體（Arduino IDE）

使用 **Arduino IDE** + **Earle Philhower arduino-pico** 套件燒錄。

**安裝 Boards Manager URL：**
```
https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
```

**Tools 設定：**
- Board：`Resberry Pi Pico`
- USB Stack：`Adafruit TinyUSB`

**需安裝 Library：**
- `Adafruit TinyUSB Library`

韌體檔案：`firmware/claude_macropad/claude_macropad.ino`

**燒錄方式：** 按住 BOOT 按鈕插上 USB，放開後於 Arduino IDE 按 Upload。

### Hook 腳本（主機端）

**安裝 pyserial：**
```
pip install pyserial
```

修改 `hooks/led_hook.py` 第 16 行，填入 RP2040 在裝置管理員中的 COM port：
```python
PORT = "COM4"  # 依實際裝置調整
```

### Claude Code Hooks 設定

加入 `~/.claude/settings.json`：

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

## ⚠️ 六、 製作提醒

1. **旋鈕高度差：** 雙層鍵帽比一般鍵帽略高，若旋鈕帽太矮難以轉動，可在旋鈕底部墊入 1–2mm 墊圈。
2. **標籤防褪色：** 有透明蓋保護，但長時間日光照射仍會褪色，建議多印幾份備用。
3. **線路固定：** 外殼半透明，內部焊接建議使用熱縮套管或熱熔膠固定，讓內視效果整齊。
4. **標籤尺寸：** 14mm × 14mm，上方功能名稱，下方小字快捷鍵，建議使用相片紙或名片紙。

---

## 📁 七、 專案檔案結構

```
claude_microPad/
├── README.md                          # 本文件
├── CLAUDE.md                          # Claude Code 專案指引
├── diagram.json                       # Wokwi 電路模擬圖
├── 按鈕佈局.drawio                     # 按鍵佈局設計圖（Draw.io）
├── 按鈕佈局.jpg                        # 按鍵佈局匯出圖
├── 概念圖.jpg                          # 外觀概念圖
├── buttonIcon/                        # 各按鍵 PNG 圖示
├── firmware/
│   └── claude_macropad/
│       └── claude_macropad.ino        # Arduino 韌體
└── hooks/
    └── led_hook.py                    # Claude Code Hook 腳本
```

---

*文件更新日期：2026-04-05*
