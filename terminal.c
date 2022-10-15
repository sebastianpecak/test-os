#include "terminal.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

static uint8_t COLOR_ERROR;
static uint8_t COLOR_WARNING;
static uint8_t COLOR_SUCCESS;
static uint8_t COLOR_NORMAL;

void terminal_initialize(void) 
{
    COLOR_NORMAL  = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    COLOR_SUCCESS = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    COLOR_WARNING = vga_entry_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    COLOR_ERROR   = vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);


    terminal_row    = 0;
    terminal_column = 0;
    terminal_color  = COLOR_NORMAL;
    terminal_buffer = (uint16_t*) 0xB8000;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}
void terminal_putchar(char c) 
{
    if (c == '\n')
    {
        terminal_column = 0;
        ++terminal_row;
        return;
    }
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}
void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}
uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}
void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}
void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}
void terminal_writestring_error(const char* data)
{
    terminal_color = COLOR_ERROR;
    terminal_write(data, strlen(data));
    terminal_color = COLOR_NORMAL;
}
void terminal_writestring_warning(const char* data)
{
    terminal_color = COLOR_WARNING;
    terminal_write(data, strlen(data));
    terminal_color = COLOR_NORMAL;
}
void terminal_writestring_success(const char* data)
{
    terminal_color = COLOR_SUCCESS;
    terminal_write(data, strlen(data));
    terminal_color = COLOR_NORMAL;
}
