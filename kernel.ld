ENTRY(boot)

SECTIONS{
    . = 0x80200000;
    __kernel_base = .; /*kernel程式起點位置*/

    .text :{
        KEEP(*(.text.boot));
        *(.text .text.*);
    }

    .rodata : ALIGN(4){
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4){
        *(.data .data.*);
    }

    .bss : ALIGN(4){
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .;
    }

    . = ALIGN(4);
    . += 128 * 1024;
    __stack_top = .; /*stack高位是底 低位是頂*/

    . = ALIGN(4096);
    __free_ram = .;
    . += 64*1024*1024;
    __free_ram_end = .;/* 依據kernel大小變化 一定排在後面不寫死*/



}