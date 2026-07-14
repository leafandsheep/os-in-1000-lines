#pragma once
#include "common.h"

__attribute__((noreturn)) void exit(void);
void putchar(char ch);

int getchar(void);//ch14
int syscall(int sysno,int arg0,int arg1,int arg2);//ch14

//ch16的函式宣告
int readfile(const char *filename, char *buf, int len);
int writefile(const char *filename, const char *buf, int len);