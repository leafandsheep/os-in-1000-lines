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
        :[sepc]"r"(USER_BASE), [sstatus]"r"(SSTATUS_SPIE)//這邊設定到時候trap完成回到user_base以及回去umode後可以繼續中斷
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
    else {
        PANIC("unexpected syscall a3=%x\n",f->a3);//報錯錯誤的syscall種類
    }

}  
//ch14寫的getchar
long getchar(void){
    struct sbiret ret =sbi_call(0,0,0,0,0,0,0,2);//回傳值就是主控台的輸入
    return ret.error;//回傳值設定上會被放在這邊
}




void kernel_main(void){
    memset(__bss,0,(size_t)__bss_end-(size_t)__bss);//清理bss空間

    WRITE_CSR(stvec,(uint32_t)kernel_entry);//把trap入口位置寫給stvec
    
    //ch12確認shell成功加入
   
    printf("shell_bin_size = %d\n",(int)_binary_shell_bin_size);
    printf("shell_bin[0] = %x\n",_binary_shell_bin_start[0]);

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