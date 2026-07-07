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

