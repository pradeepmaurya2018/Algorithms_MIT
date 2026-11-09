#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256
#define PAGE_FAULT_VECTOR 14

void idt_init(void);

#endif