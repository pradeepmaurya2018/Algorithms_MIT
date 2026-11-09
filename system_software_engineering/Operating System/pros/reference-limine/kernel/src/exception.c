#include <stdint.h>

#include "panic.h"

void exception_dispatch(
    uint64_t vector,
    uint64_t error_code,
    uint64_t rip
)
{
    kernel_panic_exception(vector, error_code, rip);
}
