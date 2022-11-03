#ifndef SYS_DEV_PS2_H
#define SYS_DEV_PS2_H

// DEVICE: PS2 CONTROLLER.
// https://wiki.osdev.org/%228042%22_PS/2_Controller

#include "sys_stdint.h"

#define DEV_PS2_PORT_DATA 0x60
#define DEV_PS2_PORT_CTRL 0x64

#define SYS_ATTR_NO_PADDING __attribute__((packed))

struct DevPs2StatusRegFlags_s
{
    uint8_t output_buffer_status : 1; // Must be set before attempting to read data from IO port 0x60.
    uint8_t input_buffer_status  : 1; // Must be clear before attempting to write data to IO port 0x60 or IO port 0x64.
    uint8_t system_flag          : 1; // Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST).
    uint8_t command_data         : 1; // Command/data (0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command).
    uint8_t unknown_1            : 1;
    uint8_t unknown_2            : 1;
    uint8_t timeout_error        : 1; // Time-out error (0 = no error, 1 = time-out error).
    uint8_t parity_error         : 1; // Parity error (0 = no error, 1 = parity error).
} SYS_ATTR_NO_PADDING;

struct DevPs2Configuration_s
{
    uint8_t first_port_irq         : 1; // 1 = enabled, 0 = disabled
    uint8_t second_port_irq        : 1; // 1 = enabled, 0 = disabled, only if 2 PS/2 ports supported
    uint8_t system_flag            : 1; // 1 = system passed POST, 0 = your OS shouldn't be running
    uint8_t zero_1                 : 1;
    uint8_t first_port_clock       : 1; // 1 = disabled, 0 = enabled
    uint8_t second_port_clock      : 1; // 1 = disabled, 0 = enabled, only if 2 PS/2 ports supported
    uint8_t first_port_translation : 1; // 1 = enabled, 0 = disabled
    uint8_t zero_2                 : 1;
} SYS_ATTR_NO_PADDING;

/**
 * @brief Waits for data and reads one byte.
 * 
 * @return uint8_t 
 */
uint8_t sys_dev_ps2_poll_data();
/**
 * @brief Spinlock: waits until input data is available to read.
 * 
 */
void sys_dev_ps2_wait_for_input();
/**
 * @brief Spinlock: waits until output data is possible to write.
 * 
 */
void sys_dev_ps2_wait_for_output();

void sys_dev_ps2_write_cmd(uint8_t command);
void sys_dev_ps2_write_data(uint8_t data);

void sys_dev_ps2_write_to_first_port(uint8_t data);
void sys_dev_ps2_write_to_second_port(uint8_t data);

void sys_dev_ps2_get_status_reg(struct DevPs2StatusRegFlags_s* output);
void sys_dev_ps2_get_config(struct DevPs2Configuration_s* output);

void sys_dev_ps2_set_config(const struct DevPs2Configuration_s* config);

void sys_dev_ps2_init();

void sys_dev_ps2_test();

#endif // SYS_DEV_PS2_H
