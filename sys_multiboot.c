#include "sys_multiboot.h"
#include "terminal.h"
#include "sys_stdio.h"

#define CHECK_REAL_AND_EXPECTED_SIZE(_struct) \
    do { \
        /*sys_printf("%s expected %u, real %u\n", #_struct, _struct##_EXPECTED_SIZE, sizeof(struct _struct));*/ \
        if (sizeof(struct _struct) != _struct##_EXPECTED_SIZE) { \
            terminal_writestring_error("INVALID SIZE OF: struct "); \
            terminal_writestring_error(#_struct); \
            terminal_putchar('\n'); \
            return BOOL_FALSE; \
        } \
    } while (0)

Bool_t sys_multiboot_is_valid(const struct MultibootBootInfo_s* multiboot)
{
    // Validate structure size.
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_Mem_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_BootDevice_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_CmdLine_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_Module_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_Mods_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_SymbolTable_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_ElfHeader_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_Syms_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_MmapEntry_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_Mmap_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_DrivePort_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_DriveEntry_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_Drives_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_BootloaderName_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_ApmTable_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_Vbe_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_FrameBufferPallete_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_FrameBufferIndexedColor_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_FrameBufferRgbColor_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_FrameBufferColor_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_FrameBuffer_s);
    CHECK_REAL_AND_EXPECTED_SIZE(MultibootBootInfo_s);

    // 4th and 5th bits in flags are mutually exclusive.
    if ((multiboot->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_SYMBOL_TABLE) != 0 &&
        (multiboot->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_ELF_HEADER) != 0)
    {
        terminal_writestring_error("Flags HAS_SYMBOL_TABLE and HAS_ELF_HEADER are both on.\n");
        return BOOL_FALSE;
    }

    return BOOL_TRUE;
}
