#ifndef SYS_TXT_TERMINAL_H
#define SYS_TXT_TERMINAL_H

#include "sys_multiboot.h"
#include "sys_string.h"
#include "sys_stddef.h"

#pragma pack(1)
struct EgaTxtTerminalEntry_s
{
    union
    {
        uint16_t word;
        struct
        {
            uint8_t character;
            union
            {
                uint8_t attribute;
                // Without underline function.
                struct
                {
                    uint8_t fg    : 4;
                    uint8_t bg    : 3;
                    uint8_t blink : 1;
                };
                // With underline.
                struct
                {
                    uint8_t ul_bit   : 1;
                    uint8_t ul_fg    : 3;
                    uint8_t ul_bg    : 3;
                    uint8_t ul_blink : 1;
                };
            };
        };
    };
};
#pragma pack()

// struct EgaTxtTerminal_s
// {
// };
struct TxtTerminal_s
{
    //uint16_t* phy_addr;
    struct EgaTxtTerminalEntry_s* buffer;
    uint32_t width;
    uint32_t height;
    uint8_t bytes_per_char;
};

// int sys_txt_terminal_init(struct TxtTerminal_s* term, const struct MultibootBootInfo_s* boot_info)
// {
//     if ((boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_FRAMEBUFFER) == 0)
//     {
//         return -1;
//     }

//     sys_memset(term, 0, sizeof(struct TxtTerminal_s));
//     term->buffer = TYPE_CAST(struct EgaTxtTerminalEntry_s*, boot_info->framebuffer.addr);
//     term->width = boot_info->framebuffer.width;
//     term->height = boot_info->framebuffer.height;
//     term->bytes_per_char = boot_info->framebuffer.bpp;

//     const size_t term_size_bytes = term->width * term->height * term->bytes_per_char;
//     sys_memset(term->buffer, 0, term_size_bytes);

//     return 0;
// }

// struct EgaTxtTerminalEntry_s* sys_txt_terminal_get_entry(struct TxtTerminal_s* term, int x, int y)
// {
//     // SANITIZER NEEDED.

//     const size_t line_addr = (y * term->width) + x;
//     return &term->buffer[line_addr];
// }

// int sys_txt_terminal_write(struct TxtTerminal_s* term, int x, int y, uint8_t data, uint8_t color)
// {
//     // SANITIZER NEEDED.

//     const size_t line_addr = (y * term->width) + x;
//     term->phy_addr[line_addr] = byte;

//     return 0;
// }

// int sys_txt_terminal_read(const struct TxtTerminal_s* term, int x, int y)
// {
//     // SANITIZER NEEDED.
//     const size_t line_addr = (y * term->width) + x;
//     return term->phy_addr[line_addr];
// }

#endif // SYS_TXT_TERMINAL_H
