#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>

/* x86-64 page size */
#define VMM_PAGE_SIZE 4096ULL

/* Page-table entry flags */
#define VMM_PRESENT  (1ULL << 0)
#define VMM_WRITABLE (1ULL << 1)
#define VMM_USER     (1ULL << 2)
#define VMM_NO_EXEC  (1ULL << 63)

/*
 * Initialize VMM using the currently active CR3
 * and Limine's HHDM offset.
 */
void vmm_init(uint64_t hhdm_offset);

/*
 * Map one 4 KiB virtual page to a physical page.
 *
 * Both addresses must be page-aligned.
 * flags contains permission bits such as VMM_WRITABLE.
 *
 * Returns true on success, false on failure.
 */
bool vmm_map_page(
    uint64_t virtual_address,
    uint64_t physical_address,
    uint64_t flags
);

#endif