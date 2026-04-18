依照以下 SOP 執行 gitPush 流程，將本地 commit 推送至遠端。

---

## 第一階段：預檢機制

用 Bash **平行**執行以下指令：

- `git status --porcelain` — 偵測未提交的變更
- `git branch --show-current` — 取得目前分支名稱
- `git config user.name` 與 `git config user.email` — Git 帳號身份
- `git log --oneline -5` — 取得 commit 訊息風格供後續摘要參考
- `git rev-parse --abbrev-ref @{u} 2>/dev/null || echo "NO_UPSTREAM"` — 偵測是否有遠端追蹤分支

**若 git status --porcelain 輸出不為空，立即中斷：**

> **[系統提示] 檢測到工作區仍有未提交的變更 (Uncommitted changes detected)。**
> 為了維護版本紀錄的嚴謹性，請先完成 `commit` 提交或使用 `git stash` 暫存變更後，再執行 `gitPush` 流程。

**若偵測結果為 `NO_UPSTREAM`（首次推送新分支），跳至「首次推送流程」。**

---

## 第二階段：狀態彙整與環境資訊

顯示以下資訊供用戶核對：

```
👤 Git 帳號：<user.name> (<user.email>)
🌿 目前分支：<branch-name>
```

執行 `git log --oneline @{u}..HEAD` 列出所有待推送的 Commit。

若目前分支為 `main` 或 `master`，額外顯示：

> **[警告] 您目前正在保護分支操作，請再次確認後繼續。**

---

## 第三階段：合併策略與摘要生成

根據待推送的 Commit 標題（不檢索 diff）生成建議摘要，**風格須參考第一階段取得的 `git log` 慣例**（`feat:` / `fix:` / 中文等）。

顯示格式如下：

```
[推送準備] 偵測到 N 個待推送的 Commit：
- `<commit 1>`
- `<commit 2>`
- ...

建議的綜合提交訊息 (Combined Message)：
`<綜合後的訊息>`

請確認操作：
1. 確認並合併推送（預設）
2. 編輯訊息後推送
3. 不合併，直接按原樣推送
```

若只有 1 個待推送的 Commit，跳過合併，直接詢問是否確認推送（不顯示選項 1/2）。

**收到用戶確認之前，不可執行任何 git 寫入操作。**

---

## 第四階段：執行與結果回饋

### 選項 1 / 2（合併推送）

執行前顯示不可逆警告：

> **[警告] Squash 操作將以單一 Commit 取代目前的 N 個 Commit，此操作在推送後無法還原。**
> 已自動建立備份分支：`backup/<branch>-<YYYYMMDD>`，可隨時用於復原。

執行步驟：
1. `git branch backup/<branch>-<YYYYMMDD>` — 建立備份分支
2. `git merge-base HEAD @{u}` — 找出遠端基準點 hash
3. `git reset --soft <基準點 hash>` — 將所有 commit 壓回暫存區
4. 用 HEREDOC 格式執行 `git commit -m "<綜合訊息>"`（不加 Co-Authored-By）
5. `git push`

若 `git push` 失敗，立即提示：

> **[警告] 推送失敗，您的原始 Commit 已保存於備份分支 `backup/<branch>-<YYYYMMDD>`。**
> 執行 `git reset --hard backup/<branch>-<YYYYMMDD>` 可完整復原。

### 選項 3（原樣推送）

直接執行 `git push`。

成功後回報：

> **[成功] 已將更新推送至遠端分支 [分支名稱]。**

---

## 首次推送流程（NO_UPSTREAM）

偵測到分支尚未設定遠端追蹤時，顯示：

```
[首次推送] 此分支尚未設定遠端追蹤分支。
將執行：git push -u origin <branch>

確認推送？（Y / N）
```

收到確認後，執行 `git push -u origin <branch>`，**跳過合併策略（不執行 Squash）**。

---

## 異常處理

- **推送被拒絕 (rejected)：** 提示「遠端倉庫有更新，請先執行 `git pull` 進行同步」
- **沒有待推送的 Commit：** 告知「目前沒有領先遠端的 Commit，無需推送。」
- **永遠不要** 跳過 hooks（`--no-verify`）或使用 `--force`，除非用戶明確要求
