#pragma once //避免重複引用

//ch6我們定義基本型別和便利的巨集
typedef int bool;//定義bool的型別
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned int size_t;
typedef uint32_t paddr_t;//物理位置型別
typedef uint32_t vaddr_t;// 虛擬位置型別

#define true 1
#define false 0
#define NULL ((void*)0)
#define align_up(value,align) __builtin_align_up(value, align)
#define is_aligned(value,align) __builtin_is_aligned(value,align)
#define offsetof(type,member) __builtin_offsetof(type,member)
#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg//這四者是編譯器內建的

//接下來是我們要實作的四個記憶體操作函式
void *memset(void* buf , char c, size_t n);//前面bss已經實作過
void *memcpy(void* dst,const void* src, size_t n);
char* strcpy(char* dst, const char* src);
int strcmp(const char* s1, const char* s2);

//宣告printf
void printf(const char* c,...);

//ch9需要定義PAGE_ZIZE
#define PAGE_SIZE 4096

