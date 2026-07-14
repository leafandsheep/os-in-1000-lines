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

//ch13告訴我們user.ld最開始的位置
#define USER_BASE 0x1000000
//ch13sstatus暫存器中第五位是SPIE 於是define
#define SSTATUS_SPIE (1<<5)
//ch14定義syscall 的scause值
#define SCAUSE_ECALL 8
void handle_syscall(struct trap_frame* f);
long getchar(void);
#define PROC_EXITED 2 //這是process進入終止的狀態

//ch15 我們加入irtio相關的定義
//以下是想放的值不是位址
#define SECTOR_SIZE 512//一個扇區幾個byte
#define VIRTQ_ENTRY_NUM 16 //開16格descriptor
#define VIRTIO_DEVICE_BLK 2 //裝置的種類是磁碟
#define VIRTIO_BLK_PADDR 0x10001000 //MMIO的基底位址
//以下是基於基底的偏移量 用來算暫存器位址
#define VIRTIO_REG_MAGIC 0x00//實際是偏移量 從基底去加上 做確認是virtio裝置用
#define VIRTIO_REG_VERSION 0x04//virtio 的協定版本
#define VIRTIO_REG_DEVICE_ID 0x08//裝置類型
#define VIRTIO_REG_PAGE_SIZE 0x28//guest一頁多大（應為4096）用來算頁號 virtqueue的位址
#define VIRTIO_REG_QUEUE_SEL 0x30//選第幾個virtqueue 但這邊情境是只有一個
#define VIRTIO_REG_QUEUE_NUM_MAX 0x34//最多幾個descriptor
#define VIRTIO_REG_QUEUE_NUM  0x38//想開幾個desc
#define VIRTIO_REG_QUEUE_PFN 0x40//寫virtqueue的頁號
#define VIRTIO_REG_QUEUE_READY  0x44//就緒標記
#define VIRTIO_REG_QUEUE_NOTIFY  0x50//索引放好後去通知裝置來avail ring拿
#define VIRTIO_REG_DEVICE_STATUS 0x70//握手時會對狀態做更改
#define VIRTIO_REG_DEVICE_CONFIG 0x100//磁碟總共有幾個扇區
#define VIRTIO_STATUS_ACK 1//握手的狀態值
#define VIRTIO_STATUS_DRIVER 2//握手的狀態值
#define VIRTIO_STATUS_DRIVER_OK 4//握手的狀態值
#define VIRTQ_DESC_F_NEXT 1 //三個一組的descriptor中會設定前兩個有next 最後一個不設
#define VIRTQ_DESC_F_WRITE 2//裝置能不能寫 data status有時需要裝置寫
#define VIRTQ_AVAIL_F_NO_INTERRUPT 1//結束後cpu自己檢查
#define VIRTIO_BLK_T_IN  0//放在header說要讀
#define VIRTIO_BLK_T_OUT 1//放在header說要寫

// Virtqueue Descriptor Table entry.
struct virtq_desc {//descriptor的結構
    uint64_t addr;//緩衝區的物理位址，裡面放header/data/status descs是一個長度3的desc陣列 裡面放desc[0].addr = &header的概念
    uint32_t len;//緩衝區長度
    uint16_t flags;//flags(F_NEXT或是F_WRITe)
    uint16_t next;//指向下一個desc物理位址
} __attribute__((packed));//不padding

// Virtqueue Available Ring.
struct virtq_avail {//一個ring就是共用16大小descs
    uint16_t flags;//這個ring的設定 例如不發中段訊號
    uint16_t index;//這個ring目前放了幾個請求
    uint16_t ring[VIRTQ_ENTRY_NUM];//放索引頭的陣列 有幾條鏈 就有幾個頭 像是ring[1]=1表示有一條鏈的頭是descs[1]
} __attribute__((packed));

// Virtqueue Used Ring entry.
struct virtq_used_elem {
    uint32_t id;//那條完成的鏈的頭索引號碼
    uint32_t len;//device寫的長度(一個chain的總和)
} __attribute__((packed));

// Virtqueue Used Ring.
struct virtq_used {
    uint16_t flags;//device設的flag
    uint16_t index;//完成幾個 一路加上去
    struct virtq_used_elem ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));

// Virtqueue.
struct virtio_virtq {///三個環放在一起的最外層 最高層期
    //這邊是driver和device共用的
    struct virtq_desc descs[VIRTQ_ENTRY_NUM];//descs table
    struct virtq_avail avail;//下單的環
    struct virtq_used used __attribute__((aligned(PAGE_SIZE)));//以完成的環 要對齊
    
    //這邊是driver自己用的
    int queue_index;//第幾個queue
    volatile uint16_t *used_index;//指向used_index的指標，用volatile表示每次都要重讀
    uint16_t last_used_index;//driver自己的進度 用來和上面used_index(device)的進度比較
} __attribute__((packed));

// Virtio-blk request.
struct virtio_blk_req {
    uint32_t type;//要讀還是寫
    uint32_t reserved;
    uint64_t sector;//要存取的扇區編號
    uint8_t data[512];//緩衝區
    uint8_t status; //device回報是否執行成功寫這裡
} __attribute__((packed));

//ch15初始化virtq宣告
struct virtio_virtq* virtq_init(unsigned index);

//ch16定義tar檔案系統相關的結構與參數
#define FILES_MAX 2 //本次最多放兩個檔案在disk
#define DISK_MAX_SIZE align_up(sizeof(struct file) * FILES_MAX, SECTOR_SIZE)//去算disk要有奪大
struct tar_header {//磁碟上 為了滿足ustar格式
    char name[100];//檔名
    char mode[8];//權限
    char uid[8];//擁有者userid
    char gid[8];//擁有者的groupid
    char size[12];//純data檔案大小
    char mtime[12];//修改時間
    char checksum[8];//把header的所有值做加總
    char type;//檔案類型
    char linkname[100];
    char magic[6];//ustar格式識別
    char version[2];//版本
    char uname[32];//擁有者名字
    char gname[32];//擁有者群組名字
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];//把struct長度湊到512
    char data[];//開個指標指向struct最頂部
} __attribute__((packed));

struct file {//記憶體 kerbel用的格式
    bool in_use;//這個file格子有人了
    char name[100]; //檔名
    char data[1024]; //檔站內容
    size_t size; //檔案大小
};

//ch16函式宣告
void fs_flush(void);
struct file* fs_lookup(const char* filename);

//ch16 sstatus的問題 先定義SUM參數
#define SSTATUS_SUM  (1 << 18) //對應開關位址

