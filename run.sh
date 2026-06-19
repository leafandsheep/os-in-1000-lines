#/bin/bash
set -xue

QEMU=qemu-system-riscv32

#要加入載入核心的程式
#首先是給訂flag
CC=/opt/homebrew/opt/llvm/bin/clang 
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib"
#接著編譯kernel
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf kernel.c 
#map可以直接看到記憶體位置是怎麼樣被分配的 ps.elf=Executable and linkeable Format
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf