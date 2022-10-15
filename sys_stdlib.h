#ifndef SYS_STDLIB_H
#define SYS_STDLIB_H

#include "sys_stdint.h"

enum Bool_e
{
    BOOL_FALSE = 0,
    BOOL_TRUE
};
typedef enum Bool_e Bool_t;

enum NumberBase_e
{
    BASE_NONE    = 0,
    BASE_BINARY  = 2,
    BASE_OCTAL   = 8,
    BASE_DECIMAL = 10,
    BASE_HEX     = 16
};

int uint32_to_string(uint32_t number, uint8_t base, char* output, size_t output_size);

#endif // SYS_STDLIB_H
