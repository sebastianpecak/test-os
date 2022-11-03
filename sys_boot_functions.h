#ifndef SYS_BOOT_FUNCTIONS_H
#define SYS_BOOT_FUNCTIONS_H

#define INIT_STACK() \
    do { asm("mov $_stack_top, %esp"); } while (0)

#define HLT_LOOP() \
    do { asm("cli\n 1: hlt\n jmp 1b"); } while (0)

#define HLT_LOOP_WITH_CALL(_func_name) \
    do { asm("1: hlt\n call " #_func_name "\n jmp 1b"); } while (0)

#define GET_BOOT_INFO() \
    do { asm("pushl %ebx\n pushl %eax"); } while (0)

#define CALL(_func_name) \
    do { asm("call " #_func_name); } while (0)

#endif // SYS_BOOT_FUNCTIONS_H
