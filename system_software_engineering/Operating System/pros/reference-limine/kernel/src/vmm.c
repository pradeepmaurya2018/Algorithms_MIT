#include "vmm.h"
#include "pmm.h"

static uint64_t *pml4;
static uint64_t hhdm_offset;

/*
 * Physical address mask for standard 4 KiB
 * page-table entries.
 */
#define VMM_PHYS_ADDR_MASK 0x000FFFFFFFFFF000ULL

#define VMM_PAGE_PRESENT  (1ULL << 0)
#define VMM_PAGE_WRITABLE (1ULL << 1)
#define VMM_PAGE_USER     (1ULL << 2)

/*
 * IMPORTANT:
 *
 * On x86-64, bit 7 of a PDE/PDPT entry is
 * the Page Size (PS) bit.
 *
 * It is NOT the same thing as the 4 KiB
 * page size constant VMM_PAGE_SIZE.
 */
#define VMM_PAGE_LARGE   (1ULL << 7)


/*
 * Convert a physical address to its HHDM
 * virtual address.
 */
static inline uint64_t *phys_to_virt(uint64_t phys)
{
    return (uint64_t *)(phys + hhdm_offset);
}


/*
 * Read the currently active CR3.
 */
static inline uint64_t read_cr3(void)
{
    uint64_t value;

    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(value)
    );

    return value;
}


/*
 * Allocate one page for a page table and
 * clear all 512 entries.
 */
static uint64_t alloc_table(void)
{
    uint64_t phys = pmm_alloc_page();

    if (phys == 0) {
        return 0;
    }

    uint64_t *table = phys_to_virt(phys);

    for (uint64_t i = 0; i < 512; i++) {
        table[i] = 0;
    }

    return phys;
}


/*
 * Initialize VMM using Limine's active page tables.
 */
void vmm_init(uint64_t offset)
{
    hhdm_offset = offset;

    uint64_t cr3 = read_cr3();

    uint64_t pml4_phys =
        cr3 & VMM_PHYS_ADDR_MASK;

    pml4 = phys_to_virt(pml4_phys);
}


/*
 * Get an existing next-level page table,
 * or allocate a new one.
 */
static uint64_t *get_next_table(
    uint64_t *parent,
    uint64_t index
)
{
    uint64_t entry = parent[index];

    /*
     * Existing entry.
     */
    if (entry & VMM_PAGE_PRESENT) {

        /*
         * At the PDPT and PD levels, bit 7
         * indicates a large page.
         *
         * We only support 4 KiB pages here.
         */
        if (entry & VMM_PAGE_LARGE) {
            return 0;
        }

        uint64_t next_phys =
            entry & VMM_PHYS_ADDR_MASK;

        return phys_to_virt(next_phys);
    }


    /*
     * No table exists.
     *
     * Allocate a new page-table page.
     */
    uint64_t new_phys = alloc_table();

    if (new_phys == 0) {
        return 0;
    }

    /*
     * Page-table entries are:
     *
     *     physical address
     *     +
     *     PRESENT
     *     +
     *     WRITABLE
     */
    parent[index] =
        new_phys |
        VMM_PAGE_PRESENT |
        VMM_PAGE_WRITABLE;

    return phys_to_virt(new_phys);
}


/*
 * Map one 4 KiB virtual page to one
 * 4 KiB physical page.
 */
bool vmm_map_page(
    uint64_t virtual_address,
    uint64_t physical_address,
    uint64_t flags
)
{
    if (pml4 == 0) {
        return false;
    }


    /*
     * Both addresses must be 4 KiB aligned.
     */
    if ((virtual_address & (VMM_PAGE_SIZE - 1)) ||
        (physical_address & (VMM_PAGE_SIZE - 1))) {
        return false;
    }


    /*
     * Check that the virtual address is
     * canonical in 48-bit x86-64 mode.
     */
    uint64_t upper =
        virtual_address >> 48;

    uint64_t bit47 =
        (virtual_address >> 47) & 1;

    if ((!bit47 && upper != 0) ||
        (bit47 && upper != 0xFFFF)) {
        return false;
    }


    /*
     * Extract the four page-table indexes.
     *
     * 47........39   PML4
     * 38........30   PDPT
     * 29........21   PD
     * 20........12   PT
     * 11.........0   offset
     */
    uint64_t pml4_index =
        (virtual_address >> 39) & 0x1FF;

    uint64_t pdpt_index =
        (virtual_address >> 30) & 0x1FF;

    uint64_t pd_index =
        (virtual_address >> 21) & 0x1FF;

    uint64_t pt_index =
        (virtual_address >> 12) & 0x1FF;


    /*
     * PML4 -> PDPT
     */
    uint64_t *pdpt =
        get_next_table(
            pml4,
            pml4_index
        );

    if (pdpt == 0) {
        return false;
    }


    /*
     * PDPT -> Page Directory
     */
    uint64_t *pd =
        get_next_table(
            pdpt,
            pdpt_index
        );

    if (pd == 0) {
        return false;
    }


    /*
     * Page Directory -> Page Table
     */
    uint64_t *pt =
        get_next_table(
            pd,
            pd_index
        );

    if (pt == 0) {
        return false;
    }


    /*
     * Do not overwrite an existing mapping.
     */
    if (pt[pt_index] & VMM_PAGE_PRESENT) {
        return false;
    }


    /*
     * Construct the final PTE.
     *
     * Supported permission flags:
     *
     *     WRITABLE
     *     USER
     *     NO_EXEC
     */
    uint64_t entry_flags =
        flags &
        (
            VMM_PAGE_WRITABLE |
            VMM_PAGE_USER |
            VMM_NO_EXEC
        );


    pt[pt_index] =
        (physical_address & VMM_PHYS_ADDR_MASK) |
        VMM_PAGE_PRESENT |
        entry_flags;


    /*
     * Flush this virtual address from
     * the TLB.
     */
    __asm__ volatile (
        "invlpg (%0)"
        :
        : "r"(virtual_address)
        : "memory"
    );

    return true;
}
