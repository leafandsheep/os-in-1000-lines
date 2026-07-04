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