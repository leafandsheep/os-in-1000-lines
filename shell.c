//於是我們可以開始來寫使用者的main函式了
#include"user.h"

void main(void){
    //ch13現在讓user碰kernel程式然後來看會怎麼報錯
    *((volatile int*)0x80200000)=0x1234;//volatile是說就算後面也沒用不要直接優化刪除，還是要確實執行
    for(;;);//一開始簡單功能
}