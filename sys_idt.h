#ifndef SYS_IDT_H
#define SYS_IDT_H

#include "sys_stdint.h"

/**
 * The IDT consists of 256 interrupt vectors–the first 32 (0–31 or 0x00–0x1F) of which are used for processor exceptions.
 * 
 */

// Max number of entries in IDT table.
#define SYS_IDT_MAX_SIZE 256

enum IDTEntry_GateType_e
{
    IDT_ENTRY_GATE_TYPE_UNKNOWN          = 0x0,
    IDT_ENTRY_GATE_TYPE_16_BIT_TASK      = 0x5, // Offset value is unused and should be set to zero.
    IDT_ENTRY_GATE_TYPE_16_BIT_INTERRUPT = 0x6,
    IDT_ENTRY_GATE_TYPE_16_BIT_TRAP      = 0x7,
    IDT_ENTRY_GATE_TYPE_32_BIT_INTERRUPT = 0xE,
    IDT_ENTRY_GATE_TYPE_32_BIT_TRAP      = 0xF
};

// Single entry in IDT table.
#pragma pack(1)
struct IDTEntry_s
{
    uint16_t offset_1;      // Address of the entry point (bits 0..15).
    uint16_t selector;      // Code segment selector in GDT or LDT.
    uint8_t  reserved;      // Unused - set to 0.
    uint8_t  gate_type : 4; // enum IDTEntry_GateType_e.
    uint8_t  zero      : 1; // Zero bit.
    uint8_t  dpl       : 2; // CPU Privilege Levels which are allowed to access this interrupt via the INT instruction.
    uint8_t  present   : 1; // Entry present bit.
    uint16_t offset_2;      // Address of the entry point (bits 16..32).
};
#pragma pack()

// CPU IDTR register structure.
#pragma pack(1)
struct IDTR_s
{
    uint16_t size;
    struct IDTEntry_s* idt;
    //uint32_t line_address;
};
#pragma pack()

void sys_idt_init();
void sys_idt_install();
void sys_idt_enable();

#endif // SYS_IDT_H
