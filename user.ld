ENTRY(start)

SECTIONS{
    . = 0x1000000;/*虛擬位址*/

    .text :{
        KEEP(*(.text.start));
        *(.text .text.*);
    }

    .rodata : ALIGN(4){
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4){
        *(.data .data.*);
    }

    .bss : ALIGN(4){
        *(.bss .bss.* .sbss .sbss.*);
        . = ALIGN(16);
        . += 64 * 1024;
        __stack_top = .; /*stack高位是底 低位是頂*/

        ASSERT(. < 0x1800000, "too large executable");/*超過的話linking直接報錯*/
    }
}