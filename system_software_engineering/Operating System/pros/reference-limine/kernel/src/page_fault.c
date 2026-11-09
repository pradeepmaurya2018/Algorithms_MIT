#include <stdint.h>
#include "serial.h"
#include "panic.h"
/* Read the faulting virtual address from CR2. */
static uint64_t read_cr2(void)
{
    uint64_t address;

    __asm__ volatile (
        "mov %%cr2, %0"
        : "=r"(address)
    );

    return address;
}

// /* Halt permanently after a fatal page fault. */
// static __attribute__((noreturn))
// void page_fault_halt(void)
// {
//     for (;;) {
//         __asm__ volatile ("cli; hlt");
//     }
// }

void page_fault_handler(uint64_t error_code)
{
    uint64_t fault_address = read_cr2();

    serial_write("\n");
    serial_write("========================================\n");
    serial_write(" PrOS PAGE FAULT (#PF)\n");
    serial_write("========================================\n");

    serial_write("Faulting virtual address (CR2): ");

    serial_write_hex(fault_address);

    serial_write("\n");

    serial_write("Error code: ");
    serial_write_hex(error_code);
    serial_write("\n");

    serial_write("Fault type: ");

    if (error_code & (1ULL << 0)) {
        serial_write("Protection violation\n");
    } else {
        serial_write("Non-present page\n");
    }

    if (error_code & (1ULL << 1)) {
        serial_write("Access: Write\n");
    } else {
        serial_write("Access: Read\n");
    }

    if (error_code & (1ULL << 2)) {
        serial_write("Privilege: User mode\n");
    } else {
        serial_write("Privilege: Supervisor mode\n");
    }

    if (error_code & (1ULL << 3)) {
        serial_write("Reserved page-table bit violation\n");
    }

    if (error_code & (1ULL << 4)) {
        serial_write("Instruction fetch fault\n");
    }

    kernel_panic("Unhandled page fault");
}