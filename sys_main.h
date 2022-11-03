#ifndef SYS_MAIN_H
#define SYS_MAIN_H

#include "sys_stdint.h"
#include "sys_multiboot.h"

void sys_main(uint32_t magic, const struct MultibootBootInfo_s* boot_info);

#endif // SYS_MAIN_H
