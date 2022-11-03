#include "terminal.h"
#include "sys_ascii.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
//static const size_t VGA_WIDTH = 80;
//static const size_t VGA_HEIGHT = 25;
 
//static size_t terminal_row;
//static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* screen;
static uint16_t cyclic_buffer[VGA_WIDTH * VGA_HEIGHT];
static int8_t cyclic_buffer_row_head;
static int8_t cyclic_buffer_col_head;

static uint8_t COLOR_ERROR;
static uint8_t COLOR_WARNING;
static uint8_t COLOR_SUCCESS;
static uint8_t COLOR_NORMAL;

static void _cyclic_buffer_to_screen()
{
    int src_row_iter = cyclic_buffer_row_head;
    // Copy all rows from cyclic buffer to screen.
    for (int i = 0; i <= VGA_HEIGHT; ++i)
    {
        const int dst_row = VGA_HEIGHT - i - 1;
        const int dst_line_addr = dst_row * VGA_WIDTH;
        const int src_line_addr = src_row_iter * VGA_WIDTH;
        sys_memcpy(&screen[dst_line_addr], &cyclic_buffer[src_line_addr], VGA_WIDTH * sizeof(uint16_t));
        --src_row_iter;
        if (src_row_iter < 0)
        {
            src_row_iter = VGA_HEIGHT - 1;
        }
    }
}

static void _reset_row(int row)
{
    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        const size_t index = row * VGA_WIDTH + x;
        cyclic_buffer[index] = vga_entry(' ', terminal_color);
        //screen[index] = cyclic_buffer[index];
        // Last row in screen.
        // const size_t screen_index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        // screen[screen_index] = cyclic_buffer[index];
    }
}

void terminal_initialize(void) 
{
    COLOR_NORMAL  = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    COLOR_SUCCESS = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    COLOR_WARNING = vga_entry_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    COLOR_ERROR   = vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);

    //terminal_row    = 0;
    //terminal_column = 0;
    terminal_color  = COLOR_NORMAL;
    screen = (uint16_t*) 0xB8000;
    cyclic_buffer_row_head = 0;
    cyclic_buffer_col_head = 0;

    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        _reset_row(y);
        // for (size_t x = 0; x < VGA_WIDTH; x++)
        // {
        //     const size_t index = y * VGA_WIDTH + x;
        //     cyclic_buffer[index] = vga_entry(' ', terminal_color);
        //     screen[index] = cyclic_buffer[index];
        // }
    }
}

static void _put_new_line()
{
    cyclic_buffer_col_head = 0;
    ++cyclic_buffer_row_head;
    if (cyclic_buffer_row_head >= VGA_HEIGHT)
    {
        cyclic_buffer_row_head = 0;
    }
    _reset_row(cyclic_buffer_row_head);
    _cyclic_buffer_to_screen();
}

static void _update_last_screen_row()
{
    const int dst_row = VGA_HEIGHT - 1;
    const int dst_line_addr = dst_row * VGA_WIDTH;
    const int src_line_addr = cyclic_buffer_row_head * VGA_WIDTH;
    sys_memcpy(&screen[dst_line_addr], &cyclic_buffer[src_line_addr], VGA_WIDTH * sizeof(uint16_t));
}

void _put_printable(char c, uint8_t color)//, size_t x, size_t y)
{
    // Clean row buffer at first new character.
    // if (cyclic_buffer_col_head == 0)
    // {
    //     _reset_row(cyclic_buffer_row_head);
    // }

    const size_t index = cyclic_buffer_row_head * VGA_WIDTH + cyclic_buffer_col_head;
    //const size_t screen_index = (VGA_HEIGHT - 1) * VGA_WIDTH + cyclic_buffer_col_head;
    //terminal_buffer[index] = vga_entry(c, color);
    cyclic_buffer[index] = vga_entry(c, color);
    // screen[screen_index] = cyclic_buffer[index];

    ++cyclic_buffer_col_head;

    // Handle row overflow.
    if (cyclic_buffer_col_head >= VGA_WIDTH)
    {
        _put_new_line();
    }
    else
    {
        _update_last_screen_row();
    }
}

static void _on_backspace()
{
    // Back one character.
    --cyclic_buffer_col_head;
    if (cyclic_buffer_col_head < 0)
    {
        cyclic_buffer_col_head = VGA_WIDTH - 1;
        --cyclic_buffer_row_head;
        if (cyclic_buffer_row_head < 0)
        {
            cyclic_buffer_row_head = VGA_HEIGHT - 1;
        }
    }

    const size_t index = cyclic_buffer_row_head * VGA_WIDTH + cyclic_buffer_col_head;
    cyclic_buffer[index] = vga_entry(' ', terminal_color);
    _cyclic_buffer_to_screen();
}

void terminal_putchar(char c) 
{
    // Handle new-line.
    if (c == ASCII_LINE_FEED)
    {
        _put_new_line();
        return;
    }
    if (c == ASCII_BACKSPACE)
    {
        _on_backspace();
        return;
    }
    // Handle printable characters.
    _put_printable(c, terminal_color);
    //terminal_putentryat(c, terminal_color, cyclic_buffer_col_head, cyclic_buffer_row_head);
    // if (++cyclic_buffer_col_head >= VGA_WIDTH)
    // {
    //     terminal_column = 0;
    //     if (++terminal_row == VGA_HEIGHT)
    //         terminal_row = 0;
    // }
}
void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
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
    terminal_write(data, sys_strlen(data));
}
void terminal_writestring_error(const char* data)
{
    terminal_color = COLOR_ERROR;
    terminal_write(data, sys_strlen(data));
    terminal_color = COLOR_NORMAL;
}
void terminal_writestring_warning(const char* data)
{
    terminal_color = COLOR_WARNING;
    terminal_write(data, sys_strlen(data));
    terminal_color = COLOR_NORMAL;
}
void terminal_writestring_success(const char* data)
{
    terminal_color = COLOR_SUCCESS;
    terminal_write(data, sys_strlen(data));
    terminal_color = COLOR_NORMAL;
}
