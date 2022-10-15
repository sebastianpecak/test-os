#include "sys_stdint.h"
#include "sys_multiboot.h"
#include "terminal.h"
#include "sys_stdlib.h"
#include "sys_stddef.h"
#include "sys_sanitizer.h"
#include "sys_tools.h"
#include "sys_stdio.h"

SYS_COMPILE_TIME_ASSERT(sizeof(void*) == sizeof(uint32_t), pointers_not_32bit_wide);

void print_boot_info(const struct MultibootBootInfo_s* bootInfo);

void sys_main(uint32_t magic, uint32_t boot_info_addr)
{
    // To be able to print to screen.
    terminal_initialize();

    const struct MultibootBootInfo_s* boot_info = TYPE_CAST(struct MultibootBootInfo_s*, boot_info_addr);

    SYS_ASSERT(sizeof(void*) == sizeof(uint32_t));
    SYS_ASSERT_NOT_NULL(boot_info);
    SYS_ASSERT(magic == MULTIBOOT_BOOTLOADER_MAGIC);
    SYS_ASSERT(sys_multiboot_is_valid(boot_info) == BOOL_TRUE);

    print_boot_info(boot_info);
}

void print_boot_info_mem(const struct MultibootBootInfo_Mem_s* mem)
{
    sys_printf("--- section MEM ---\n");
    sys_printf("lower: %u KB\n", mem->lower_kb);
    sys_printf("upper: %u KB\n", mem->upper_kb);
}

void print_boot_info_boot_dev(const struct MultibootBootInfo_BootDevice_s* boot_dev)
{
    sys_printf("--- section BOOT DEVICE ---\n");
    sys_printf("(drive/part1/part2/part3): %u/%u/%u/%u\n", boot_dev->drive, boot_dev->part1, boot_dev->part2, boot_dev->part3);
}

void print_boot_info_cmdline(const struct MultibootBootInfo_CmdLine_s* cmdline)
{
    sys_printf("--- section CMDLINE ---\n");
    sys_printf("cmdline: %s\n", cmdline->cstr);
}

void print_boot_info_mods(const struct MultibootBootInfo_Mods_s* mods)
{
    sys_printf("--- section MODS ---\n");
    sys_printf("count: %u\n", mods->count);
    // TODO: print mods list if count > 0.
}

void print_boot_info_symbol_table(const struct MultibootBootInfo_SymbolTable_s* sym_tab)
{
    sys_printf("--- section SYMBOL TABLE ---\n");
    sys_printf("tabsize : %u\n", sym_tab->tabsize);
    sys_printf("strsize : %u\n", sym_tab->strsize);
    sys_printf("addr    : %#x\n", sym_tab->addr);
    sys_printf("reserved: %u\n", sym_tab->reserved);
}

void print_boot_info_elf_header(const struct MultibootBootInfo_ElfHeader_s* elf_hdr)
{
    sys_printf("--- section ELF HEADER ---\n");
    sys_printf("num  : %u\n", elf_hdr->num);
    sys_printf("size : %u\n", elf_hdr->size);
    sys_printf("addr : %#x\n", elf_hdr->addr);
    sys_printf("shndx: %u\n", elf_hdr->shndx);
}

void print_boot_info_mmap(const struct MultibootBootInfo_Mmap_s* mmap)
{
    unsigned int counter = 0;

    sys_printf("--- section MMAP ---\n");
    sys_printf("length: %u\n", mmap->length);
    if (mmap->length == 0)
    {
        return;
    }
    uint8_t* head_byte = mmap->head;
    size_t to_read = mmap->length;
    for (;to_read > 0;)
    {
        struct MultibootBootInfo_MmapEntry_s* current = head_byte;
        sys_printf("[%u] size %u, base_addr %#x, length %u, type %u\n",
            counter++, current->size, current->base_addr, current->length, current->type);
        // Move to next element (size can be different than 20B).
        head_byte += current->size + sizeof(current->size);
        to_read -= (current->size + sizeof(current->size));
    }
}

void print_boot_info_drives(const struct MultibootBootInfo_Drives_s* drives)
{
    unsigned int counter = 0;

    sys_printf("--- section DRIVES ---\n");
    sys_printf("length: %u\n", drives->length);
    if (drives->length == 0)
    {
        return;
    }
    uint8_t* head_byte = drives->drives;
    size_t to_read = drives->length;
    for (;to_read > 0;)
    {
        struct MultibootBootInfo_DriveEntry_s* current = head_byte;
        sys_printf("[%u] size %u, number %u, mode ",
            counter++, current->size, current->number);
        if (current->mode == MULTIBOOT_BOOT_INFO_DRIVE_ENTRY_MODE_CHS)
        {
            sys_printf("CHS, cylinders %u, heads %u, sectors %u\n",
                current->cylinders, current->heads, current->sectors);
        }
        else if (current->mode == MULTIBOOT_BOOT_INFO_DRIVE_ENTRY_MODE_LBA)
        {
            sys_printf("LBA\n");
        }
        else
        {
            sys_printf("UNKNOWN\n");
        }
        // Move to next element.
        head_byte += current->size + sizeof(current->size);
        to_read -= (current->size + sizeof(current->size));
    }
}

void print_boot_info_vbe(const struct MultibootBootInfo_Vbe_s* vbe)
{
    sys_printf("--- section VBE ---\n");
    sys_printf("control_info : %#x\n", vbe->control_info);
    sys_printf("mode_info    : %#x\n", vbe->mode_info);
    sys_printf("mode         : %u\n", vbe->mode);
    sys_printf("interface_seg: %#x\n", vbe->interface_seg);
    sys_printf("interface_off: %#x\n", vbe->interface_off);
    sys_printf("interface_len: %#x\n", vbe->interface_len);
}

// TODO: add %ll support for uint64_t.

void print_boot_info_framebuffer(const struct MultibootBootInfo_FrameBuffer_s* framebuffer)
{
    sys_printf("--- section FRAMEBUFFER ---\n");
    //sys_printf("addr : %#llx\n", framebuffer->addr);
    sys_printf("addr  : %#x\n", (uint32_t)framebuffer->addr);
    sys_printf("pitch : %u\n", framebuffer->pitch);
    sys_printf("width : %u\n", framebuffer->width);
    sys_printf("height: %u\n", framebuffer->height);
    sys_printf("bpp   : %u\n", framebuffer->bpp);
    sys_printf("type  : %u\n", framebuffer->type);
}

void print_boot_info(const struct MultibootBootInfo_s* boot_info)
{
    sys_printf("MULTIBOOT INFO:\n");
    sys_printf("boot-info-addr: %#x\n", ADDRESS_TO_UINT32(boot_info));
    sys_printf("flags         : %#x\n", boot_info->flags);
    if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_MEM)
    {
        print_boot_info_mem(&boot_info->mem);
    }
    if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_BOOT_DEVICE)
    {
        print_boot_info_boot_dev(&boot_info->boot_device);
    }
    // if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_CMDLINE)
    // {
    //     print_boot_info_cmdline(&boot_info->cmdline);
    // }
    if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_MODS)
    {
        print_boot_info_mods(&boot_info->mods);
    }
    // if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_SYMBOL_TABLE)
    // {
    //     print_boot_info_symbol_table(&boot_info->syms.symbol_table);
    // }
    // if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_ELF_HEADER)
    // {
    //     print_boot_info_elf_header(&boot_info->syms.elf_header);
    // }
    // if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_MMAP)
    // {
    //     print_boot_info_mmap(&boot_info->mmap);
    // }
    if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_DRIVES)
    {
        print_boot_info_drives(&boot_info->drives);
    }
    if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_BOOTLOADER_NAME)
    {
        sys_printf("loader-name: %s\n", boot_info->name.cstr);
    }
    if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_VBE)
    {
        print_boot_info_vbe(&boot_info->vbe);
    }
    if (boot_info->flags & MULTIBOOT_BOOTINFO_FLAG_HAS_FRAMEBUFFER)
    {
        print_boot_info_framebuffer(&boot_info->framebuffer);
    }
}
