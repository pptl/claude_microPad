自動總結本專案的 git 變更、列出清單，並在用戶確認後 push 到遠端。

執行以下步驟：

## 步驟 1 — 收集變更資訊

先用 Bash 執行 `pwd` 取得目前專案目錄路徑，之後所有指令皆在該目錄下執行。

用 Bash 工具**平行**執行以下指令：

- `git status` — 取得所有未提交的變更
- `git diff HEAD` — 取得完整 diff（含已 staged 與未 staged）
- `git log --oneline -5` — 取得最近 5 筆 commit，了解此 repo 的 commit 訊息風格
- `git stash list` — 確認是否有 stash

## 步驟 2 — 分析並整理變更

根據 diff 和 status，產出一份**繁體中文**的變更摘要，格式如下：

```
📦 變更摘要

**已修改的檔案：**
- `path/to/file` — 一句話說明這個檔案改了什麼、為什麼

**新增的檔案：**
- `path/to/file` — 說明用途

**刪除的檔案：**
- `path/to/file` — 說明原因

**建議的 commit 訊息：**
`<type>: <簡短描述（英文）>`

詳細說明（如有必要）：
- ...
```

- commit 訊息**用英文**，格式遵循此 repo 慣例（`feat:` / `fix:` / `chore:` 等）
- 每個檔案的說明**用繁體中文**，聚焦在「改了什麼、為什麼」

## 步驟 3 — 詢問確認

顯示完整摘要前，先用 Bash 執行以下指令取得目前 git 帳號資訊：

- `git config user.name` — 取得使用者名稱
- `git config user.email` — 取得使用者 Email

在摘要最後顯示：

```
👤 目前 Git 帳號：<user.name> (<user.email>)
```

然後問用戶：

> 「以上是本次的變更摘要。確認要 commit 並 push 嗎？可直接回覆「確認」，或告訴我要修改 commit 訊息。」

**收到確認之前，不可執行任何 git 寫入操作。**

## 步驟 4 — 執行 commit 與 push（只在用戶確認後）

1. `git add` 所有相關變更的檔案（**不用** `git add -A`，逐一列出檔案以避免加入敏感檔）
2. 用 HEREDOC 格式執行 `git commit -m`，commit 訊息結尾附上：
   ```
   Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
   ```
3. `git push origin main`
4. 回報 push 成功，並顯示最新的 commit hash

## 步驟 5 — 例外處理

- **沒有任何變更**：告知用戶「目前沒有未提交的變更，工作目錄是乾淨的。」
- **有 untracked 的敏感檔案**（`.env`、`*secret*`、`*credential*`）：警告用戶，不將其加入 commit
- **push 失敗**：顯示錯誤訊息，建議用戶先執行 `git pull --rebase` 解決衝突

## 注意事項

- **永遠不要** force push（`--force`）
- **永遠不要** 跳過 hooks（`--no-verify`）
- 若 commit 訊息由用戶提供，使用用戶的版本，不要自行修改
