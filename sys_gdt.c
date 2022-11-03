#include "sys_dgt.h"
#include "sys_stdio.h"
#include "sys_stddef.h"
#include "sys_sanitizer.h"
#include "sys_string.h"

// TODO: do not used macro.
#define MAKE_GDT_ENTRY(_gdt_entry, _base_32_bits, _limit_20_bits, _is_exec) \
    do                                                                      \
    {                                                                       \
        (_gdt_entry).base_1 = LOWER_16_BITS(_base_32_bits);                 \
        (_gdt_entry).base_2 = LOWER_8_BITS(UPPER_16_BITS(_base_32_bits));   \
        (_gdt_entry).base_3 = UPPER_8_BITS(UPPER_16_BITS(_base_32_bits));   \
        (_gdt_entry).limit_1 = LOWER_16_BITS(_limit_20_bits);               \
        (_gdt_entry).limit_2 = UPPER_16_BITS(_limit_20_bits);               \
        (_gdt_entry).access_RW = 1;                                         \
        (_gdt_entry).access_E = (_is_exec);                                 \
        (_gdt_entry).access_S = 1;                                          \
        (_gdt_entry).access_P = 1;                                          \
        (_gdt_entry).flag_DB = 1;                                           \
        (_gdt_entry).flag_G = 1;                                            \
    } while (0)

/**
   asm is a statement, not an expression.

    asm <optional stuff> (
        "assembler template"
        : outputs
        : inputs
        : clobbers
        : labels)
 */

static void _print_gdtr(const struct GDTR_s *gdtr)
{
    SYS_ASSERT_NOT_NULL(gdtr);

    const size_t entries_count = (gdtr->size + 1) / sizeof(struct GDTEntry_s);

    sys_printf("+++++++++ GDTR START +++++++++\n");

    // Print GDTR info.
    sys_printf("gdtr.size          = %uB\n", gdtr->size);
    sys_printf("gdtr.offset        = %#x\n", gdtr->gdt);
    sys_printf("gdtr.entries_count = %u\n", entries_count);

    // Print all GDT entries.
    for (size_t i = 0; i < entries_count; ++i)
    {
        const uint32_t base = ((TYPE_CAST(uint32_t, gdtr->gdt[i].base_3) << 24) |
                               (TYPE_CAST(uint32_t, gdtr->gdt[i].base_2) << 16) |
                               TYPE_CAST(uint32_t, gdtr->gdt[i].base_1));
        const uint32_t limit = ((TYPE_CAST(uint32_t, gdtr->gdt[i].limit_2) << 16) |
                                TYPE_CAST(uint32_t, gdtr->gdt[i].limit_1));

        sys_printf("GDT[%u] ", i);
        sys_printf("base %#x, limit %#x, ", base, limit);
        sys_printf("access (A %u, RW %u, DC %u, E %u, S %u, DPL %u, P %u), ",
                   gdtr->gdt[i].access_A, gdtr->gdt[i].access_RW, gdtr->gdt[i].access_DC, gdtr->gdt[i].access_E,
                   gdtr->gdt[i].access_S, gdtr->gdt[i].access_DPL, gdtr->gdt[i].access_P);
        sys_printf("flags (L %u, DB %u, G %u)\n", gdtr->gdt[i].flag_L, gdtr->gdt[i].flag_DB, gdtr->gdt[i].flag_G);
    }

    sys_printf("+++++++++ GDTR END +++++++++\n");
}

/**
 * @brief Show GDT already configured in CPU.
 *
 */
static void _show_current_dgt()
{
    struct GDTR_s gdtr;

    // Load GDTR.
    asm("sgdt %0"
        : "=m"(gdtr)
        :);

    _print_gdtr(&gdtr);
}

/**
 * @brief Our new GDT table.
 *
 */
static struct GDTEntry_s _gdt[SYS_GDT_MAX_SIZE];
static size_t _gdt_entries_count;

void sys_gdt_init()
{
    _show_current_dgt();

    // Entry 0 in GDT must be NULL.
    _gdt_entries_count = 0x10;// 1;
    sys_memset(_gdt, 0, sizeof(_gdt));

    // Global code segment.
    MAKE_GDT_ENTRY(
        _gdt[_gdt_entries_count],
        0x0,
        SYS_GDT_MAX_LIMIT_VALUE,
        1);
    ++_gdt_entries_count;
    // Global data segment.
    MAKE_GDT_ENTRY(
        _gdt[_gdt_entries_count],
        0x0,
        SYS_GDT_MAX_LIMIT_VALUE,
        0);
    ++_gdt_entries_count;
}

void sys_gdt_install()
{
    struct GDTR_s gdtr =
        {
            .size = (sizeof(struct GDTEntry_s) * _gdt_entries_count) - 1,
            .gdt = _gdt};

    //_print_gdtr(&gdtr);

    asm("lgdt %0" ::"m"(gdtr));

    _show_current_dgt();
}
