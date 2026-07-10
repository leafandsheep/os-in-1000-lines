#include"user.h"//user.c是負責掌管使用者程式的流程 而shell.c則是提供使用者程式的main本體
extern char __stack_top[];

__attribute__((noreturn)) void exit(void){//表示直接跟編譯器說沒有return
    for(;;);
}  

void putchar(char ch){
    
}

__attribute__((section(".text.start")))//編譯時放到對應section
__attribute__((naked))
void start(void){//使用者程式的開始
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "call main\n"
        "call exit\n"
        :
        :[stack_top]"r"(__stack_top)
    );

}