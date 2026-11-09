#include "pmm.h"
#include <limine.h>
#include <stddef.h>
/*
 * Initial PMM implementation.
 *
 * Supports physical addresses below 4 GiB.
 * Page size: 4096 bytes.
 */

#define MAX_PHYS_ADDR (4ULL * 1024 * 1024 * 1024)
#define MAX_PAGES     (MAX_PHYS_ADDR / PAGE_SIZE)
#define BITMAP_SIZE   ((MAX_PAGES + 7) / 8)
#define RESERVED_LOW_MEMORY (1ULL * 1024 * 1024)
/*
 * Two bitmaps:
 *
 * usable_bitmap:
 *   1 = page belongs to usable RAM
 *
 * allocated_bitmap:
 *   1 = page is allocated or unavailable
 *
 * Both are stored in kernel BSS.
 */
static uint8_t usable_bitmap[BITMAP_SIZE];
static uint8_t allocated_bitmap[BITMAP_SIZE];
static uint8_t reserved_bitmap[BITMAP_SIZE];

static uint64_t total_pages;
static uint64_t free_pages;


/* Set a bit in a bitmap. */
static void bitmap_set(uint8_t *bitmap, uint64_t index)
{
    bitmap[index / 8] |= (uint8_t)(1U << (index % 8));
}

/* Clear a bit in a bitmap. */
static void bitmap_clear(uint8_t *bitmap, uint64_t index)
{
    bitmap[index / 8] &= (uint8_t)~(1U << (index % 8));
}

/* Read a bit from a bitmap. */
static bool bitmap_test(const uint8_t *bitmap, uint64_t index)
{
    return (bitmap[index / 8] & (1U << (index % 8))) != 0;
}


/*
 * Initialize the physical memory manager from the Limine map.
 *
 * Call only after Limine has supplied the memory map.
 */
bool pmm_init(struct limine_memmap_response *response)
{
    // struct limine_memmap_response *response =
    //     memmap_request.response;

    /*
     * Initially mark every page unavailable.
     * Only explicitly usable pages will be enabled.
     */
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        usable_bitmap[i] = 0;
        allocated_bitmap[i] = 0xFF;
        reserved_bitmap[i] = 0;
    }

    total_pages = 0;
    free_pages = 0;

    if (response == NULL) {
        return false;
    }

    for (uint64_t i = 0; i < response->entry_count; i++) {

        struct limine_memmap_entry *entry =
            response->entries[i];

        if (entry->type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        uint64_t start = entry->base;
        uint64_t end = entry->base + entry->length;

        /* Ignore regions outside our initial 4 GiB limit. */
        if (start >= MAX_PHYS_ADDR) {
            continue;
        }

        if (end > MAX_PHYS_ADDR || end < start) {
            end = MAX_PHYS_ADDR;
        }

        /*
         * Only manage fully contained pages.
         * Align start upward and end downward.
         */
        uint64_t first_page =
            (start + PAGE_SIZE - 1) / PAGE_SIZE;

        uint64_t last_page = end / PAGE_SIZE;

        for (uint64_t page = first_page;
             page < last_page;
             page++) {

            if (page >= MAX_PAGES) {
                break;
            }

            bitmap_set(usable_bitmap, page);
            bitmap_clear(allocated_bitmap, page);

            total_pages++;
            free_pages++;
        }
    }


    /*
     * Reserve the first 1 MiB of physical memory.
     */
    for (uint64_t page = 0;
     page < RESERVED_LOW_MEMORY / PAGE_SIZE;
     page++) {

        if (bitmap_test(usable_bitmap, page) &&
            !bitmap_test(allocated_bitmap, page)) {

            bitmap_set(allocated_bitmap, page);
            bitmap_set(reserved_bitmap, page);
            free_pages--;
            }
     }
    return true;
}


/*
 * Allocate one 4 KiB physical page.
 *
 * Returns:
 *   Physical address of allocated page
 *   0 if no page is available
 */
uint64_t pmm_alloc_page(void)
{
    for (uint64_t page = 1; page < MAX_PAGES; page++) {

        if (!bitmap_test(usable_bitmap, page)) {
            continue;
        }

        if (bitmap_test(allocated_bitmap, page)) {
            continue;
        }

        bitmap_set(allocated_bitmap, page);
        free_pages--;

        return page * PAGE_SIZE;
    }

    return 0;
}


/*
 * Release a previously allocated page.
 *
 * Returns false for invalid or already-free pages.
 */
bool pmm_free_page(uint64_t physical_address)
{
    if (physical_address == 0 ||
        physical_address >= MAX_PHYS_ADDR ||
        physical_address % PAGE_SIZE != 0) {

        return false;
    }

    uint64_t page = physical_address / PAGE_SIZE;

    if (!bitmap_test(usable_bitmap, page)) {
        return false;
    }

    if (!bitmap_test(allocated_bitmap, page)) {
        return false;
    }
    if (bitmap_test(reserved_bitmap, page)) {
        return false;
    }

    bitmap_clear(allocated_bitmap, page);
    free_pages++;

    return true;
}


uint64_t pmm_get_free_pages(void)
{
    return free_pages;
}


uint64_t pmm_get_total_pages(void)
{
    return total_pages;
}
/*
 * Reclaim bootloader-reclaimable physical memory.
 *
 * Call only after the kernel has finished using all Limine
 * bootloader-provided data that may reside in these regions.
 *
 * Returns the number of pages reclaimed.
 */
uint64_t pmm_reclaim_bootloader_memory(
    struct limine_memmap_response *response)
{
    if (response == NULL) {
        return 0;
    }

    uint64_t reclaimed_pages = 0;

    for (uint64_t i = 0; i < response->entry_count; i++) {

        struct limine_memmap_entry *entry =
            response->entries[i];

        if (entry->type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            continue;
        }

        uint64_t start = entry->base;
        uint64_t end = entry->base + entry->length;

        /* Ignore regions outside our 4 GiB PMM limit. */
        if (start >= MAX_PHYS_ADDR) {
            continue;
        }

        /* Handle overflow and clip the region to 4 GiB. */
        if (end < start || end > MAX_PHYS_ADDR) {
            end = MAX_PHYS_ADDR;
        }

        /* Manage only complete pages. */
        uint64_t first_page =
            (start + PAGE_SIZE - 1) / PAGE_SIZE;

        uint64_t last_page = end / PAGE_SIZE;

        for (uint64_t page = first_page;
             page < last_page;
             page++) {

            if (page >= MAX_PAGES) {
                break;
            }
            /* Never reclaim the first 1 MiB of physical memory. */
            if (page * PAGE_SIZE < RESERVED_LOW_MEMORY) {
                continue;
            }

            /* Never reclaim a permanently reserved page. */
            if (bitmap_test(reserved_bitmap, page)) {
                continue;
            }

            /*
             * Do not count a page twice if it is already
             * managed by the PMM.
             */
            if (bitmap_test(usable_bitmap, page)) {
                continue;
            }

            bitmap_set(usable_bitmap, page);
            bitmap_clear(allocated_bitmap, page);

            total_pages++;
            free_pages++;
            reclaimed_pages++;
             }
    }

    return reclaimed_pages;
}