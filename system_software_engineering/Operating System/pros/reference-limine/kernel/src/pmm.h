#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>
#include <limine.h>

#define PAGE_SIZE 4096ULL

bool pmm_init(struct limine_memmap_response *response);
uint64_t pmm_reclaim_bootloader_memory(
    struct limine_memmap_response *response
);
uint64_t pmm_alloc_page(void);
bool pmm_free_page(uint64_t physical_address);

uint64_t pmm_get_free_pages(void);
uint64_t pmm_get_total_pages(void);

#endif