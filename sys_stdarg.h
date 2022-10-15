#ifndef SYS_STDARG_H
#define SYS_STDARG_H

#include "sys_stdint.h"
#include "sys_stddef.h"

typedef uint8_t * va_list;

#define va_start(_va_list, _last_explicit_arg) \
    (_va_list) = ((va_list)&(_last_explicit_arg) + sizeof(_last_explicit_arg))

#define va_arg(_va_list, _type) \
    *(_type*)_va_list; \
    _va_list += sizeof(_type)

#define va_copy(_va_list_dst, _va_list_src) \
    (_va_list_dst) = (_va_list_src)

#define va_end(_va_list) \
    _va_list = NULL

#endif // SYS_STDARG_H
