#ifndef SYS_STDIO_H
#define SYS_STDIO_H

#include "sys_stdarg.h"

#define MAKE_FUNC_NAME(_name) \
    sys_ ## _name

int MAKE_FUNC_NAME(vsprintf)(char* output, const char* format, va_list arg);
int MAKE_FUNC_NAME(sprintf)(char* str, const char* format, ...);
int MAKE_FUNC_NAME(printf)(const char *format, ...);
//int MAKE_FUNC_NAME(puts)(const char *str);

#endif // SYS_STDIO_H
