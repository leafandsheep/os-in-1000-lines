#include "common.h"

struct trap_frame{//傳入trap時狀態的結構物件
    uint32_t ra;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t  s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t  s10;
    uint32_t s11;
    uint32_t sp;
}__attribute__((packed));//不要填充
//以下是ch8實作csr讀取寫入的巨集（因為csr是在編譯時就要被確認）
#define READ_CSR(reg)\
    ({\
        unsigned long __tmp;\
        __asm__ __volatile__("csrr %0," #reg :"=r"(__tmp));\
        __tmp;\
    })

#define WRITE_CSR(reg,value)\
    do{\
        uint32_t __tmp = (value);\
        __asm__ __volatile__("csrw " #reg " ,%0" ::"r"(__tmp));\
    }while(0)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t; //從編譯器進行的名稱定應

extern char __bss[] , __bss_end[], __stack_top[] ; //引用ld的變數

struct sbiret{
    long error;//表示是否成功0是成功 負數是失敗種類
    long value;//代表sbi_call回傳的資料
};

struct sbiret sbi_call(long arg0, long arg1, long arg2,long arg3,long arg4,long arg5 ,long fid,long eid);
void putchar(char c);

//這邊實作ch7的panic巨集 因為這樣行號才能呼叫對
#define PANIC(fmt, ...)do{\
    printf("PANIC: %s:%d: "fmt"\n",__FILE__,__LINE__,##__VA_ARGS__);\
    while(1){}\
    }while(0)
//這裡用do-while是為了解決拆巨集時導致的編譯器文法錯誤
//＃＃是為了避免沒有傳入參數

void kernel_entry(void);