#include "tss.h"
#include "serial.h"

struct tss64 tss __attribute__((aligned(16)));

extern void tss_load(void);

void tss_init(void)
{
    tss = (struct tss64){0};

    /*
     * Place the I/O bitmap beyond the TSS.
     */
    tss.iomap_base = sizeof(tss);

    serial_write("PrOS: Initializing TSS\n");

    serial_write("PrOS: TSS address: ");
    serial_write_hex((uint64_t)&tss);
    serial_write("\n");

    serial_write("PrOS: TSS size: ");
    serial_write_hex(sizeof(tss));
    serial_write("\n");

    serial_write("PrOS: TSS first qword: ");
    serial_write_hex(*(uint64_t *)&tss);
    serial_write("\n");

    serial_write("PrOS: TSS IOMAP base: ");
    serial_write_hex(tss.iomap_base);
    serial_write("\n");

    tss_load();
    serial_write("PrOS: TSS loaded\n");
}
