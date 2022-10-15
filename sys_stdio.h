#ifndef SYS_STDIO_H
#define SYS_STDIO_H

// #include "sys_stdint.h"

// struct FILE_s
// {
// };
// typedef struct FILE_s FILE;

//int sys_fprintf(FILE *stream, const char *format, ...);
int sys_printf(const char *format, ...);//SYS_VARG_HEAD());

#endif // SYS_STDIO_H
