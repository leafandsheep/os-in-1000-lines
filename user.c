#include"user.h"//user.c是負責掌管使用者程式的流程 而shell.c則是提供使用者程式的main本體
extern char __stack_top[];

__attribute__((noreturn)) void exit(void){//表示直接跟編譯器說沒有return
    syscall(SYS_EXIT,0,0,0);//trap進kernel 走systemcall
    for(;;);
}  
int syscall(int sysno,int arg0,int arg1,int arg2){//ch14的systemcall函式
    register int a0 __asm__("a0")= arg0;
    register int a1 __asm__("a1")= arg1;
    register int a2 __asm__("a2")= arg2;
    register int a3 __asm__("a3")= sysno;//要在後面handle_syscall函式來讀取這些暫存器

    __asm__ __volatile__(
        "ecall\n"//觸發後存csr然後跳kernel_entry
        :"=r"(a0)
        :"r"(a0),"r"(a1),"r"(a2),"r"(a3)
        :"memory"
    );

    return a0;//回傳值放a這個return a0之前是kernel_entry lw回來的a0
}

//ch14
void putchar(char ch){
    syscall(SYS_PUTCHAR,ch,0,0);//這裡就是user用的putchar
}
int getchar(void){
    return syscall(SYS_GETCHAR,0,0,0);//現在要去實作第二種syscall指令
}

//ch16做user用的readfile和writefile函式
int readfile(const char* filename, char* buf,int len) {//函數回傳的是讀多少byte
    return syscall(SYS_READFILE, (int) filename, (int) buf, len);//這裡len是disk緩衝區大小
}

int writefile(const char* filename, const char* buf, int len) {//函數回傳的是寫多少byte
    return syscall(SYS_WRITEFILE, (int) filename, (int) buf, len);
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