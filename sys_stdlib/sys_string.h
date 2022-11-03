#ifndef SYS_STRING_H
#define SYS_STRING_H

#include "sys_stdint.h"

void*  sys_memset(void* dst, int value, size_t num);
void*  sys_memcpy(void* dst, const void* src, size_t num);
int    sys_memcmp(const void* ptr1, const void* ptr2, size_t num);

size_t sys_strlen(const char* str);
char*  sys_strcpy(char* dst, const char* src);

#endif // SYS_STRING_H
