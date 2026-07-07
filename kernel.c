#include"kernel.h"
#include"common.h"
//接下來要寫三個函式：1.memset 用來清空bss2.kernel_main 核心主要運行內容3. boot
//依照順序使用到前者





//然後寫putchar要用的sbi_call sbi_call是依據不同參數呼叫ecall的函式
struct sbiret sbi_call(long arg0, long arg1, long arg2,long arg3,long arg4,long arg5 ,long fid,long eid){//參數用long 表示參數size跟隨word大小 其中eid表示某一類指令 再根據fid鎖定 
    register long a0 __asm__("a0")=arg0; //a0是ecall指定的暫存器位置
    register long a1 __asm__("a1")=arg1; //a1是ecall指定的暫存器位置
    register long a2 __asm__("a2")=arg2; //a2是ecall指定的暫存器位置
    register long a3 __asm__("a3")=arg3; //a3是ecall指定的暫存器位置
    register long a4 __asm__("a4")=arg4; //a4是ecall指定的暫存器位置
    register long a5 __asm__("a5")=arg5; //a5是ecall指定的暫存器位置
    register long a6 __asm__("a6")=fid; //
    register long a7 __asm__("a7")=eid; 
    //接下來寫組合語言呼叫ecall
    __asm__ __volatile__(
        "ecall\n"//只執行ecall ecall呼叫handler後 handler去讀暫存器的值
        :"+r"(a0), "+r"(a1) //sbi輸出a0放error a1放回傳資料 用+r是因為又讀又寫
        :"r"(a2),"r"(a3),"r"(a4),"r"(a5),"r"(a6),"r"(a7)//只讀
        :"memory"//mem有可能在ecall的過程被更動 要重新取
    );

    return (struct sbiret){.error=a0,.value = a1};
    

}

void putchar(char c){//包裝去使用sbi_call而sbi_call去使用ecall
    sbi_call(c,0,0,0,0,0,0,1);
}


void kernel_main(void){
    memset(__bss,0,(size_t)__bss_end-(size_t)__bss);//清理bss空間

    PANIC("PANIC test");//ch07實作PANIC跑看看


    const char* s = "\n\nHello, World!\n";//ch5 輸出helloWorld
    for(const char*p =s;*p!='\0';p++){
        putchar(*p);//要去實作輸出單個字元的函式 利用sbi_call
    }

    //接下來測試ch5中的printf
    int a=6;
    printf("1+2+3=%d\nyou%s\nhex:0x%x\n",a,"good",0xff);

    for(;;){
        __asm__ __volatile__("wfi");//cpu低效能待機狀態，特權指令wait for interrupt
    }
    
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