#ifndef SYS_GDT_H
#define SYS_GDT_H

#include "sys_stdint.h"

#define SYS_GDT_MAX_SIZE 8192
#define SYS_GDT_DEFAULT_CODE_SELECTOR 0x10 // have no idea what is this index. I supposed index in GDT, but with value 0x2 or 0x1 it crashes.
#define SYS_GDT_DEFAULT_DATA_SELECTOR 0x10
#define SYS_GDT_MAX_LIMIT_VALUE 0xFFFFFUL

#pragma pack(1)
struct GDTEntry_s
{
    uint16_t limit_1;
    uint16_t base_1;
    uint8_t  base_2;
    union
    {
        uint8_t access;
        struct
        {
            uint8_t  access_A   : 1;
            uint8_t  access_RW  : 1;
            uint8_t  access_DC  : 1;
            uint8_t  access_E   : 1;
            uint8_t  access_S   : 1;
            uint8_t  access_DPL : 2;
            uint8_t  access_P   : 1;
        };
    };
    uint8_t  limit_2 : 4;
    uint8_t  flag_reserved : 1;
    uint8_t  flag_L        : 1;
    uint8_t  flag_DB       : 1;
    uint8_t  flag_G        : 1;
    uint8_t  base_3;
};
#pragma pack()

#pragma pack(1)
struct GDTR_s
{
    uint16_t size;
    struct GDTEntry_s* gdt;
};
#pragma pack()

void sys_gdt_init();
void sys_gdt_install();

#endif // SYS_GDT_H
