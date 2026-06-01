# 從零開始寫作業系統

> 跟著 [Writing an OS in 1,000 Lines](https://operating-system-in-1000-lines.vercel.app/tw/) 教材，用約 1000 行 C 程式碼在 RISC-V 架構上從頭實作一個作業系統。

---

## 為什麼做這個

二下學期修作業系統，並找到可以實作的專案，因此紀錄follow教學以學習作業系統實作的學習歷程

---

## 學習目標

親手實作一個具備以下功能的迷你 OS：

- **啟動流程**：從韌體到 kernel 的完整開機序列
- **記憶體管理**：頁面配置器、Sv32 分頁機制
- **行程管理**：context switch、cooperative multitasking
- **例外與系統呼叫**：trap handler、U-mode / S-mode 切換
- **裝置驅動**：virtio-blk 虛擬磁碟驅動
- **檔案系統**：基於 tar 格式的完整讀寫流程

---

## 技術環境

| 項目 | 內容 |
|------|------|
| 目標架構 | RISC-V 32-bit (rv32) |
| 模擬器 | QEMU virt machine |
| 編譯器 | Clang + lld (LLVM) |
| 韌體 | OpenSBI |
| 語言 | C、RISC-V Assembly |

---

## 學習進度

| # | 章節 | 狀態 | 完成日期 |
|---|------|------|----------|
| 00 | 簡介 | ✅ | 2026-05-29 |
| 01 | 環境建置 | ✅ | 2026-06-01 |
| 02 | RISC-V 101 | ✅ | 2026-06-01 |
| 03 | 架構總覽 | ⬜ | — |
| 04 | 啟動（Boot） | ⬜ | — |
| 05 | Hello World | ⬜ | — |
| 06 | C 標準函式庫 | ⬜ | — |
| 07 | Kernel Panic | ⬜ | — |
| 08 | 例外處理 | ⬜ | — |
| 09 | 記憶體配置 | ⬜ | — |
| 10 | 行程 | ⬜ | — |
| 11 | 分頁表 | ⬜ | — |
| 12 | 應用程式 | ⬜ | — |
| 13 | 使用者模式 | ⬜ | — |
| 14 | 系統呼叫 | ⬜ | — |
| 15 | VirtIO 磁碟 | ⬜ | — |
| 16 | 檔案系統 | ⬜ | — |
| 17 | 總結 | ⬜ | — |

---

## 記錄方式

每次學習有兩種記錄：

- **每日日誌** (`journal/daily/`)：當天學了什麼、卡住哪裡、有什麼理解突破
- **章節筆記** (`journal/chapters/`)：每章完成後整理核心概念、程式碼重點、與其他章節的連結

---

## 參考資源

- 教材：[OS in 1,000 Lines 繁體中文版](https://operating-system-in-1000-lines.vercel.app/tw/)
- 作者原始碼：[nuta/operating-system-in-1000-lines](https://github.com/nuta/operating-system-in-1000-lines)

