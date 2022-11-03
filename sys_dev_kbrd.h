#ifndef SYS_DEV_KBRD_H
#define SYS_DEV_KBRD_H

// DEVICE: PS2 KEYBOARD.
// https://wiki.osdev.org/PS/2_Keyboard

#include "sys_stdint.h"

struct DevKbrdState_s
{
    uint8_t is_capslock    : 1;
    uint8_t is_left_shift  : 1;
    uint8_t is_right_shift : 1;
    uint8_t is_left_ctrl   : 1;
    uint8_t is_left_alt    : 1;
};

void sys_dev_kbrd_init();
/**
 * @brief Returns last pressed key in ASCII format.
 * 
 * @return uint8_t 
 */
uint8_t sys_dev_kbrd_get_ascii();

void sys_dev_kbrd_test();

#endif // SYS_DEV_KBRD_H
