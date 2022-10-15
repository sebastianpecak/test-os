#ifndef SYS_ENTRY_POINT_H
#define SYS_ENTRY_POINT_H

/**
 * @brief This is the ultimate entry point to the kernel.
 *        Bootloader passes control to this very function.
 *        There is NO STACK SET UP yet.
 *        NAKED function means no stack manipulation on calling.
 * 
 */
void __attribute__((naked)) sys_entry_point(void);

#endif // SYS_ENTRY_POINT_H
