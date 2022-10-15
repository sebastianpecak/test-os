#ifndef SYS_STRING_H
#define SYS_STRING_H

#include "sys_stdint.h"

size_t strlen(const char* str);
char* strcpy(char* dst, const char* src);
void* sys_memset(void* dst, int value, size_t num);

#endif // SYS_STRING_H
