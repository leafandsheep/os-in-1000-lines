#include "common.h"
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t; //從編譯器進行的名稱定應

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

//接下來來寫ch10的PCB 呼應OS的process相關內容
#define PROCS_MAX 8 //process數量最大值，用來開一個process table
#define PROC_UNUSED 0 //該位置空的沒有行程
#define PROC_RUNNABLE 1//該位置有行程執行中或是等待執行
struct process{
    int pid;
    int state;//Unused or Runnable
    vaddr_t sp;
    uint8_t stack[8192];
    uint32_t* page_table;//ch11加上的 是第一層table
};  

//ch10的yield宣告
void yield(void);

//ch11宣告PTE會用的巨集 注意左移31要unsigned
#define SATP_SV32 (1u<<31) //是開啟Sv32模式的位元 放在satp暫存器中
#define PAGE_V (1 <<0)//os學過的valid bit
#define PAGE_R (1<<1)//可讀
#define PAGE_W (1<<2)//可寫
#define PAGE_X (1<<3)//可執行
#define PAGE_U (1<<4)//user可使用

//ch11
void map_page(uint32_t* table1,uint32_t vaddr,paddr_t paddr,uint32_t flags );