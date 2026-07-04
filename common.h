#pragma once //避免重複引用

#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg//這四者是編譯器內建的

//宣告printf
void printf(const char* c,...);