#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

bool heap_init(void);

void *kmalloc(size_t size);
void kfree(void *ptr);

uint64_t heap_get_used_bytes(void);
uint64_t heap_get_total_bytes(void);

#endif
