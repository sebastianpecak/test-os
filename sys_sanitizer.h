#ifndef SYS_SANITIZER_H
#define SYS_SANITIZER_H

#include "sys_stddef.h"

// #define SYS_FLAG_ENABLE_SANITIZER

//#ifdef SYS_FLAG_ENABLE_SANITIZER

#define SYS_ASSERT(_expr) \
    do { \
        if (!(_expr)) { terminal_writestring("ERROR: "); terminal_writestring(#_expr); terminal_putchar('\n' ); } \
    } while (0)

#define SYS_ASSERT_NOT_NULL(_expr) \
    SYS_ASSERT((_expr) != NULL)

#define SYS_GLUE(_a, _b) \
    (_a ## _b)

#define SYS_COMPILE_TIME_ASSERT(_expr, _msg) \
    typedef char SYS_GLUE(compile_time_assert_, _msg) [(_expr) ? (+1) : (-1)]

// #else

// #define SYS_ASSERT_NOT_NULL(_exp, _return) \
//     NOP

// #endif // SYS_FLAG_ENABLE_SANITIZER

#endif // SYS_SANITIZER_H
