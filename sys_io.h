#ifndef SYS_IO_H
#define SYS_IO_H

#include "sys_stdint.h"

void sys_io_write_byte(uint16_t port, uint8_t value);
void sys_io_write_word(uint16_t port, uint16_t value);
void sys_io_write_dword(uint16_t port, uint32_t value);

uint8_t sys_io_read_byte(uint16_t port);
uint16_t sys_io_read_word(uint16_t port);
uint32_t sys_io_read_dword(uint16_t port);

// void sys_io_test();

#endif // SYS_IO_H
