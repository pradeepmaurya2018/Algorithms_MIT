#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include "serial.h"

#define HEAP_START       0xFFFFA10000000000ULL
#define HEAP_PAGE_SIZE   4096ULL
#define HEAP_ALIGNMENT   16ULL

#define HEAP_FLAGS (VMM_WRITABLE | VMM_NO_EXEC)

struct heap_block {
    size_t size;
    bool free;
    struct heap_block *next;
    struct heap_block *prev;
};

#define BLOCK_HEADER_SIZE \
    ((sizeof(struct heap_block) + HEAP_ALIGNMENT - 1) \
      & ~(HEAP_ALIGNMENT - 1))

static struct heap_block *heap_head = NULL;
static struct heap_block *heap_tail = NULL;

static uint64_t heap_total_bytes = 0;
static uint64_t heap_used_bytes = 0;
static bool heap_ready = false;

static size_t align_up(size_t value)
{
    return (value + HEAP_ALIGNMENT - 1)
           & ~(HEAP_ALIGNMENT - 1);
}

static bool heap_grow(void)
{
    uint64_t virt = HEAP_START + heap_total_bytes;

    serial_write("PrOS: heap_grow: allocating physical page\n");

    uint64_t phys = pmm_alloc_page();

    if (phys == 0) {
        serial_write(
            "PrOS: heap_grow: PMM allocation FAILED\n"
        );
        return false;
    }

    serial_write("PrOS: heap_grow: mapping virtual page\n");

    if (!vmm_map_page(virt, phys, HEAP_FLAGS)) {
        serial_write(
            "PrOS: heap_grow: VMM mapping FAILED\n"
        );

        pmm_free_page(phys);
        return false;
    }

    heap_total_bytes += HEAP_PAGE_SIZE;

    serial_write("PrOS: heap_grow: page mapped\n");

    /*
     * If the last block is free, extend it.
     */
    if (heap_tail != NULL && heap_tail->free) {
        heap_tail->size += HEAP_PAGE_SIZE;

        serial_write(
            "PrOS: heap_grow: extended free tail\n"
        );

        return true;
    }

    /*
     * Otherwise create a new free block.
     */
    struct heap_block *block =
        (struct heap_block *)virt;

    block->size = HEAP_PAGE_SIZE;
    block->free = true;
    block->next = NULL;
    block->prev = heap_tail;

    if (heap_tail != NULL) {
        heap_tail->next = block;
    } else {
        heap_head = block;
    }

    heap_tail = block;

    serial_write(
        "PrOS: heap_grow: created new free block\n"
    );

    return true;
}

bool heap_init(void)
{
    if (heap_ready) {
        return true;
    }

    heap_head = NULL;
    heap_tail = NULL;
    heap_total_bytes = 0;
    heap_used_bytes = 0;

    if (!heap_grow()) {
        return false;
    }

    heap_ready = true;

    return true;
}

void *kmalloc(size_t size)
{
    if (!heap_ready || size == 0) {
        return NULL;
    }

    size = align_up(size);

    if (size > SIZE_MAX - BLOCK_HEADER_SIZE) {
        return NULL;
    }

    size_t required = size + BLOCK_HEADER_SIZE;

    for (;;) {
        struct heap_block *block = heap_head;

        /*
         * First-fit search.
         */
        while (block != NULL) {
            if (block->free && block->size >= required) {
                break;
            }

            block = block->next;
        }

        /*
         * No suitable block.
         */
        if (block == NULL) {
            if (!heap_grow()) {
                return NULL;
            }

            continue;
        }

        size_t remaining = block->size - required;

        /*
         * Split if enough room remains.
         */
        if (remaining >=
            BLOCK_HEADER_SIZE + HEAP_ALIGNMENT) {

            struct heap_block *new_block =
                (struct heap_block *)
                ((uintptr_t)block + required);

            new_block->size = remaining;
            new_block->free = true;
            new_block->prev = block;
            new_block->next = block->next;

            if (new_block->next != NULL) {
                new_block->next->prev = new_block;
            } else {
                heap_tail = new_block;
            }

            block->next = new_block;
            block->size = required;
        }

        block->free = false;

        heap_used_bytes += block->size;

        return (void *)(
            (uintptr_t)block + BLOCK_HEADER_SIZE
        );
    }
}

void kfree(void *ptr)
{
    if (ptr == NULL || !heap_ready) {
        return;
    }

    struct heap_block *block =
        (struct heap_block *)
        ((uintptr_t)ptr - BLOCK_HEADER_SIZE);

    if (block->free) {
        return;
    }

    block->free = true;

    heap_used_bytes -= block->size;

    /*
     * Merge with next block.
     */
    if (block->next != NULL &&
        block->next->free) {

        struct heap_block *next =
            block->next;

        block->size += next->size;
        block->next = next->next;

        if (block->next != NULL) {
            block->next->prev = block;
        } else {
            heap_tail = block;
        }
    }

    /*
     * Merge with previous block.
     */
    if (block->prev != NULL &&
        block->prev->free) {

        struct heap_block *prev =
            block->prev;

        prev->size += block->size;
        prev->next = block->next;

        if (prev->next != NULL) {
            prev->next->prev = prev;
        } else {
            heap_tail = prev;
        }
    }
}

uint64_t heap_get_used_bytes(void)
{
    return heap_used_bytes;
}

uint64_t heap_get_total_bytes(void)
{
    return heap_total_bytes;
}
