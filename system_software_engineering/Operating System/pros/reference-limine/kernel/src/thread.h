#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stdbool.h>

#define THREAD_STACK_SIZE 4096ULL

enum thread_state {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_TERMINATED
};

struct thread {
    uint64_t id;

    /*
     * Saved stack pointer.
     *
     * This points to the saved CPU context on the
     * thread's kernel stack.
     */
    uint64_t rsp;

    void *stack;
    uint64_t stack_size;

    void (*entry)(void);

    enum thread_state state;
};

bool thread_init(void);

struct thread *thread_create(void (*entry)(void));

void thread_start(struct thread *thread);

void thread_yield(void);

void thread_exit(void);

void scheduler(void);

void scheduler_set_threads(
    struct thread *a,
    struct thread *b
);

#endif
