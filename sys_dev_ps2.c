#include "sys_dev_ps2.h"
#include "sys_stddef.h"
#include "sys_io.h"
#include "sys_stdio.h"
#include "sys_sanitizer.h"
#include "sys_string.h"

#define CMD_GET_RAM_BYTE 0x20
#define CMD_SET_RAM_BYTE 0x60
#define CMD_ENABLE_FIRST_PORT 0xAE
#define CMD_DISABLE_FIRST_PORT  0xAD
#define CMD_DISABLE_SECOND_PORT 0xA7
#define CMD_SELF_TEST 0xAA
#define CMD_SELF_TEST_RESP_PASS 0x55
#define CMD_SELF_TEST_RESP_FAIL 0xFC
#define CMD_TEST_FIRST_PORT 0xAB
#define CMD_TEST_FIRST_PORT_RESP_PASS 0
#define CMD_DEV_RESET 0xFF
#define CMD_DEV_RESET_RESP_PASS 0xFA
#define CMD_DEV_RESET_RESP_FAIL 0xFC
#define CMD_WRITE_TO_SECOND_PORT 0xD4
#define CONFIG_BYTE 0
#define CONFIG_BYTE_MAX 0x1F

static void _print_status_reg(const struct DevPs2StatusRegFlags_s* status_reg)
{
    sys_printf("status_reg(%#x): output_buffer_status %u, input_buffer_status %u, system_flag %u, command_data %u, unknown_1 %u, unknown_2 %u, timeout_error %u, parity_error %u\n",
        status_reg,
        status_reg->output_buffer_status,
        status_reg->input_buffer_status,
        status_reg->system_flag,
        status_reg->command_data,
        status_reg->unknown_1,
        status_reg->unknown_2,
        status_reg->timeout_error,
        status_reg->parity_error
    );
}

static void _print_config(const struct DevPs2Configuration_s* config)
{
    sys_printf("config(%#x): first_port_irq %u, second_port_irq %u, system_flag %u, zero_1 %u, first_port_clock %u, second_port_clock %u, first_port_translation %u, zero_2 %u\n",
        *TYPE_CAST(uint8_t*, config),
        config->first_port_irq,
        config->second_port_irq,
        config->system_flag,
        config->zero_1,
        config->first_port_clock,
        config->second_port_clock,
        config->first_port_translation,
        config->zero_2
    );
}

void sys_dev_ps2_wait_for_input()
{
    uint8_t status_reg_buffer = 0;
    struct DevPs2StatusRegFlags_s* status_reg = TYPE_CAST(struct DevPs2StatusRegFlags_s*, &status_reg_buffer);

    do
    {
        status_reg_buffer = sys_io_read_byte(DEV_PS2_PORT_CTRL);
    } while (status_reg->output_buffer_status == 0);
}

void sys_dev_ps2_wait_for_output()
{
    uint8_t status_reg_buffer = 0;
    struct DevPs2StatusRegFlags_s* status_reg = TYPE_CAST(struct DevPs2StatusRegFlags_s*, &status_reg_buffer);

    do
    {
        status_reg_buffer = sys_io_read_byte(DEV_PS2_PORT_CTRL);
    } while (status_reg->input_buffer_status != 0);
}

uint8_t sys_dev_ps2_poll_data()
{
    sys_dev_ps2_wait_for_input();
    return sys_io_read_byte(DEV_PS2_PORT_DATA);
}

void sys_dev_ps2_write_cmd(uint8_t command)
{
    sys_dev_ps2_wait_for_output();
    sys_io_write_byte(DEV_PS2_PORT_CTRL, command);
}

void sys_dev_ps2_write_data(uint8_t data)
{
    sys_dev_ps2_wait_for_output();
    sys_io_write_byte(DEV_PS2_PORT_DATA, data);
}

void sys_dev_ps2_get_status_reg(struct DevPs2StatusRegFlags_s* output)
{
    uint8_t* status_reg_buffer = TYPE_CAST(uint8_t*, output);
    *status_reg_buffer = sys_io_read_byte(DEV_PS2_PORT_CTRL);
}

static uint8_t _read_ps2_ram_byte(uint8_t byte_idx)
{
    SYS_ASSERT(byte_idx <= CONFIG_BYTE_MAX);

    const uint8_t command = CMD_GET_RAM_BYTE | byte_idx;
    sys_dev_ps2_write_cmd(command);
    return sys_dev_ps2_poll_data();
}

void sys_dev_ps2_get_config(struct DevPs2Configuration_s* output)
{
    uint8_t* buffer = TYPE_CAST(uint8_t*, output);
    *buffer = _read_ps2_ram_byte(CONFIG_BYTE);
}

static void _write_ps2_ram_byte(uint8_t value, uint8_t byte_idx)
{
    SYS_ASSERT(byte_idx <= CONFIG_BYTE_MAX);

    const uint8_t command = CMD_SET_RAM_BYTE | byte_idx;
    sys_dev_ps2_write_cmd(command);
    sys_dev_ps2_write_data(value);
}

void sys_dev_ps2_set_config(const struct DevPs2Configuration_s* config)
{
    _write_ps2_ram_byte(*TYPE_CAST(uint8_t*, config), CONFIG_BYTE);
}

static void _disable_first_port()
{
    sys_dev_ps2_write_cmd(CMD_DISABLE_FIRST_PORT);
}

static void _disable_second_port()
{
    sys_dev_ps2_write_cmd(CMD_DISABLE_SECOND_PORT);
}

static void _enable_first_port()
{
    sys_dev_ps2_write_cmd(CMD_ENABLE_FIRST_PORT);
}

static int _validate_ps2_config(const struct DevPs2Configuration_s* config)
{
    if (config->zero_1 != 0)
    {
        sys_printf("_validate_ps2_config: zero_1 is not ZERO.\n");
        return -1;
    }
    if (config->zero_2 != 0)
    {
        sys_printf("_validate_ps2_config: zero_2 is not ZERO.\n");
        return -1;
    }
    if (config->system_flag != 1)
    {
        sys_printf("_validate_ps2_config: system_flag is not set.\n");
        return -1;
    }

    return 0;
}

void sys_dev_ps2_write_to_first_port(uint8_t data)
{
    sys_dev_ps2_write_data(data);
}

void sys_dev_ps2_write_to_second_port(uint8_t data)
{
    sys_dev_ps2_write_cmd(CMD_WRITE_TO_SECOND_PORT);
    sys_dev_ps2_write_data(data);
}

static int _ps2_apply_new_config(const struct DevPs2Configuration_s* config)
{
    SYS_ASSERT_NOT_NULL(config);

    struct DevPs2Configuration_s config_verify;

    sys_dev_ps2_set_config(config);
    sys_dev_ps2_get_config(&config_verify);
    if (sys_memcmp(config, &config_verify, sizeof(struct DevPs2Configuration_s)) != 0)
    {
        sys_printf("_ps2_apply_new_config: failed to apply new PS2 configuration.\n");
        _print_config(config);
        _print_config(&config_verify);
        return -1;
    }

    return 0;
}

static void _enable_first_port_irq()
{
    struct DevPs2Configuration_s config;

    sys_dev_ps2_get_config(&config);
    if (_validate_ps2_config(&config) < 0)
    {
        sys_printf("_enable_first_port_irq: PS2 config validation failed.\n");
        return;
    }

    config.first_port_irq = 1;
    _ps2_apply_new_config(&config);
}

//  Step 1: Initialise USB Controllers

// This has nothing to do with the PS/2 Controller or PS/2 Devices, however if the system is using (typically limited/dodgy) USB Legacy Support it will interfere with PS/2 Controller initialisation. Therefore you need to initialise USB controllers and disable USB Legacy Support beforehand.
// Step 2: Determine if the PS/2 Controller Exists

// Before you touch the PS/2 controller at all, you should determine if it exists. On some systems (e.g. 80x86 Apple machines) it doesn't exist and any attempt to touch it can cause a system crash. The correct way to do this is is with ACPI. Check bit 1 (value = 2, the "8042" flag) in the "IA PC Boot Architecture Flags" field at offset 109 in the Fixed ACPI Description Table (FADT). If this bit is clear, then there is no PS/2 Controller to configure. Otherwise, if the bit is set, or the system doesn't support ACPI (no ACPI tables and no FADT) then there is a PS/2 Controller.
// Step 3: Disable Devices

// So that any PS/2 devices can't send data at the wrong time and mess up your initialisation; start by sending a command 0xAD and command 0xA7 to the PS/2 controller. If the controller is a "single channel" device, it will ignore the "command 0xA7".
// Step 4: Flush The Output Buffer

// Sometimes (e.g. due to interrupt controlled initialisation causing a lost IRQ) data can get stuck in the PS/2 controller's output buffer. To guard against this, now that the devices are disabled (and can't send more data to the output buffer) it can be a good idea to flush the controller's output buffer. There's 2 ways to do this - poll bit 0 of the Status Register (while reading from IO Port 0x60 if/when bit 0 becomes set), or read from IO Port 0x60 without testing bit 0. Either way should work (as you're discarding the data and don't care what it was).
// Step 5: Set the Controller Configuration Byte

// Because some bits of the Controller Configuration Byte are "unknown", this means reading the old value (command 0x20), changing some bits, then writing the changed value back (command 0x60). You want to disable all IRQs and disable translation (clear bits 0, 1 and 6).

// While you've got the Configuration Byte, test if bit 5 was set. If it was clear, then you know it can't be a "dual channel" PS/2 controller (because the second PS/2 port should be disabled).
// Step 6: Perform Controller Self Test

// To test the PS/2 controller, send command 0xAA to it. Then wait for its response and check it replied with 0x55. Note: this can reset the PS/2 controller on some hardware (tested on a 2016 laptop). At the very least, the Controller Configuration Byte should be restored for compatibility with such hardware. You can either determine the correct value yourself based on the above table or restore the value read before issuing 0xAA.
// Step 7: Determine If There Are 2 Channels

// If you know it's a single channel controller (from Step 5) then skip this step. Otherwise, send a command 0xAE to enable the second PS/2 port and read the Controller Configuration Byte again. Now bit 5 of the Controller Configuration Byte should be clear - if it's set then you know it can't be a "dual channel" PS/2 controller (because the second PS/2 port should be enabled). If it is a dual channel device, send a command 0xA7 to disable the second PS/2 port again.
// Step 8: Perform Interface Tests

// This step tests the PS/2 ports. Use command 0xAB to test the first PS/2 port, then check the result. Then (if it's a "dual channel" controller) use command 0xA9 to test the second PS/2 port, then check the result.

// At this stage, check to see how many PS/2 ports are left. If there aren't any that work you can just give up (display errors and terminate the PS/2 Controller driver). Note: If one of the PS/2 ports on a dual PS/2 controller fails, then you can still keep using/supporting the other PS/2 port.
// Step 9: Enable Devices

// Enable any PS/2 port that exists and works using command 0xAE (for the first port) and command 0xA8 (for the second port). If you're using IRQs (recommended), also enable interrupts for any (usable) PS/2 ports in the Controller Configuration Byte (set bit 0 for the first PS/2 port, and/or bit 1 for the second PS/2 port, then set it with command 0x60).
// Step 10: Reset Devices

// All PS/2 devices should support the "reset" command (which is a command for the device, and not a command for the PS/2 Controller). To send the reset, just send the byte 0xFF to each (usable) device. The device/s will respond with 0xFA (success) or 0xFC (failure), or won't respond at all (no device present). If your code supports "hot-plug PS/2 devices" (see later), then you can assume each device is "not present" and let the hot-plug code figure out that the device is present if/when 0xFA or 0xFC is received on a PS/2 port.

// TODO: Potential wiki error, the 0xFF reset device command seems to respond 0xAA and then 0xFA on self-test success, and PS/2_Keyboard agrees. --Sortie 14:15, 27 April 2015 (CDT)

// TODO: Sortie: I don't think so, I send byte 0xFF to keyboard (or mouse), then received 0xFA (command acknowledged) after that received 0xAA (self test successful) the last one was 0x00 - mouse device ID. --Akasei 00:35, 19 February 2017 (UTC+1) 

void sys_dev_ps2_init()
{
    struct DevPs2Configuration_s config;
    uint8_t data = 0;

    // Do we have PS2 controller - NOT IMPLEMENTED YET.
    // Disable all PS2 devices.
    _disable_first_port();
    _disable_second_port();
    // Flush PS2 output buffer / discard data.
    (void) sys_io_read_byte(DEV_PS2_PORT_DATA);
    // Configure PS2 controller.
    sys_dev_ps2_get_config(&config);
    if (_validate_ps2_config(&config) < 0)
    {
        return;
    }
    config.first_port_irq         = 0;
    config.second_port_irq        = 0;
    config.first_port_translation = 0;
    //_print_config(&config_to_set);
    if (_ps2_apply_new_config(&config) < 0)
    {
        return;
    }
    // Test PS2 controller.
    sys_dev_ps2_write_cmd(CMD_SELF_TEST);
    data = sys_dev_ps2_poll_data();
    if (data != CMD_SELF_TEST_RESP_PASS)
    {
        sys_printf("sys_dev_ps2_init: PS2 controller test failed %#x.\n", data);
        return;
    }
    // Perform interface tests.
    sys_dev_ps2_write_cmd(CMD_TEST_FIRST_PORT);
    data = sys_dev_ps2_poll_data();
    if (data != CMD_TEST_FIRST_PORT_RESP_PASS)
    {
        sys_printf("sys_dev_ps2_init: First port test failed %#x.\n", data);
        return;
    }
    // Enable devices.
    _enable_first_port();
    _enable_first_port_irq();
    // Reset devices.
    sys_dev_ps2_write_to_first_port(CMD_DEV_RESET);
    //sys_dev_ps2_write_data(CMD_DEV_RESET);
    data = sys_dev_ps2_poll_data();
    if (data != CMD_DEV_RESET_RESP_PASS)
    {
        sys_printf("sys_dev_ps2_init: dev reset failed %#x.\n", data);
        return;
    }
}

void sys_dev_ps2_test()
{
    uint8_t data = 0;

    sys_printf("sys_dev_ps2_test: start\n");
    do
    {
        data = sys_dev_ps2_poll_data();
        sys_printf("%#x\n", data);
    } while (data != 1);
    
    sys_printf("sys_dev_ps2_test: finished\n");
}
