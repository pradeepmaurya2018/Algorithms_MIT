#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>

__attribute__((noreturn))
void kernel_panic(const char *message);

__attribute__((noreturn))
void kernel_panic_exception(
    uint64_t vector,
    uint64_t error_code,
    uint64_t rip
);

#endif
