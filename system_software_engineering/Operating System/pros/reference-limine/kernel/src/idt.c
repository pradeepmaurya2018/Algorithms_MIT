#include <stdint.h>
#include "idt.h"
#include "serial.h"

#define IDT_INTERRUPT_GATE 0x8E

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct idtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_ENTRIES];

extern void isr0(void);
extern void isr6(void);
extern void isr8(void);
extern void isr13(void);
extern void isr14(void);

static void idt_set_gate(
    uint8_t vector,
    void (*handler)(void),
    uint16_t selector
) {
    uint64_t address = (uint64_t)handler;

    idt[vector].offset_low =
        (uint16_t)(address & 0xFFFF);

    idt[vector].selector = selector;
    idt[vector].ist = 0;
    idt[vector].type_attr = IDT_INTERRUPT_GATE;

    idt[vector].offset_middle =
        (uint16_t)((address >> 16) & 0xFFFF);

    idt[vector].offset_high =
        (uint32_t)((address >> 32) & 0xFFFFFFFF);

    idt[vector].reserved = 0;
}

void idt_init(void)
{
    /* 1. Clear all IDT entries. */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i] = (struct idt_entry){0};
    }

    /* 2. Read the current code segment selector. */
    uint16_t cs;

    __asm__ volatile (
        "mov %%cs, %0"
        : "=r"(cs)
    );

    /* 3. Install exception gates before loading the IDT. */

    /* Divide Error — vector 0 */
    idt_set_gate(0, isr0, cs);

    /* Invalid Opcode — vector 6 */
    idt_set_gate(6, isr6, cs);

    /* Double Fault — vector 8 */
    idt_set_gate(8, isr8, cs);

    /* General Protection Fault — vector 13 */
    idt_set_gate(13, isr13, cs);

    /* Page Fault — vector 14 */
    idt_set_gate(PAGE_FAULT_VECTOR, isr14, cs);

    /* 4. Construct the IDTR. */
    struct idtr idtr;

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)idt;

    /* 5. Load the completed IDT. */
    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idtr)
        : "memory"
    );

    /* 6. Report initialization. */
    serial_write("PrOS: IDT loaded\n");
    serial_write("PrOS: Exception gates installed: 0, 6, 8, 13, 14\n");
}
