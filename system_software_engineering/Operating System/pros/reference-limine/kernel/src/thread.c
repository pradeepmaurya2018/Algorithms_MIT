#include <stdint.h>
#include <stdbool.h>

#include "thread.h"
#include "heap.h"
#include "serial.h"

static uint64_t next_thread_id = 1;

extern void context_switch(uint64_t *old_rsp, uint64_t new_rsp);
extern void thread_bootstrap(void);

static struct thread *current_thread = 0;
static struct thread *thread_a = 0;
static struct thread *thread_b = 0;

bool thread_init(void)
{
    next_thread_id = 1;

    current_thread = 0;
    thread_a = 0;
    thread_b = 0;

    serial_write("PrOS: Thread subsystem initialized\n");

    return true;
}

struct thread *thread_create(void (*entry)(void))
{
    serial_write("PrOS: thread_create: ENTER\n");

    uint64_t debug_rsp_enter;
    uint64_t debug_stack0_enter;
    uint64_t debug_stack1_enter;
    uint64_t debug_stack2_enter;

    __asm__ volatile ("mov %%rsp, %0" : "=r"(debug_rsp_enter));

    // debug_stack0_enter = ((uint64_t *)debug_rsp_enter)[0];
    // debug_stack1_enter = ((uint64_t *)debug_rsp_enter)[1];
    // debug_stack2_enter = ((uint64_t *)debug_rsp_enter)[2];
    //
    // serial_write("PrOS: thread_create ENTER RSP: ");
    // serial_write_hex(debug_rsp_enter);
    // serial_write("\n");
    //
    // serial_write("PrOS: thread_create ENTER [RSP]: ");
    // serial_write_hex(debug_stack0_enter);
    // serial_write("\n");
    //
    // serial_write("PrOS: thread_create ENTER [RSP+8]: ");
    // serial_write_hex(debug_stack1_enter);
    // serial_write("\n");
    //
    // serial_write("PrOS: thread_create ENTER [RSP+16]: ");
    // serial_write_hex(debug_stack2_enter);
    serial_write("\n");

    if (entry == 0) {
        serial_write("PrOS: thread_create: invalid entry\n");
        return 0;
    }

    /*
     * Allocate thread control block.
     */
    serial_write(
        "PrOS: thread_create: allocating TCB\n"
    );

    struct thread *thread =
        kmalloc(sizeof(struct thread));

    if (thread == 0) {
        serial_write(
            "PrOS: thread_create: TCB allocation FAILED\n"
        );
        return 0;
    }

    serial_write(
        "PrOS: thread_create: TCB allocated\n"
    );

    /*
     * Initialize TCB immediately.
     */
    thread->id = next_thread_id++;
    thread->rsp = 0;
    thread->stack = 0;
    thread->stack_size = 0;
    thread->entry = entry;
    thread->state = THREAD_READY;

    /*
     * Allocate private kernel stack.
     */
    serial_write(
        "PrOS: thread_create: allocating stack\n"
    );

    void *stack =
        kmalloc(THREAD_STACK_SIZE);

    if (stack == 0) {
        serial_write(
            "PrOS: thread_create: stack allocation FAILED\n"
        );

        kfree(thread);
        return 0;
    }

    serial_write(
        "PrOS: thread_create: stack allocated\n"
    );

    thread->stack = stack;
    thread->stack_size = THREAD_STACK_SIZE;

    /*
     * Build initial stack context.
     */
    serial_write(
        "PrOS: thread_create: building initial context\n"
    );

    uint64_t *sp =
        (uint64_t *)(
            (uint64_t)stack +
            THREAD_STACK_SIZE
        );

    sp = (uint64_t *)(
        (uint64_t)sp & ~0xFULL
    );

    /*
     * Context expected by context_switch():
     *
     *     r15
     *     r14
     *     r13
     *     r12 = entry
     *     rbx
     *     rbp
     *     RIP = thread_bootstrap
     */
    *(--sp) = (uint64_t)thread_bootstrap;  /* RIP */
    *(--sp) = 0;                           /* rbp */
    *(--sp) = 0;                           /* rbx */
    *(--sp) = (uint64_t)entry;             /* r12 */
    *(--sp) = 0;                           /* r13 */
    *(--sp) = 0;                           /* r14 */
    *(--sp) = 0;                           /* r15 */

    thread->rsp = (uint64_t)sp;

    serial_write(
        "PrOS: thread_create: initial context built\n"
    );

    serial_write("PrOS: thread rsp = ");
    serial_write_hex(thread->rsp);
    serial_write("\n");

    serial_write("PrOS: saved r15 = ");
    serial_write_hex(((uint64_t *)thread->rsp)[0]);
    serial_write("\n");

    serial_write("PrOS: saved r14 = ");
    serial_write_hex(((uint64_t *)thread->rsp)[1]);
    serial_write("\n");

    serial_write("PrOS: saved r13 = ");
    serial_write_hex(((uint64_t *)thread->rsp)[2]);
    serial_write("\n");

    serial_write("PrOS: saved r12 = ");
    serial_write_hex(((uint64_t *)thread->rsp)[3]);
    serial_write("\n");

    serial_write("PrOS: saved rbx = ");
    serial_write_hex(((uint64_t *)thread->rsp)[4]);
    serial_write("\n");

    serial_write("PrOS: saved rbp = ");
    serial_write_hex(((uint64_t *)thread->rsp)[5]);
    serial_write("\n");

    serial_write("PrOS: saved RIP = ");
    serial_write_hex(((uint64_t *)thread->rsp)[6]);
    serial_write("\n");

    /*
     * Verify that the TCB is still readable.
     */
    if (thread->entry != entry ||
        thread->stack != stack ||
        thread->stack_size != THREAD_STACK_SIZE ||
        thread->state != THREAD_READY) {

        serial_write(
            "PrOS: thread_create: TCB CORRUPTED\n"
        );

        return 0;
    }

    serial_write(
        "PrOS: thread_create: TCB verified\n"
    );

    // void *debug_return_address =
    //     __builtin_return_address(0);
    //
    // serial_write("PrOS: thread_create return address: ");
    // serial_write_hex((uint64_t)debug_return_address);
    // serial_write("\n");

    serial_write(
        "PrOS: thread_create: RETURNING\n"
    );

    return thread;
}

void thread_start(struct thread *thread)
{
    if (thread == 0) {
        serial_write(
            "PrOS: thread_start: NULL thread\n"
        );
        return;
    }

    current_thread = thread;
    thread->state = THREAD_RUNNING;

    serial_write(
        "PrOS: Starting kernel thread\n"
    );

    uint64_t old_rsp;

    context_switch(
        &old_rsp,
        thread->rsp
    );
}

void scheduler(void)
{
    struct thread *old = current_thread;
    struct thread *next = 0;

    if (old == 0) {
        return;
    }

    /*
     * Select the other thread only if it is READY.
     */
    if (old == thread_a) {
        if (thread_b != 0 &&
            thread_b->state == THREAD_READY) {
            next = thread_b;
        }
    } else if (old == thread_b) {
        if (thread_a != 0 &&
            thread_a->state == THREAD_READY) {
            next = thread_a;
        }
    }

    /*
     * No runnable thread exists.
     */
    if (next == 0) {
        return;
    }

    current_thread = next;

    /*
     * A normal yield moves RUNNING -> READY.
     *
     * A terminating thread remains TERMINATED.
     */
    if (old->state == THREAD_RUNNING) {
        old->state = THREAD_READY;
    }

    next->state = THREAD_RUNNING;

    context_switch(
        &old->rsp,
        next->rsp
    );
}

void thread_yield(void)
{
    scheduler();
}

void scheduler_set_threads(
    struct thread *a,
    struct thread *b
)
{
    thread_a = a;
    thread_b = b;
    current_thread = a;

    serial_write(
        "PrOS: scheduler_set_threads: configured\n"
    );
}
void thread_exit(void)
{
    if (current_thread == 0) {
        for (;;) {
            __asm__ volatile ("cli");
            __asm__ volatile ("hlt");
        }
    }

    current_thread->state = THREAD_TERMINATED;

    scheduler();

    /*
     * A terminated thread must never continue executing.
     */
    for (;;) {
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }
}