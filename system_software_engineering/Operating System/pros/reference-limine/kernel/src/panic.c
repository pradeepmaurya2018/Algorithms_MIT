#include <stdint.h>

#include "panic.h"
#include "serial.h"

static __attribute__((noreturn))
void panic_halt(void)
{
    __asm__ volatile ("cli");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

__attribute__((noreturn))
void kernel_panic(const char *message)
{
    serial_write("\n");
    serial_write("========================================\n");
    serial_write("             PrOS KERNEL PANIC\n");
    serial_write("========================================\n");

    serial_write("Reason: ");

    if (message != 0) {
        serial_write(message);
    } else {
        serial_write("Unknown");
    }

    serial_write("\n");
    serial_write("System halted.\n");

    panic_halt();
}

__attribute__((noreturn))
void kernel_panic_exception(
    uint64_t vector,
    uint64_t error_code,
    uint64_t rip
)
{
    serial_write("\n");
    serial_write("========================================\n");
    serial_write("         PrOS EXCEPTION PANIC\n");
    serial_write("========================================\n");

    serial_write("Exception vector: ");
    serial_write_hex(vector);
    serial_write("\n");

    serial_write("Error code: ");
    serial_write_hex(error_code);
    serial_write("\n");

    serial_write("Instruction pointer (RIP): ");
    serial_write_hex(rip);
    serial_write("\n");

    serial_write("System halted.\n");

    panic_halt();
}
