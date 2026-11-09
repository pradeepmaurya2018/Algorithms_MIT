#include <stdint.h>

#include "kernel_stack.h"
#include "heap.h"
#include "serial.h"

#define KERNEL_BOOTSTRAP_STACK_SIZE 16384ULL

extern void kernel_stack_switch_and_call(
    uint64_t stack_top,
    void (*continuation)(void)
);

static void kernel_stack_continuation(void);

void kernel_stack_init(void)
{
    serial_write(
        "PrOS: Allocating PrOS-owned kernel stack\n"
    );

    void *stack =
        kmalloc(KERNEL_BOOTSTRAP_STACK_SIZE);

    if (stack == 0) {
        serial_write(
            "PrOS: ERROR: Kernel stack allocation FAILED\n"
        );

        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    uint64_t stack_top =
        (uint64_t)stack +
        KERNEL_BOOTSTRAP_STACK_SIZE;

    serial_write(
        "PrOS: PrOS-owned kernel stack allocated\n"
    );

    serial_write(
        "PrOS: Switching to PrOS-owned kernel stack\n"
    );

    kernel_stack_switch_and_call(
        stack_top,
        kernel_stack_continuation
    );

    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}

/*
 * This function executes on the new PrOS-owned stack.
 */
static void kernel_stack_continuation(void)
{
    serial_write(
        "PrOS: Now executing on PrOS-owned kernel stack\n"
    );

    /*
     * We deliberately do not return from here.
     *
     * The old stack is no longer used and can safely be
     * reclaimed by the PMM.
     */
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
