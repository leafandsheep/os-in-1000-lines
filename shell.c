//於是我們可以開始來寫使用者的main函式了
#include"user.h"

void main(void){
    //ch13現在讓user碰kernel程式然後來看會怎麼報錯
    //*((volatile int*)0x80200000)=0x1234;//volatile是說就算後面也沒用不要直接優化刪除，還是要確實執行
    //printf("HELLo World from shell\n");ch14測試用
    //ch14實作互動式輸入輸出
    while(1){
        restart://爆數量就重來
            printf("> ");
            char cmdline[128];
            for(int i=0;;i++){
                
                char ch = getchar();
                putchar(ch);//實時印出
                //接著做判斷
                if(i == sizeof(cmdline)-1){//陣列末位要從頭
                    printf("command line too long\n");
                    goto restart;
                }
                else if(ch == '\r'){//表示結束輸入
                    printf("\n");
                    cmdline[i]='\0';
                    break;//結束本次輸入進入輸出環節
                }
                else{
                    cmdline[i]=ch;
                }
            }
            if(strcmp(cmdline,"hello")==0){//測試指令輸入是否相符
                printf("Hello world from shell\n");
            }
            else if(strcmp(cmdline,"exit")==0){
                printf("exit\n");
                exit();//終止執行
            }
            else if (strcmp(cmdline,"readfile")==0){
                char user_buf[128];//user的buf開在這裡
                int len = readfile("hello.txt",user_buf,sizeof(user_buf));
                user_buf[len] = '\0';//補結尾標記
                printf("%s\n",user_buf);
            }
            else if (strcmp(cmdline,"writefile")==0){
                writefile("hello.txt","HELLO TO PROFESSORS!",20);
            }    
            else{
                printf("unknown command:%s\n",cmdline);
            }
    }
    for(;;);//一開始簡單功能
}