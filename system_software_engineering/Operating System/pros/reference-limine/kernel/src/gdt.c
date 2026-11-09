#include <stdint.h>
#include "gdt.h"
#include "serial.h"
#include "tss.h"
struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

/*
 * GDT layout:
 *
 *   0 = Null
 *   1 = Kernel 64-bit code
 *   2 = Kernel data
 *   3 = User 64-bit code
 *   4 = User data
 *
 * TSS entries will be added later.
 */

uint64_t gdt[7] __attribute__((aligned(8)));
extern struct tss64 tss;
extern void gdt_load(void);


static void gdt_set_tss(uint64_t base, uint32_t limit)
{
    uint64_t low = 0;
    uint64_t high = 0;

    low |= (limit & 0xFFFFULL);
    low |= (base & 0xFFFFFFULL) << 16;

    /*
     * Type = 0x9
     * Present = 1
     */
    low |= 0x89ULL << 40;

    low |= ((uint64_t)(limit >> 16) & 0xF) << 48;
    low |= ((uint64_t)(base >> 24) & 0xFF) << 56;

    high = base >> 32;

    gdt[5] = low;
    gdt[6] = high;
}
static void gdt_build(void)
{
    /*
     * Null descriptor.
     */
    gdt[0] = 0x0000000000000000ULL;

    /*
     * Kernel 64-bit code:
     *
     * Access = 0x9A
     * G = 1
     * L = 1
     *
     * D/B = 0
     */
    gdt[1] = 0x00AF9A000000FFFFULL;

    /*
     * Kernel data:
     *
     * Access = 0x92
     * G = 1
     * D/B = 1
     */
    gdt[2] = 0x00CF92000000FFFFULL;

    /*
     * User 64-bit code:
     *
     * Access = 0xFA
     * G = 1
     * L = 1
     */
    gdt[3] = 0x00AFFA000000FFFFULL;

    /*
     * User data:
     *
     * Access = 0xF2
     * G = 1
     * D/B = 1
     */
    gdt[4] = 0x00CFF2000000FFFFULL;
    gdt_set_tss((uint64_t)&tss, sizeof(tss) - 1);
    serial_write("PrOS: TSS GDT low: ");
    serial_write_hex(gdt[5]);
    serial_write("\n");

    serial_write("PrOS: TSS GDT high: ");
    serial_write_hex(gdt[6]);
    serial_write("\n");
}


void gdt_init(void)
{
    gdt_build();

    serial_write("PrOS: Installing PrOS GDT\n");

    gdt_load();

    serial_write("PrOS: PrOS GDT loaded\n");

    uint16_t cs;
    uint16_t ds;

    __asm__ volatile (
        "mov %%cs, %0\n"
        "mov %%ds, %1"
        : "=r"(cs), "=r"(ds)
    );

    serial_write("PrOS: New CS selector: ");
    serial_write_hex(cs);
    serial_write("\n");

    serial_write("PrOS: New DS selector: ");
    serial_write_hex(ds);
    serial_write("\n");
}

