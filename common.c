#include"common.h"

void putchar(char c);

void printf(const char* fmt,...){//fmt是format string 另外這裡只有實作三種佔位符%d %x %s
    va_list vargs;//給一個參數上的游標物件
    va_start(vargs,fmt);//從fmt後開始為自由參數，要讀

    for(const char* s=fmt;*s!='\0';s++){
        if(*s=='%'&&*(s+1)!='\0'){
            s++;
            switch(*s){
                case '%': {
                    putchar('%');
                    break;
                }
                case 'd': {
                    int n=va_arg(vargs,int);//讀下一個參數 並當作指定的型別讀取 以下只是實作如何用字元方式印出整數
                    if(n<0){
                        putchar('-');
                        n*=-1;
                    }
                    int count=0;
                    int i=n;
                    int devisor=1;
                    if(n==0){
                        putchar('0');
                        break;
                    }
                    while(i){
                        count++;
                        devisor*=10;
                        i/=10;
                    }
                    while(count){
                        putchar('0'+n/(devisor/10));
                        
                        devisor/=10;
                        n=n%devisor;
                        count--;
                    }
                    break;  
                }
                case 'x':{
                    
                    int n=va_arg(vargs,int);//讀下一個參數 並當作指定的型別讀取 以下只是實作如何用字元方式印出整數
                    if(n<0){
                        putchar('-');
                        n*=-1;
                    }
                    int count=0;
                    int i=n;
                    int devisor=1;
                    if(n==0){
                        putchar('0');
                        break;
                    }
                    while(i){
                        count++;
                        devisor*=16;
                        i/=16;
                    }
                    while(count){
                        putchar("0123456789abcdef"[n/(devisor/16)]);
                        
                        devisor/=16;
                        n=n%devisor;
                        count--;
                    }
                    break;  
                }
                
                case 's':{
                    const char* c=va_arg(vargs,char*);
                    while(*c!='\0'){
                        putchar(*c);
                        c++;
                    }
                    break;
                };
            }
        }
        else putchar(*s);
    }
    va_end(vargs);
}

//ch6要實作的函式
void *memset(void *buf, char c, size_t n){//函數型別是萬用指標
    uint8_t *p = (uint8_t *) buf;
    while(n--){
        *p = c;
        p++;
    }
    return buf;
}

void *memcpy(void* dst, const void* src, size_t n ){
    uint8_t* p = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    while(n--){
        *p=*s;
        p++;
        s++;
    }
    return dst;
}

char* strcpy(char* dst ,const char* src ){
    char* p = dst;
    const char* s =src;
    while(*s !='\0'){
        *p = *s;
        p++;
        s++;
    }
    *p='\0';
    return dst;
}
int strcmp(const char* p1,const char* p2){
    while(*p1!='\0'&&*p2!='\0'){
        if(*p1!=*p2){
            break;
        }
        p1++;
        p2++;
    }
    return *p1-*p2;
}