#include "sys_idt.h"
#include "sys_stddef.h"
#include "sys_stdio.h"
#include "sys_string.h"
#include "sys_dgt.h"
#include "sys_sanitizer.h"
#include "sys_main.h"

#define MAKE_IDT_ENTRY(_entry, _handler) \
    do { \
        (_entry).selector = SYS_GDT_DEFAULT_CODE_SELECTOR; \
        (_entry).offset_1 = LOWER_16_BITS(_handler); \
        (_entry).offset_2 = UPPER_16_BITS(_handler); \
        (_entry).gate_type = IDT_ENTRY_GATE_TYPE_32_BIT_INTERRUPT; \
        (_entry).present = 1; \
    } while (0)

// This is our IDT table.
static struct IDTEntry_s _idt[SYS_IDT_MAX_SIZE];
static size_t _idt_entries_count;

static void _print_idtr(const struct IDTR_s *idtr)
{
    SYS_ASSERT_NOT_NULL(idtr);

    const size_t entries_count = (idtr->size + 1) / sizeof(struct IDTEntry_s);

    sys_printf("+++++++++ IDTR START +++++++++\n");

    // Print IDTR info.
    sys_printf("idtr.size          = %uB\n", idtr->size);
    sys_printf("idtr.offset        = %#x\n", idtr->idt);
    sys_printf("idtr.entries_count = %u\n", entries_count);

    // Print all GDT entries.
    for (size_t i = 0; i < entries_count; ++i)
    {
        const uint32_t offset = ((TYPE_CAST(uint32_t, idtr->idt[i].offset_2) << 16) |
                                TYPE_CAST(uint32_t, idtr->idt[i].offset_1));

        sys_printf("IDT[%u] ", i);
        sys_printf("offset %#x, selector %#x, reserved %u, gate_type %#x, zero %u, DPL %u, P %u\n",
        offset, idtr->idt[i].selector, idtr->idt[i].reserved, idtr->idt[i].gate_type, idtr->idt[i].zero, idtr->idt[i].dpl, idtr->idt[i].present);
    }

    sys_printf("+++++++++ IDTR END +++++++++\n");
}

/**
 * @brief Show GDT already configured in CPU.
 *
 */
static void _show_current_igt()
{
    struct IDTR_s idtr;

    // Load IDTR.
    asm("sidt %0"
        : "=m"(idtr)
        :);

    _print_idtr(&idtr);
}

#define IRQ_PROLOG() \
    do { asm("pushal"); } while (0)

#define IRQ_EPILOG() \
    do { asm("popal; iret"); } while (0)

__attribute__((naked))
void _irq_not_handled()
{
    IRQ_PROLOG();
    sys_printf("_irq_not_handled\n");
    IRQ_EPILOG();
}

__attribute__((naked))
void _irq_div_by_zero()
{
    IRQ_PROLOG();
    sys_printf("_irq_div_by_zero\n");
    asm("hlt");
    IRQ_EPILOG();
}

__attribute__((naked))
void _irq_keyboard()
{
    IRQ_PROLOG();
    sys_printf("_irq_keyboard\n");
    IRQ_EPILOG();
}

enum IrqIndex_e
{
    IRQ_INDEX_DIV_BY_ZERO = 0,
    IRQ_INDEX_KEYBOARD = 1
};

void sys_idt_init()
{
    //_show_current_igt();

    _idt_entries_count = 0;
    sys_memset(_idt, 0, sizeof(_idt));

    // Set default IRQ handler for all IRQs.
    for (size_t i = 0; i < SYS_IDT_MAX_SIZE; ++i)
    {
        MAKE_IDT_ENTRY(_idt[i], _irq_not_handled);
        ++_idt_entries_count;
    }

    // Set real IRQ handlers.
    MAKE_IDT_ENTRY(_idt[IRQ_INDEX_DIV_BY_ZERO], _irq_div_by_zero);
    MAKE_IDT_ENTRY(_idt[IRQ_INDEX_KEYBOARD]   , _irq_keyboard);
}

void sys_idt_install()
{
    struct IDTR_s idtr =
    {
        .size = (sizeof(struct IDTEntry_s) * _idt_entries_count) - 1, //sizeof(_idt) - 1,
        .idt  = _idt
    };

    asm("lidt %0" :: "m"(idtr));

    //_show_current_igt();
}

void sys_idt_enable()
{
    asm("sti");
}
