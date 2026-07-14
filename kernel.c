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

__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void){
    __asm__ __volatile__(//這邊一樣練習祭祖教過的操作
        /*"csrw sscratch, sp\n"//把sp複製到sscratch暫存 因為等等要跳sp */
        "csrrw sp,sscratch,sp\n"//把process sp換到要的process的kernel stack頂
        "addi sp, sp, -4*31\n"//開31個空間存通用暫存器的原始值 並不要讓例外處理時影響原本狀態
        "sw ra, 4*0(sp)\n"
        "sw gp, 4*1(sp)\n"
        "sw tp, 4*2(sp)\n"
        "sw t0, 4*3(sp)\n"
        "sw t1, 4*4(sp)\n"
        "sw t2, 4*5(sp)\n"
        "sw t3, 4*6(sp)\n"
        "sw t4, 4*7(sp)\n"
        "sw t5, 4*8(sp)\n"
        "sw t6, 4*9(sp)\n"
        "sw a0, 4*10(sp)\n"
        "sw a1, 4*11(sp)\n"
        "sw a2, 4*12(sp)\n"
        "sw a3, 4*13(sp)\n"
        "sw a4, 4*14(sp)\n"
        "sw a5, 4*15(sp)\n"
        "sw a6, 4*16(sp)\n"
        "sw a7, 4*17(sp)\n"
        "sw s0, 4*18(sp)\n"
        "sw s1, 4*19(sp)\n"
        "sw s2, 4*20(sp)\n"
        "sw s3, 4*21(sp)\n"
        "sw s4, 4*22(sp)\n"
        "sw s5, 4*23(sp)\n"
        "sw s6, 4*24(sp)\n"
        "sw s7, 4*25(sp)\n"
        "sw s8, 4*26(sp)\n"
        "sw s9, 4*27(sp)\n"
        "sw s10, 4*28(sp)\n"
        "sw s11, 4*29(sp)\n"
        //最後記得存原始sp
        "csrr a0,sscratch\n"//此處可以用a0因為我們已經存好a0了
        "sw a0,4*30(sp)\n"//放a0的是user process的sp

        //接下來是ch10做的更新 重點是sscratch的重新裝填程stack頂的位址
        "addi a0,sp,4*31\n"
        "csrw sscratch, a0\n"

        //接下來給Ｃ寫的handle_trap處理 先把新sp這個參數放a0
        "mv a0, sp\n"
        "call handle_trap\n"//下面我們會實作handle_trap 函式

        //處理完例外後暫存器載回原來狀態
        "lw ra, 4*0(sp)\n"
        "lw gp, 4*1(sp)\n"
        "lw tp, 4*2(sp)\n"
        "lw t0, 4*3(sp)\n"
        "lw t1, 4*4(sp)\n"
        "lw t2, 4*5(sp)\n"
        "lw t3, 4*6(sp)\n"
        "lw t4, 4*7(sp)\n"
        "lw t5, 4*8(sp)\n"
        "lw t6, 4*9(sp)\n"
        "lw a0, 4*10(sp)\n"
        "lw a1, 4*11(sp)\n"
        "lw a2, 4*12(sp)\n"
        "lw a3, 4*13(sp)\n"
        "lw a4, 4*14(sp)\n"
        "lw a5, 4*15(sp)\n"
        "lw a6, 4*16(sp)\n"
        "lw a7, 4*17(sp)\n"
        "lw s0, 4*18(sp)\n"
        "lw s1, 4*19(sp)\n"
        "lw s2, 4*20(sp)\n"
        "lw s3, 4*21(sp)\n"
        "lw s4, 4*22(sp)\n"
        "lw s5, 4*23(sp)\n"
        "lw s6, 4*24(sp)\n"
        "lw s7, 4*25(sp)\n"
        "lw s8, 4*26(sp)\n"
        "lw s9, 4*27(sp)\n"
        "lw s10, 4*28(sp)\n"
        "lw s11, 4*29(sp)\n"
        "lw sp, 4*30(sp)\n"
        "sret\n"//很重要 因為是stvec進來的 不是存ra要從sepc回去 
        
    );
}
void handle_trap(struct trap_frame* f){//讀取三個暫存器的值後報錯停機
    uint32_t scause = READ_CSR(scause);
    uint32_t stval = READ_CSR(stval);
    uint32_t user_pc = READ_CSR(sepc);//例外前執行到哪

    //ch14 因應user trap進kernel要調用syscall 所以對trap種類做個判斷 去判scause的值
    if(scause == SCAUSE_ECALL){
        handle_syscall(f);//f從a0讀的
        user_pc+=4;//因為回到trap點已經執行完了 可以直接下一個
    }
    else {
        PANIC("unexpected trap scause =%x, stval=%x, sepc=%x\n",scause , stval,user_pc);
    }
    WRITE_CSR(sepc, user_pc);//更新sepc
    
}
//接下來實作分配n頁的函式ch9
extern char __free_ram[],__free_ram_end[];//先引用ld的對應位置
paddr_t alloc_pages(uint32_t n){
    static paddr_t next_paddr = (paddr_t)__free_ram; // 用static因為不能每次呼叫都從頭開始
    paddr_t paddr = next_paddr;//紀錄等等要回傳的位置
    next_paddr +=n*PAGE_SIZE;
    //再做個超過自由記憶體容量的檢查
    if(next_paddr>(paddr_t)__free_ram_end){
        PANIC("oout of。memory");
    }
    memset((void*)paddr,0,n*PAGE_SIZE);//歸零後給出空間
    return paddr;
}

//接下來實作ch10的context_switch函式 負責移出先前的sp和移入新加入的sp
__attribute__((naked))//因為要調sp 不然可能不準
void switch_context(uint32_t* prev_sp,uint32_t* next_sp){
    __asm__ __volatile__(
        //先存prev的暫存器狀態
        "addi sp, sp,-13*4\n"
        "sw ra ,0(sp)\n"
        "sw s0, 1*4(sp)\n"
        "sw s1, 2*4(sp)\n"
        "sw s2, 3*4(sp)\n"
        "sw s3, 4*4(sp)\n"
        "sw s4, 5*4(sp)\n"
        "sw s5, 6*4(sp)\n"
        "sw s6, 7*4(sp)\n"
        "sw s7, 8*4(sp)\n"
        "sw s8, 9*4(sp)\n"
        "sw s9, 10*4(sp)\n"
        "sw s10, 11*4(sp)\n"
        "sw s11, 12*4(sp)\n"
        //接著因為更新sp a的sp要存新的 存回arg1的位置
        "sw sp, (a0)\n"
        "lw sp, (a1)\n"//這樣sp就調到next的位置，然後載回狀態進暫存器

        "lw ra, 0*4(sp)\n"
        "lw s0, 1*4(sp)\n"
        "lw s1, 2*4(sp)\n"
        "lw s2, 3*4(sp)\n"
        "lw s3, 4*4(sp)\n"
        "lw s4, 5*4(sp)\n"
        "lw s5, 6*4(sp)\n"
        "lw s6, 7*4(sp)\n"
        "lw s7, 8*4(sp)\n"
        "lw s8, 9*4(sp)\n"
        "lw s9, 10*4(sp)\n"
        "lw s10, 11*4(sp)\n"
        "lw s11, 12*4(sp)\n"
        "addi sp , sp, 13*4\n"
        "ret\n"//跳回ra

    );
}

//ch13user_entry在這邊實作
__attribute__((naked)) void user_entry(void){//用檔sret所以naked
    __asm__ __volatile__(
        "csrw sepc, %[sepc]\n"
        "csrw sstatus,%[sstatus]\n"
        "sret\n"
        :
        :[sepc]"r"(USER_BASE), [sstatus]"r"(SSTATUS_SPIE|SSTATUS_SUM)
        //這邊設定到時候trap完成回到user_base以及回去umode後可以繼續中斷
        //ch16更新了SSTATUS_SUM讓kernel函式可以動user記憶體位址
    );
} 

//ch10然後實作crreat_process函式 讓pcb和對應函式執行
struct process procs[PROCS_MAX]; //先建一個process table
extern char __kernel_base[];
struct process* create_process(const void* image, size_t image_size){//ch13對傳入參數更改成image,image_size 也就是image在kernel.data的bin_start,bin_size
    //先來找空出來的位置
    struct process* proc = NULL;
    int i;
    for( i=0;i<PROCS_MAX;i++){
        if(procs[i].state==PROC_UNUSED){
            proc = &procs[i];
            break;
        }
    }
    if(!proc){
        PANIC("no free process slots");//沒多餘位置就報錯
    }
    //但是這個process物件裡面沒有值 到時候cs會出問題 於是我們初始化他 重點在於操作sp
    uint32_t* sp = (uint32_t *) &proc->stack[sizeof(proc->stack)];
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;//以上是放暫存器s11~s0
    //*--sp = (uint32_t)pc;//放ra的 ch10做的現在ch13要更新
    *--sp = (uint32_t)user_entry;//ch13的更新 蠻有意思 因為process是在s-mode被創造 但要sret回去umode可是這個西的process沒有回去過umode sret讀不到對的值 於是user_entry負責先填好sret要的值好做sret回去umode

    //ch11給process加上table元素 並且填入表格
    uint32_t* page_table = (uint32_t*)alloc_pages(1);
    for(paddr_t paddr =(paddr_t)__kernel_base; paddr<(paddr_t)__free_ram_end;paddr+=PAGE_SIZE ){
        map_page(page_table,paddr,paddr,PAGE_R|PAGE_W|PAGE_X);//每個process都會畫一次分頁
    }
    //ch15這裡也把vitio-blk的MMIO暫存器也放進page table裡面
    map_page(page_table, VIRTIO_BLK_PADDR, VIRTIO_BLK_PADDR,PAGE_R|PAGE_W);
    //ch13這裡要實作把user原本寄放在kernel區的raw_bin複製到user的對應虛擬位址空間
    for(uint32_t off=0;off<image_size;off+=PAGE_SIZE){
        paddr_t page =alloc_pages(1); //先開一頁記憶體 單純是要放東西讀空間 是物理位置
        //接下來要定義兩個變數 一是剩餘多少還沒複製 二是這次要複製的大小
        size_t remaining = image_size - off;
        size_t copy_size = PAGE_SIZE<=remaining? PAGE_SIZE: remaining;//如果剩下不夠一頁就不用複製那麼多

        memcpy((void*)page,image+off,copy_size); //可以開始複製
        map_page(page_table,USER_BASE+off,page,PAGE_U|PAGE_R|PAGE_W|PAGE_X);

    }

    //最後記得更新pcb狀態
    proc->pid =i+1;
    proc->sp = (uint32_t)sp;
    proc->state = PROC_RUNNABLE;
    proc->page_table=page_table;//ch11補的
    return proc;//回傳最後這個pcb的位置

}

//所以現在來測試開兩個想要用的函式去測試cs和process_create是否成功
void delay(void){//延遲用s
    for(int i=0;i<30000000;i++){
        __asm__ __volatile__(
            "nop\n"
        );
    }
}
struct process* proc_a;
struct process* proc_b;

void proc_A_entry(void){
    printf("starting process A\n");
    while(1){
        putchar('A');
        /*switch_context(&proc_a->sp,&proc_b->sp);*/
        delay();
        yield();
    }
}
void proc_B_entry(void){
    printf("starting process B\n");
    while(1){
        putchar('B');
        /*switch_context(&proc_b->sp,&proc_a->sp);*/
        delay();
        yield();
    }
}
//ch10排程器yield的實作
struct process* current_proc;
struct process* idle_proc;
void yield(void){
    struct process* next = idle_proc;//當前next先設給idle避免沒地方可跳idle
    for(int i=0;i<PROCS_MAX;i++){
        struct process* proc = &procs[(current_proc->pid+i)%PROCS_MAX];//輪詢去找可用空格也避免starvation
        if(proc->state==PROC_RUNNABLE &&proc->pid>0){
            next = proc;
            break;
        }
    }
    if(next == current_proc){
        return;//不用切換
    }
    //加入儲存next的stack頂位置到sscratch
    __asm__ __volatile__(
        "sfence.vma\n"//確保之前寫入都落地 沒有留在緩存
        "csrw satp,%[satp]\n"
        "sfence.vma\n"//確保tlb更新
        "csrw sscratch, %[sscratch]\n"
        :
        :[sscratch]"r"((uint32_t)&next->stack[sizeof(next->stack)]), [satp]"r"(SATP_SV32|(uint32_t)next->page_table>>12)//就是下一個processtable1的物理位址前20bit 符合放入satp的格式      
    );


    struct process* prev = current_proc;
    current_proc=next;
    switch_context(&prev->sp,&next->sp);
}

//ch11實作map_page負責建立表 會傳入對應的root表指標 要配對的虛擬位置 物理位置 和flags
void map_page(uint32_t* table1 ,uint32_t vaddr,paddr_t paddr,uint32_t flags ){
    //先做格式判斷是否要報錯
    if(!is_aligned(vaddr,PAGE_SIZE)){
        PANIC("unaligned vadddr %x",vaddr);
    }
    if(!is_aligned(paddr,PAGE_SIZE)){
        PANIC("unaligned padddr %x",paddr);
    }
    //幾下來是透過vaddr確認table各自要填什麼
    uint32_t vpn1 = (vaddr>>22)&0x3ff;
    if((table1[vpn1]&PAGE_V)==0){//檢查table1對應欄位是否valid 若不valid要建表
        uint32_t pt_addr = alloc_pages(1);
        table1[vpn1] = ((pt_addr>>12)<<10)|PAGE_V; //這樣表裡放的就是對的格式了
    }
    
    uint32_t vpn0 = (vaddr>>12)&0x3ff;
    uint32_t* table0 = (uint32_t*)((table1[vpn1]>>10)<<12);//這樣可以得出第二層表的表位址，下面再接著填第二層表的內容
    table0[vpn0]= ((paddr>>12)<<10)|flags|PAGE_V; //table0對應欄位放的就是實際物理位置的前面20bit

}

//ch13 用以方便操作shell.bin.o的raw binary
extern char _binary_shell_bin_start[];
extern char _binary_shell_bin_size[];//在.sh使用指令編譯成elf時設定的變數 可以直接引用

//ch14要寫handle_syscall
void handle_syscall(struct trap_frame* f){//這裡的f是對應的process kernel_stack的sp（低點）
    if(f->a3==SYS_PUTCHAR){//去看是不是putchar的syscall
        putchar(f->a0);//就是在user.c時putchar放的字
    }
    else if(f->a3==SYS_GETCHAR){
        while(1){//電腦會一直偵測在按下鍵盤前都是-1
            long ch = getchar();//調用的是kerne的getchar
            if(ch>=0){
                f->a0 = ch;//存到對應位址 不是直接給暫存器
                break;
            } 
            yield();//沒偵測到就yield出去等下次 避免busy_waiting
        }
    }
    else if(f->a3==SYS_EXIT){
        printf("process %d exited\n",current_proc->pid);//輸出相關參數
        current_proc->state = PROC_EXITED;//改狀態
        yield();
        PANIC("unreachable"); //因為yield出去且狀態終止 所以不會走到這行 但我們也沒有實作free就是了
    }
    else if(f->a3==SYS_READFILE){
        const char* filename=(const char* )f->a0;
        char* buf= (char*)f->a1;//很重要 這裡的buf是user_buf
        int len = f->a2;//以上三個讀取暫存器參數 因為ecall直接跳

        struct file* file=fs_lookup(filename);
        if(!file){
            printf("file not found: %s\n", filename);//報錯找不到
            f->a0 = -1;
            return;
        }
        if(len>(int)sizeof(file->data)){
            len=file->size;//len不能超過
        }
        memcpy(buf,file->data,len);//檔案複製給user_buf 此時在kernel動用到user記憶體空間 會有問題 後面會處理
        f->a0 =len;
        
        }
    else if(f->a3==SYS_WRITEFILE){
        const char* filename=(const char* )f->a0;
        char* buf= (char*)f->a1;
        int len = f->a2;//以上三個讀取暫存器參數 因為ecall直接跳

        struct file* file=fs_lookup(filename);
        if(!file){
            printf("file not found: %s\n", filename);//報錯找不到
            f->a0 = -1;
            return;
        }
        if(len>(int)sizeof(file->data)){
            len=file->size;//len不能超過
        }
        memcpy(file->data,buf,len);
        file->size=len;
        fs_flush();//推進磁碟
        f->a0 =len;
    }
    else {
        PANIC("unexpected syscall a3=%x\n",f->a3);//報錯錯誤的syscall種類
    }

}  
//ch14寫的getchar
long getchar(void){
    struct sbiret ret =sbi_call(0,0,0,0,0,0,0,2);//回傳值就是主控台的輸入
    return ret.error;//回傳值設定上會被放在這邊
}

//ch15以下是讀取MMIO暫存器的函式
uint32_t virtio_reg_read32(unsigned offset){
    return *((volatile uint32_t*)(VIRTIO_BLK_PADDR +offset));
}
uint64_t virtio_reg_read64(unsigned offset){
    return *((volatile uint32_t*)(VIRTIO_BLK_PADDR +offset));
}
void virtio_reg_write32(unsigned offset,uint32_t value){
    *((volatile uint32_t* )(VIRTIO_BLK_PADDR+offset))=value;
}
void virtio_reg_fetch_and_or32(unsigned offset,uint32_t value){//握手時用的
    virtio_reg_write32(offset,virtio_reg_read32(offset)|value);//讀出來做or再寫回去
}
//以下是ch15的初始化
struct virtio_virtq *blk_request_vq;//建立一個最大的virtq物件的指標
struct virtio_blk_req *blk_req;//給一個訂單物件的指標
paddr_t blk_req_paddr;//訂單物件的物理位址
uint64_t blk_capacity;//整個磁碟的容量bytes

void virtio_blk_init(void) {//初始化block的函式
    if (virtio_reg_read32(VIRTIO_REG_MAGIC) != 0x74726976)
        PANIC("virtio: invalid magic value");//檢查是不是virtio裝置
    if (virtio_reg_read32(VIRTIO_REG_VERSION) != 1)
        PANIC("virtio: invalid version");//確認版本
    if (virtio_reg_read32(VIRTIO_REG_DEVICE_ID) != VIRTIO_DEVICE_BLK)
        PANIC("virtio: invalid device id");//確認裝置id是2

    //重設裝置狀態值為0
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, 0);
    //用旗標加上ACK狀態位元表示已經發現裝置
    virtio_reg_fetch_and_or32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_ACK);
    //用旗標加上DRIVER狀態位元表示知道如何使用
    virtio_reg_fetch_and_or32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER);
    // 設定頁面大小，一樣使用4KB頁面
    virtio_reg_write32(VIRTIO_REG_PAGE_SIZE, PAGE_SIZE);
    //給一個初始化好的virtq物件
    blk_request_vq = virtq_init(0);
    //設定 DRIVER_OK 狀態位元表示可以使用
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER_OK);
    //得到裝置容量
    blk_capacity = virtio_reg_read64(VIRTIO_REG_DEVICE_CONFIG + 0) * SECTOR_SIZE;
    printf("virtio-blk: capacity is %d bytes\n", (int)blk_capacity);
    // 開一個訂單的位址 大小依據訂單物理長度
    blk_req_paddr = alloc_pages(align_up(sizeof(*blk_req), PAGE_SIZE) / PAGE_SIZE);
    blk_req = (struct virtio_blk_req *) blk_req_paddr;
}
//ch15virtq的初始化函式
struct virtio_virtq* virtq_init(unsigned index) {//回傳一個virtq物件指標
    paddr_t virtq_paddr = alloc_pages(align_up(sizeof(struct virtio_virtq), PAGE_SIZE) / PAGE_SIZE);//開頁面存vitq物件的內容
    struct virtio_virtq* vq = (struct virtio_virtq*) virtq_paddr;//把要回傳的物件指標和物件位址掛上
    vq->queue_index = index;//第幾個virtq 這邊是第0個
    vq->used_index = (volatile uint16_t* ) &vq->used.index;//device已經處理幾個了 透過memset預設是0
    virtio_reg_write32(VIRTIO_REG_QUEUE_SEL, index);//目前看第幾個virtq物件
    virtio_reg_write32(VIRTIO_REG_QUEUE_NUM, VIRTQ_ENTRY_NUM);//desc陣列給幾個 這邊是16格
    virtio_reg_write32(VIRTIO_REG_QUEUE_PFN, virtq_paddr / PAGE_SIZE);//給vitq物件的頁面編號
    return vq;//回傳物件指標
}
//ch15這邊是要進入把請求加入virtqueue的階段
void virtqu_kick(struct virtio_virtq* vq,int desc_index){//通知device新chain加入的函式
    vq->avail.ring[vq->avail.index%VIRTQ_ENTRY_NUM]=desc_index;//把頭索引用取餘的方式放進availring
    vq->avail.index++;//rin的數量+1
    __sync_synchronize();//先把目前更動都寫進記憶體
    virtio_reg_write32(VIRTIO_REG_QUEUE_NOTIFY,vq->queue_index);//把該條virtq的給device通知寫上
    vq->last_used_index++;//device預期處理的數量++
}
bool virtq_is_busy(struct virtio_virtq* vq){
    return vq->last_used_index!= *(vq->used_index); //如果遲遲數量不匹配 代表還沒做 所以回傳0 做好了回傳1
}
//ch15接下來是對磁碟做主要操作的函式
void read_write_disk(void* buf, unsigned sector,int is_write){//傳入參數有req的緩衝區空間 第幾個sector 是讀還是寫
    //先做溢位判斷
    if(sector>=blk_capacity/SECTOR_SIZE){
        printf("virtio: tried to read/write section : %d doesn't exist, capacity is %d",sector,blk_capacity/SECTOR_SIZE);
        return;
    }

    //接著操作buf內容
    blk_req->sector=sector;//第幾號扇區
    blk_req->type = is_write? VIRTIO_BLK_T_OUT:VIRTIO_BLK_T_IN;//請求是輸入還是輸出
    if(is_write){
        memcpy(blk_req->data,buf,SECTOR_SIZE);//如果是寫就buf複製到data
    }

    //接下來填vq的該chain的三個descriptor
    struct virtio_virtq* vq = blk_request_vq;
    vq->descs[0].addr=blk_req_paddr; //先放訂單的第一欄
    vq->descs[0].len=sizeof(uint32_t)*2 + sizeof(uint64_t);//type4byte reserved4byte sector8byte
    vq->descs[0].flags=VIRTQ_DESC_F_NEXT;//不是最後
    vq->descs[0].next=1;//下一個是1

    vq->descs[1].addr= blk_req_paddr+offsetof(struct virtio_blk_req,data);//因為已經開分頁了 這邊要用offset傳物理位址
    vq->descs[1].len=SECTOR_SIZE;
    vq->descs[1].flags=VIRTQ_DESC_F_NEXT|(is_write?0:VIRTQ_DESC_F_WRITE);//紀錄有下一位以及是否device能寫
    vq->descs[1].next=2;
    
    vq->descs[2].addr=blk_req_paddr+offsetof(struct virtio_blk_req,status);//struct 結構內（不特指）的物理位址差異
    vq->descs[2].len=sizeof(uint8_t);
    vq->descs[2].flags = VIRTQ_DESC_F_WRITE;//裝置一定要寫status也就是成功執行與否
    
    //填好了就通知device
    virtqu_kick(vq,0);//哪條virtq以及頭索引
    while(virtq_is_busy(vq)); //等到device處理完

    if(blk_req->status!=0){//失敗了
        printf("vitio: warn: faileed to rea or write sector %d, and status= %d",sector,blk_req->status);
        //做報錯
        return;
    }
    //此時device已經寫好了，可以輪到讀了
    if(!is_write){
        memcpy(buf,blk_req->data,SECTOR_SIZE);//從data載回來buf
    }
}

//ch16先開檔案櫃和緩衝區disk
struct file files[FILES_MAX];
uint8_t disk[DISK_MAX_SIZE];
//ch16實作八進位數轉整數，ustar填入都是8進位
int oct2int(char* oct, int len) {//填入陣列與長度
    int dec =0;
    for (int i = 0; i < len; i++) {
        if (oct[i] < '0' || oct[i] > '7')//遇到‘\0’就跳出
            break;
        dec = dec *8 + (oct[i] -'0');//進位加上個位數
    }
    return dec;
}
//ch16再接著實作從磁碟到disk再到填寫好file元素內容的函式
void fs_init(void) {
    for (unsigned sector = 0; sector < sizeof(disk) / SECTOR_SIZE; sector++)
        read_write_disk(&disk[sector * SECTOR_SIZE], sector, false);//先以sector為單位複製好磁碟內容到disk

    unsigned off = 0;//一次處理一個檔案 off是每個檔案間的隔斷
    for (int i = 0; i < FILES_MAX; i++) {
        struct tar_header* header = (struct tar_header *) &disk[off];//取一個header物件
        if (header->name[0] == '\0'){
            break;//沒有檔案就挑過
        }
        if (strcmp(header->magic, "ustar") != 0){//確認不是tar格式要報錯
            PANIC("invalid tar header: magic=\"%s\"", header->magic);// \"是打出單個引號的意思
        }
        int filesz = oct2int(header->size, sizeof(header->size));//讀檔案長度
        struct file* file = &files[i];//開一個檔案物件指標
        file->in_use = true;//這個file格子有人了
        strcpy(file->name, header->name);//寫file元素
        memcpy(file->data, header->data, filesz);//寫file元素
        file->size = filesz;//寫file元素
        printf("file: %s, size=%d\n", file->name, file->size);

        off += align_up(sizeof(struct tar_header) + filesz, SECTOR_SIZE);//最後跳到下一個檔案起始點
    }
}
//接下來實作檔案寫回磁碟的函式：讀file存header
void fs_flush(void) {
    memset(disk, 0, sizeof(disk));//先清空disk緩衝空間
    unsigned off = 0;
    for (int file_i = 0; file_i < FILES_MAX; file_i++) {//逐檔案寫入
        struct file *file = &files[file_i];//建立指向檔案的指標
        if (!file->in_use){
            continue;//不存在就跳過
        }
            
        struct tar_header* header = (struct tar_header*) &disk[off];//header起始位址
        memset(header, 0, sizeof(*header));//disk區段先歸零
        strcpy(header->name, file->name);//填入檔案名稱
        strcpy(header->mode, "000644");//填入權限
        strcpy(header->magic, "ustar");//權限：rw-r--r--
        strcpy(header->version, "00");//填入版本
        header->type = '0';//填入檔案種類，一般檔案

        int filesz = file->size;
        for (int i = sizeof(header->size); i > 0; i--) {
            header->size[i-1] = (filesz % 8) +'0';//把檔案大小從整數轉入8進位陣列
            filesz /= 8;
        }

        int checksum = ' ' * sizeof(header->checksum);//接著計算checksum checksum是把header每一個byte相加 原本checksum位址補上8個空格
        for (unsigned i = 0; i < sizeof(struct tar_header); i++){
            checksum += (unsigned char) disk[off + i];
        }
        //接著存checksum 也是8進位陣列
        for (int i=5; i >= 0; i--) {
            header->checksum[i] = (checksum % 8) +'0';
            checksum /= 8;
        }
        //接者把filedata複製到header裡
        memcpy(header->data, file->data, file->size);
        off += align_up(sizeof(struct tar_header) + file->size, SECTOR_SIZE);//最後跳到下一個檔案在disk的開始位址
    }

    
    for (unsigned sector = 0; sector < sizeof(disk) / SECTOR_SIZE; sector++){
        read_write_disk(&disk[sector * SECTOR_SIZE], sector, true);//最後把disk緩衝區寫盡磁碟
    }
        

    printf("wrote %d bytes to disk\n", sizeof(disk));
}
//這邊是實作查詢對應檔案名並且回傳對應檔案的位址 ch16
struct file* fs_lookup(const char* filename) {
    for (int i = 0; i < FILES_MAX; i++) {
        struct file* file = &files[i];
        if (!strcmp(file->name, filename)){
            return file;//相等就回傳
        }
    }
    return NULL;
}


void kernel_main(void){
    memset(__bss,0,(size_t)__bss_end-(size_t)__bss);//清理bss空間

    WRITE_CSR(stvec,(uint32_t)kernel_entry);//把trap入口位置寫給stvec
    virtio_blk_init();//對drive初始化
    fs_init();//從磁碟讀取到disk並填寫file表

    /*char buf[SECTOR_SIZE];//建立buf
    read_write_disk(buf,0,false);//讀第一塊扇區
    printf("first sector: %s\n",buf);

    strcpy(buf,"hello Professors from CSIE");
    read_write_disk(buf,0,true);//寫扇區（這裡是覆寫）

    read_write_disk(buf,0,false);//讀改過的扇區
    printf("write success: %s\n",buf);
    //ch12確認shell成功加入
   
    printf("shell_bin_size = %d\n",(int)_binary_shell_bin_size);
    printf("shell_bin[0] = %x\n",_binary_shell_bin_start[0]);*/

    //接下來是ch9的記憶體分配測試
    /*paddr_t paddr0 = alloc_pages(2);
    paddr_t paddr1 = alloc_pages(1);
    printf("alloc_pages test: paddr0 = %x\n",paddr0);
    printf("alloc_pages test: paddr1 = %x\n",paddr1);
    PANIC("end");*/
    //__asm__ __volatile__("unimp");//這裡我們用unimp保證執行一個例外（unimplemented）
    //PANIC("PANIC test");//ch07實作PANIC跑看看

    //接下來是ch10的測試+yield，先寫idle
    idle_proc=create_process(NULL,0);//不用複製image
    idle_proc->pid=0;//pid前面預設是至少是1 0是非正規
    current_proc = idle_proc;//這邊是初始化

    /*遮邊是ch10的簡單實作ch13已改掉 僅留存proc_a = create_process((uint32_t)proc_A_entry);
    proc_b = create_process((uint32_t)proc_B_entry);
    yield();//開始偵測並發布任務
    PANIC("context switch test for idle version");*/
    
    //ch13測試
    create_process(_binary_shell_bin_start,(size_t)_binary_shell_bin_size);
    yield();
    PANIC("test for ch13");

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