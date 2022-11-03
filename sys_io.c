#include "sys_io.h"

#define ASM asm volatile

void sys_io_write_byte(uint16_t port, uint8_t value)
{
    ASM("outb %b0, %w1" :: "a"(value), "Nd"(port));
}

void sys_io_write_word(uint16_t port, uint16_t value)
{
    ASM("outw %w0, %w1" :: "a"(value), "Nd"(port));
}

void sys_io_write_dword(uint16_t port, uint32_t value)
{
    ASM("outl %0, %w1" :: "a"(value), "Nd"(port));
}

uint8_t sys_io_read_byte(uint16_t port)
{
    uint8_t buffer;
    ASM("inb %w1, %b0": "=a"(buffer) : "Nd"(port));
	return buffer;
}

uint16_t sys_io_read_word(uint16_t port)
{
    uint16_t buffer;
    ASM("inw %w1, %w0": "=a"(buffer) : "Nd"(port));
	return buffer;
}

uint32_t sys_io_read_dword(uint16_t port)
{
    uint32_t buffer;
    ASM("inl %w1, %0": "=a"(buffer) : "Nd"(port));
	return buffer;
}
