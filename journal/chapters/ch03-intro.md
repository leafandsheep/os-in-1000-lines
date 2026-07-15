本張說明本次實作會實作的項目（附圖）：包含多工、例外處理、paging、system call、filesystem，都是OS課程學到的重要內容。另外其實可以看到為了減少複雜度，本次並沒有實作也是OS裡經典的IPC 和中斷處理。

```
├── disk/     - 檔案系統內容
├── common.c  - 核心／使用者通用函式：printf、memset 等
├── common.h  - 核心／使用者通用定義：結構與常數定義
├── kernel.c  - 核心程式碼：行程管理、系統呼叫、驅動程式、檔案系統
├── kernel.h  - 核心專用定義檔：結構與常數定義
├── kernel.ld - 核心的 linker script（記憶體配置）
├── shell.c   - 命令列 Shell
├── user.c    - 使用者程式庫：系統呼叫介面
├── user.h    - 使用者程式庫定義：結構與常數
├── user.ld   - 使用者程式的 linker script（記憶體配置）
└── run.sh    - 建置腳本（Build script）
```

