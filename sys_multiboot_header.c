#include "sys_multiboot.h"
#include "sys_tools.h"

#warning "COMPILER OPTION -O0 IS REQUIRED!"

/**
 * @brief This file contains data for Multiboot-compliant bootloaders.
 *        There must be no optimization enabled to keep these unused consts.
 * 
 */

static const uint32_t _multibootHeaderFlags = (
    MULTIBOOT_HEADER_FLAG_PAGE_ALIGN
#ifdef TESTOS_FLAG_MULTIBOOT_HEADER_HAS_ADDRESS_FIELDS
    | MULTIBOOT_HEADER_FLAG_ADDRESS_FIELDS
#endif // TESTOS_FLAG_MULTIBOOT_HEADER_HAS_ADDRESS_FIELDS
);

__attribute__((unused))
static const struct MultibootHeader_s _multibootHeader = {
    .magic    = MULTIBOOT_HEADER_MAGIC,
    .flags    = _multibootHeaderFlags,
    .checksum = MULTIBOOT_HEADER__MAKE_CHECKSUM(_multibootHeaderFlags)
#ifdef TESTOS_FLAG_MULTIBOOT_HEADER_HAS_ADDRESS_FIELDS
    ,
    .address_fields = {
        .header_addr   = ADDRESS_TO_UINT32(_multibootHeader),
        .load_addr     = 0x100000,
        .load_end_addr = 0,
        .bss_end_addr  = 0,
        .entry_addr    = ADDRESS_TO_UINT32(entry_point_from_bootloader)
    }
#endif // TESTOS_FLAG_MULTIBOOT_HEADER_HAS_ADDRESS_FIELDS
};
