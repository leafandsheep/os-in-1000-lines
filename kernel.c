typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t; //從編譯器進行的名稱定應

extern char __bss[] , __bss_end[], __stack_top[] ; //引用ld的變數

//接下來要寫三個函式：1.memset 用來清空bss2.kernel_main 核心主要運行內榮3. boot
//依照順序使用到前者


void *memset(void *buf, char c, size_t n){//函數型別是萬用指標
    uint8_t *p = (uint8_t *) buf;
    while(n--){
        *p = c;
        p++;
    }
    return buf;
}

void kernel_main(void){
    memset(__bss,0,(size_t)__bss_end-(size_t)__bss);//清理bss空間

    for(;;); //保持在這
}

__attribute__((section(".text.boot")))//要放在最一開始
__attribute__((naked))//裸函式 不用先去開stack(應為還沒有)
//加上以上兩個帽子之後 可以開始寫 boot
void boot (void){
    __asm__ __volatile__(//計算機組織上見過的寫法
        "mv sp, %[stack_top]\n"//把sp給值
        "j kernel_main\n"//然後執行kernel_main

        ://輸出無
        :[stack_top] "r" (__stack_top) //[stack_top]暫存器讀__stack_top這個c變數的值

    );
}