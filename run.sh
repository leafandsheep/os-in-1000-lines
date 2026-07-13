#/bin/bash
set -xue

QEMU=qemu-system-riscv32

#要加入載入核心的程式
#首先是給訂flag
CC=/opt/homebrew/opt/llvm/bin/clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib"

#ch12更新run.sh
OBJCOPY=/opt/homebrew/opt/llvm/bin/llvm-objcopy

#編譯shell的指令
$CC $CFLAGS -Wl,-Tuser.ld -Wl,-Map=shell.map -o shell.elf shell.c user.c common.c
#以下這行是把.elf處理成.bin 而且.bss這個section要填充有位置的0
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin
#以下這行因為linker要吃.o檔 要把 .bin轉成elf(但是本質還是rawbin) 順便提醒riscv是little-end
$OBJCOPY -Ibinary -Oelf32-littleriscv shell.bin shell.bin.o

#接著編譯kernel
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf kernel.c common.c shell.bin.o
#map可以直接看到記憶體位置是怎麼樣被分配的 ps.elf=Executable and linkeable Format

$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
-drive id=drive0,file=lorem.txt,format=raw,if=none \
-device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
-kernel kernel.elf
#掛上這個檔案，raw代表純資料，if是先不接介面
#使用這種類型的虛擬硬碟掛載以及指定匯流排