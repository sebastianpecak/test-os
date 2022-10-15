#ifndef SYS_MULTIBOOT_H
#define SYS_MULTIBOOT_H

#include "sys_stdint.h"
#include "sys_limits.h"
#include "sys_stdlib.h"

// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

/**
 * @brief This value must be present in the OS image at the beginning of Multiboot structure.
 * 
 */
#define MULTIBOOT_HEADER_MAGIC     0x1BADB002UL
/**
 * @brief This value is returned to operating system through eax register by Multiboot bootloader.
 * 
 */
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002UL

/**
 * @brief Generates Multiboot header checksum field value based on the flags value.
 * 
 */
#define MULTIBOOT_HEADER__MAKE_CHECKSUM(_flags) \
    (UINT32_MAX_VALUE - MULTIBOOT_HEADER_MAGIC - (_flags) + 1)

/**
 * @brief These flags are set in Multiboot header (data for bootloader).
 * 
 */
enum MultibootHeaderFlags_e
{
    MULTIBOOT_HEADER_FLAG_NONE            = 0,
    MULTIBOOT_HEADER_FLAG_PAGE_ALIGN      = (1 << 0), // Bootloader must align all boot modules on i386 page (4KB) boundaries.
    MULTIBOOT_HEADER_FLAG_MEMORY_INFO     = (1 << 1), // Bootloader must pass memory information to OS.
    MULTIBOOT_HEADER_FLAG_GRAPHICS_FIELDS = (1 << 2), // Specify the preferred graphics mode to bootloader.
    MULTIBOOT_HEADER_FLAG_ADDRESS_FIELDS  = (1 << 16) // This flag indicates the use of the address fields in the header.
};

#pragma pack(1)

struct MultibootHeaderAddressFields_s
{
    uint32_t header_addr;   // Address corresponding to the beginning of the Multiboot header.
    uint32_t load_addr;     // Contains the physical address of the beginning of the text segment.
    uint32_t load_end_addr; // Contains the physical address of the end of the data segment.
    uint32_t bss_end_addr;  // Contains the physical address of the end of the bss segment.
    uint32_t entry_addr;    // The physical address to which the boot loader should jump in order to start running the operating system. 
};

struct MultibootHeaderGraphicsFields_s
{
    uint32_t mode_type;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
};

struct MultibootHeader_s
{
    // MANDATORY FIELDS.
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    // OPTIONAL FIELDS.
#ifdef TESTOS_FLAG_MULTIBOOT_HEADER_HAS_ADDRESS_FIELDS
    struct MultibootHeaderAddressFields_s address_fields;
#endif // TESTOS_FLAG_MULTIBOOT_HEADER_HAS_ADDRESS_FIELDS
#ifdef TESTOS_FLAG_MULTIBOOT_HEADER_HAS_GRAPHICS_FIELDS
    struct MultibootHeaderGraphicsFields_s graphics_fields;
#endif // TESTOS_FLAG_MULTIBOOT_HEADER_HAS_GRAPHICS_FIELDS
};

enum MultibootBootInfoFlags_e
{
    MULTIBOOT_BOOTINFO_FLAG_NONE                = 0,
    MULTIBOOT_BOOTINFO_FLAG_HAS_MEM             = (1 << 0),
    MULTIBOOT_BOOTINFO_FLAG_HAS_BOOT_DEVICE     = (1 << 1),
    MULTIBOOT_BOOTINFO_FLAG_HAS_CMDLINE         = (1 << 2),
    MULTIBOOT_BOOTINFO_FLAG_HAS_MODS            = (1 << 3),
    MULTIBOOT_BOOTINFO_FLAG_HAS_SYMBOL_TABLE    = (1 << 4),
    MULTIBOOT_BOOTINFO_FLAG_HAS_ELF_HEADER      = (1 << 5),
    MULTIBOOT_BOOTINFO_FLAG_HAS_MMAP            = (1 << 6),
    MULTIBOOT_BOOTINFO_FLAG_HAS_DRIVES          = (1 << 7),
    MULTIBOOT_BOOTINFO_FLAG_HAS_CONFIG_TABLE    = (1 << 8),
    MULTIBOOT_BOOTINFO_FLAG_HAS_BOOTLOADER_NAME = (1 << 9),
    MULTIBOOT_BOOTINFO_FLAG_HAS_APM_TABLE       = (1 << 10),
    MULTIBOOT_BOOTINFO_FLAG_HAS_VBE             = (1 << 11),
    MULTIBOOT_BOOTINFO_FLAG_HAS_FRAMEBUFFER     = (1 << 12),
};

#define MultibootBootInfo_Mem_s_EXPECTED_SIZE 8
struct MultibootBootInfo_Mem_s
{
    uint32_t lower_kb; // Starts at address 0x0.
    uint32_t upper_kb; // Starts at address 0x100000.
};

#define MultibootBootInfo_BootDevice_s_EXPECTED_SIZE 4
struct MultibootBootInfo_BootDevice_s
{
    union
    {
        uint32_t boot_device;
        struct
        {
            uint8_t part3;
            uint8_t part2;
            uint8_t part1;
            uint8_t drive;
        };
    };
};

#define MultibootBootInfo_CmdLine_s_EXPECTED_SIZE 4
struct MultibootBootInfo_CmdLine_s
{
    union
    {
        uint32_t address;
        const char* cstr;
    };
};

#define MultibootBootInfo_Module_s_EXPECTED_SIZE 16
struct MultibootBootInfo_Module_s
{
    uint32_t start;
    uint32_t end;
    union
    {
        uint32_t string;
        const char* string_cstr;
    };
    uint32_t reserved;
};

#define MultibootBootInfo_Mods_s_EXPECTED_SIZE 8
struct MultibootBootInfo_Mods_s
{
    uint32_t count;
    union
    {
        uint32_t addr;
        struct MultibootBootInfo_Module_s* head;
    };
};

#define MultibootBootInfo_SymbolTable_s_EXPECTED_SIZE 16
struct MultibootBootInfo_SymbolTable_s
{
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr;
    uint32_t reserved;
};

#define MultibootBootInfo_ElfHeader_s_EXPECTED_SIZE 16
struct MultibootBootInfo_ElfHeader_s
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
};

#define MultibootBootInfo_Syms_s_EXPECTED_SIZE MultibootBootInfo_SymbolTable_s_EXPECTED_SIZE
struct MultibootBootInfo_Syms_s
{
    union
    {
        struct MultibootBootInfo_SymbolTable_s symbol_table;
        struct MultibootBootInfo_ElfHeader_s   elf_header;
    };
};

enum MultibootBootInfo_MmapEntryType_e
{
    MULTIBOOT_BOOT_INFO_MMAP_ENTRY_TYPE_UNKNOWN           = 0,
    MULTIBOOT_BOOT_INFO_MMAP_ENTRY_TYPE_AVAILABLE_RAM     = 1,
    MULTIBOOT_BOOT_INFO_MMAP_ENTRY_TYPE_APCI_INFO         = 3,
    MULTIBOOT_BOOT_INFO_MMAP_ENTRY_TYPE_PRESERVE_ON_HIBER = 4,
    MULTIBOOT_BOOT_INFO_MMAP_ENTRY_TYPE_UNAVAILABLE_RAM   = 5
};

#define MultibootBootInfo_MmapEntry_s_EXPECTED_SIZE 24
struct MultibootBootInfo_MmapEntry_s
{
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
};

#define MultibootBootInfo_Mmap_s_EXPECTED_SIZE 8
struct MultibootBootInfo_Mmap_s
{
    uint32_t length;
    union
    {
        uint32_t addr;
        struct MultibootBootInfo_MmapEntry_s* head;
    };
};

#define MultibootBootInfo_DrivePort_s_EXPECTED_SIZE 2
struct MultibootBootInfo_DrivePort_s
{
    uint16_t port;
};

enum MultibootBootInfo_DriveEntryMode_e
{
    MULTIBOOT_BOOT_INFO_DRIVE_ENTRY_MODE_CHS     = 0,
    MULTIBOOT_BOOT_INFO_DRIVE_ENTRY_MODE_LBA     = 1,
    MULTIBOOT_BOOT_INFO_DRIVE_ENTRY_MODE_UNKKOWN = 0xFF
};

#define MultibootBootInfo_DriveEntry_s_EXPECTED_SIZE (10 + MultibootBootInfo_DrivePort_s_EXPECTED_SIZE)
struct MultibootBootInfo_DriveEntry_s
{
    uint32_t size;
    union
    {
        uint8_t  _placeholder[6];
        uint8_t  number;
        uint8_t  mode;
        uint16_t cylinders;
        uint8_t  heads;
        uint8_t  sectors;
    };
    struct MultibootBootInfo_DrivePort_s ports[1];
};

#define MultibootBootInfo_Drives_s_EXPECTED_SIZE 8
struct MultibootBootInfo_Drives_s
{
    uint32_t length;
    union
    {
        uint32_t addr;
        struct MultibootBootInfo_DriveEntry_s* drives;
    };
};

#define MultibootBootInfo_BootloaderName_s_EXPECTED_SIZE 4
struct MultibootBootInfo_BootloaderName_s
{
    union
    {
        uint32_t addr;
        const char* cstr;
    };
};

#define MultibootBootInfo_ApmTable_s_EXPECTED_SIZE 20
struct MultibootBootInfo_ApmTable_s
{
    uint16_t version;
    uint16_t cseg;
    uint32_t offset;
    uint16_t cseg_16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg_16_len;
    uint16_t dseg_len;
};

#define MultibootBootInfo_Vbe_s_EXPECTED_SIZE 16
struct MultibootBootInfo_Vbe_s
{
    uint32_t control_info;
    uint32_t mode_info;
    uint16_t mode;
    uint16_t interface_seg;
    uint16_t interface_off;
    uint16_t interface_len;
};

enum MultibootBootInfo_FrameBufferType_e
{
    MULTIBOOT_BOOT_INFO_FRAMEBUFFER_TYPE_INDEXED_COLOR = 0,
    MULTIBOOT_BOOT_INFO_FRAMEBUFFER_TYPE_RBG_COLOR     = 1,
    MULTIBOOT_BOOT_INFO_FRAMEBUFFER_TYPE_EGA_TEXT_MODE = 2,
    MULTIBOOT_BOOT_INFO_FRAMEBUFFER_TYPE_CGA_TEXT_MODE = 16,
    MULTIBOOT_BOOT_INFO_FRAMEBUFFER_TYPE_UNKNOWN       = 0xFF
};

#define MultibootBootInfo_FrameBufferPallete_s_EXPECTED_SIZE 3
struct MultibootBootInfo_FrameBufferPallete_s
{
    union
    {
        uint8_t _placeholder[3];
        uint8_t red_value;
        uint8_t green_value;
        uint8_t blue_value;
    };
};

#define MultibootBootInfo_FrameBufferIndexedColor_s_EXPECTED_SIZE 6
//#pragma pack(1)
struct MultibootBootInfo_FrameBufferIndexedColor_s
{
    union
    {
        uint32_t palette_addr;
        struct MultibootBootInfo_FrameBufferPallete_s* pallete;
    };
    uint16_t palette_num_colors;
};
//#pragma pack()

#define MultibootBootInfo_FrameBufferRgbColor_s_EXPECTED_SIZE 6
//#pragma pack(1)
struct MultibootBootInfo_FrameBufferRgbColor_s
{
    uint32_t palette_addr;
    uint16_t palette_num_colors;
};
//#pragma pack()

#define MultibootBootInfo_FrameBufferColor_s_EXPECTED_SIZE 6
struct MultibootBootInfo_FrameBufferColor_s
{
    union
    {
        uint8_t _placeholder[6];
        struct MultibootBootInfo_FrameBufferIndexedColor_s indexed;
        struct MultibootBootInfo_FrameBufferRgbColor_s rgb;
    };
};

#define MultibootBootInfo_FrameBuffer_s_EXPECTED_SIZE (22 + MultibootBootInfo_FrameBufferColor_s_EXPECTED_SIZE)
struct MultibootBootInfo_FrameBuffer_s
{
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    union
    {
        uint16_t _placeholder;
        uint8_t bpp;
        uint8_t type;
    };
    struct MultibootBootInfo_FrameBufferColor_s color_info;
};

#define MultibootBootInfo_s_EXPECTED_SIZE (            \
    sizeof(uint32_t)                                 + \
    MultibootBootInfo_Mem_s_EXPECTED_SIZE            + \
    MultibootBootInfo_BootDevice_s_EXPECTED_SIZE     + \
    MultibootBootInfo_CmdLine_s_EXPECTED_SIZE        + \
    MultibootBootInfo_Mods_s_EXPECTED_SIZE           + \
    MultibootBootInfo_Syms_s_EXPECTED_SIZE           + \
    MultibootBootInfo_Mmap_s_EXPECTED_SIZE           + \
    MultibootBootInfo_Drives_s_EXPECTED_SIZE         + \
    sizeof(uint32_t)                                 + \
    MultibootBootInfo_BootloaderName_s_EXPECTED_SIZE + \
    sizeof(struct MultibootBootInfo_ApmTable_s*)     + \
    MultibootBootInfo_Vbe_s_EXPECTED_SIZE            + \
    MultibootBootInfo_FrameBuffer_s_EXPECTED_SIZE      \
)

struct MultibootBootInfo_s
{
    uint32_t                                  flags;
    struct MultibootBootInfo_Mem_s            mem;          // MULTIBOOT_BOOTINFO_FLAG_HAS_MEM
    struct MultibootBootInfo_BootDevice_s     boot_device;  // MULTIBOOT_BOOTINFO_FLAG_HAS_BOOT_DEVICE
    struct MultibootBootInfo_CmdLine_s        cmdline;      // MULTIBOOT_BOOTINFO_FLAG_HAS_CMDLINE
    struct MultibootBootInfo_Mods_s           mods;         // MULTIBOOT_BOOTINFO_FLAG_HAS_MODS
    struct MultibootBootInfo_Syms_s           syms;         // MULTIBOOT_BOOTINFO_FLAG_HAS_SYMBOL_TABLE || MULTIBOOT_BOOTINFO_FLAG_HAS_ELF_HEADER
    struct MultibootBootInfo_Mmap_s           mmap;         // MULTIBOOT_BOOTINFO_FLAG_HAS_MMAP
    struct MultibootBootInfo_Drives_s         drives;       // MULTIBOOT_BOOTINFO_FLAG_HAS_DRIVES
    uint32_t                                  config_table; // MULTIBOOT_BOOTINFO_FLAG_HAS_CONFIG_TABLE
    struct MultibootBootInfo_BootloaderName_s name;         // MULTIBOOT_BOOTINFO_FLAG_HAS_BOOTLOADER_NAME
    struct MultibootBootInfo_ApmTable_s*      apm_table;    // MULTIBOOT_BOOTINFO_FLAG_HAS_APM_TABLE
    struct MultibootBootInfo_Vbe_s            vbe;          // MULTIBOOT_BOOTINFO_FLAG_HAS_VBE
    struct MultibootBootInfo_FrameBuffer_s    framebuffer;  // MULTIBOOT_BOOTINFO_FLAG_HAS_FRAMEBUFFER
};
#pragma pack()

/**
 * @brief This function validates MultibootBootInfo_s structure contents received from bootloader.
 *        Returns TRUE if data is valid.
 *        Returns FALSE otherwise.
 * 
 * @param multiboot 
 * @return int 
 */
Bool_t sys_multiboot_is_valid(const struct MultibootBootInfo_s* multiboot);

#endif // SYS_MULTIBOOT_H
