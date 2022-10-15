#ifndef SYS_TERMINAL_H
#define SYS_TERMINAL_H

#include "sys_stdint.h"
#include "sys_string.h"

// extern size_t terminal_row;
// extern size_t terminal_column;
// extern uint8_t terminal_color;
// extern uint16_t* terminal_buffer;

enum vga_color
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_initialize(void);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
uint16_t vga_entry(unsigned char uc, uint8_t color);
void terminal_setcolor(uint8_t color);
void terminal_writestring(const char* data);
void terminal_writestring_error(const char* data);
void terminal_writestring_warning(const char* data);
void terminal_writestring_success(const char* data);

#endif // SYS_TERMINAL_H
