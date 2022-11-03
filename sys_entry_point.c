#include "sys_entry_point.h"
#include "sys_boot_functions.h"
#include "sys_main.h"

#define ONE_KB 1024
#define DEFAULT_STACK_SIZE (ONE_KB * 16)

__attribute__((unused))
static uint8_t _stack_base[DEFAULT_STACK_SIZE];
__attribute__((unused))
static uint8_t _stack_top;

void __attribute__((naked)) sys_entry_point(void)
{
    INIT_STACK();
    GET_BOOT_INFO();
    CALL(sys_main);
    HLT_LOOP();
}
