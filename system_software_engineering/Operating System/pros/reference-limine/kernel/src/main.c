
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "serial.h"
#include "pmm.h"
#include "vmm.h"
#include "idt.h"
#include "heap.h"
#include "gdt.h"
#include "tss.h"
#include "thread.h"
/* ---------------------------------------------------------
 * Limine protocol requests
 * --------------------------------------------------------- */

/* Request Limine base revision 6. */
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] =
    LIMINE_BASE_REVISION(6);

/* Framebuffer request. */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

/* Physical memory map request. */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

/* Request the higher-half direct map. */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

/* Executable physical/virtual address request. */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_executable_address_request
    executable_address_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID,
    .revision = 0
};

/* Limine request section markers. */
__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] =
    LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;


/* ---------------------------------------------------------
 * Halt CPU
 * --------------------------------------------------------- */

static void hcf(void)
{
    for (;;) {
#if defined(__x86_64__)
        __asm__ volatile ("cli; hlt");
#elif defined(__aarch64__) || defined(__riscv)
        __asm__ volatile ("wfi");
#elif defined(__loongarch64)
        __asm__ volatile ("idle 0");
#endif
    }
}


/* ---------------------------------------------------------
 * Serial debugging helpers
 * --------------------------------------------------------- */

/* Print a 64-bit integer in hexadecimal over serial. */
void serial_write_hex(uint64_t value)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    serial_write("0x");

    for (int shift = 60; shift >= 0; shift -= 4) {
        char c = hex_digits[(value >> shift) & 0xF];

        char str[2] = { c, '\0' };

        serial_write(str);
    }
}

/* Read the current x86-64 page-table root from CR3. */
static uint64_t read_cr3(void)
{
    uint64_t value;

    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(value)
    );

    return value;
}

/* Print a decimal unsigned integer. */
static void serial_write_dec(uint64_t value)
{
    char buffer[21];
    size_t i = 0;

    if (value == 0) {
        serial_write("0");
        return;
    }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        char str[2] = { buffer[--i], '\0' };
        serial_write(str);
    }
}

/* Convert Limine memory map type to a readable name. */
static const char *memmap_type_name(uint64_t type)
{
    switch (type) {
        case LIMINE_MEMMAP_USABLE:
            return "USABLE";

        case LIMINE_MEMMAP_RESERVED:
            return "RESERVED";

        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return "ACPI_RECLAIMABLE";

        case LIMINE_MEMMAP_ACPI_NVS:
            return "ACPI_NVS";

        case LIMINE_MEMMAP_BAD_MEMORY:
            return "BAD_MEMORY";

        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return "BOOTLOADER_RECLAIMABLE";

        case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
            return "KERNEL_AND_MODULES";

        case LIMINE_MEMMAP_FRAMEBUFFER:
            return "FRAMEBUFFER";

        case LIMINE_MEMMAP_RESERVED_MAPPED:
            return "RESERVED_MAPPED";

        default:
            return "UNKNOWN";
    }
}


/* ---------------------------------------------------------
 * Physical memory map inspection
 * --------------------------------------------------------- */

static void print_memory_map(void)
{
    struct limine_memmap_response *response =
        memmap_request.response;

    if (response == NULL) {
        serial_write("PrOS: ERROR: Memory map unavailable\n");
        hcf();
    }

    serial_write("\n");
    serial_write("========================================\n");
    serial_write(" PrOS Physical Memory Map\n");
    serial_write("========================================\n");

    serial_write("Total entries: ");
    serial_write_dec(response->entry_count);
    serial_write("\n\n");

    uint64_t usable_bytes = 0;

    for (uint64_t i = 0; i < response->entry_count; i++) {

        struct limine_memmap_entry *entry =
            response->entries[i];

        serial_write("Region ");
        serial_write_dec(i);
        serial_write("\n");

        serial_write("  Base:   ");
        serial_write_hex(entry->base);
        serial_write("\n");

        serial_write("  Length: ");
        serial_write_hex(entry->length);
        serial_write("\n");

        serial_write("  Type:   ");
        serial_write(memmap_type_name(entry->type));
        serial_write(" (");
        serial_write_hex(entry->type);
        serial_write(")\n\n");

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            usable_bytes += entry->length;
        }
    }

    serial_write("----------------------------------------\n");
    serial_write("Total usable RAM: ");
    serial_write_dec(usable_bytes / (1024 * 1024));
    serial_write(" MiB\n");
    serial_write("========================================\n");
}


/* ---------------------------------------------------------
 * Framebuffer helpers
 * --------------------------------------------------------- */

/* Scale an 8-bit colour channel to the framebuffer mask. */
static uint32_t fb_channel(
    uint8_t value,
    uint8_t mask_size,
    uint8_t mask_shift)
{
    uint64_t max = ((uint64_t)1 << mask_size) - 1;

    return (uint32_t)(
        ((uint64_t)value * max / 255) << mask_shift
    );
}

/* Build an RGB pixel using the framebuffer's channel layout. */
static uint32_t fb_pixel(
    struct limine_framebuffer *fb,
    uint8_t red,
    uint8_t green,
    uint8_t blue)
{
    return fb_channel(
               red,
               fb->red_mask_size,
               fb->red_mask_shift)
         | fb_channel(
               green,
               fb->green_mask_size,
               fb->green_mask_shift)
         | fb_channel(
               blue,
               fb->blue_mask_size,
               fb->blue_mask_shift);
}

/* Draw the framebuffer gradient pattern. */
static void fb_pattern(struct limine_framebuffer *fb)
{
    volatile uint32_t *fb_ptr = fb->address;

    for (size_t y = 0; y < fb->height; y++) {
        for (size_t x = 0; x < fb->width; x++) {

            uint8_t nX = x * 255 / fb->width;
            uint8_t nY = y * 255 / fb->height;

            fb_ptr[y * (fb->pitch / 4) + x] =
                fb_pixel(fb, 0, nY, nX);
        }
    }
}

static void test_thread(void)
{
    serial_write("PrOS: >>> Kernel thread is running! <<<\n");

    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}

static void test_thread_a(void)
{
    serial_write("THREAD A: A1\n");
    thread_yield();

    serial_write("THREAD A: A2\n");
    thread_yield();

    serial_write("THREAD A: A3\n");

    serial_write("THREAD A: finished\n");

    thread_yield();

    for (;;) {
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }
}

static void test_thread_b(void)
{
    serial_write("THREAD B: B1\n");
    thread_yield();

    serial_write("THREAD B: B2\n");
    thread_yield();

    serial_write("THREAD B: B3\n");

    serial_write("THREAD B: finished\n");

    for (;;) {
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }
}

/* ---------------------------------------------------------
 * Kernel entry point
 * --------------------------------------------------------- */

void kmain(void)
{
    /* Initialize serial debugging. */
    serial_init();

    serial_write("\n");
    serial_write("PrOS: Kernel entered kmain()\n");
    serial_write("PrOS: Serial logging is working!\n");

    gdt_init();
    idt_init();
    tss_init();

    uint16_t cs;

    __asm__ volatile (
        "mov %%cs, %0"
        : "=r"(cs)
    );

    struct {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed)) gdtr;

    __asm__ volatile (
        "sgdt %0"
        : "=m"(gdtr)
    );

    serial_write("PrOS: Current GDT base: ");
    serial_write_hex(gdtr.base);

    serial_write("PrOS: Current GDT limit: ");
    serial_write_hex(gdtr.limit);

    serial_write("PrOS: Current CS selector: ");
    serial_write_hex(cs);
    serial_write("\n");

    // serial_write("PrOS: GDT descriptor dump\n");
    //
    // uint64_t *gdt =
    //     (uint64_t *)gdtr.base;
    //
    // for (uint64_t i = 0; i <= gdtr.limit / 8; i++) {
    //     serial_write("GDT[");
    //     serial_write_dec(i);
    //     serial_write("] = ");
    //     serial_write_hex(gdt[i]);
    //     serial_write("\n");
    // }

    /* Verify Limine base revision support. */
    if (!LIMINE_BASE_REVISION_SUPPORTED(
            limine_base_revision)) {

        serial_write("PrOS: ERROR: Limine base revision unsupported\n");
        hcf();
    }

    serial_write("PrOS: Limine base revision supported\n");

    /* Inspect the page tables installed by Limine. */
    uint64_t cr3 = read_cr3();

    serial_write("PrOS: Current CR3: ");
    serial_write_hex(cr3);
    serial_write("\n");

    serial_write("PrOS: PML4 physical base: ");
    serial_write_hex(cr3 & 0x000FFFFFFFFFF000ULL);
    serial_write("\n");

    /* Read Limine's higher-half direct map (HHDM) response. */
    struct limine_hhdm_response *hhdm =
        hhdm_request.response;

    if (hhdm == NULL) {
        serial_write("PrOS: ERROR: HHDM unavailable\n");
        hcf();
    }

    serial_write("PrOS: HHDM offset: ");
    serial_write_hex(hhdm->offset);
    serial_write("\n");

    /*
 * Inspect the PML4 table through the HHDM.
 * CR3 contains the physical address of the PML4.
 */
    uint64_t pml4_phys =
        cr3 & 0x000FFFFFFFFFF000ULL;

    volatile uint64_t *pml4 =
        (volatile uint64_t *)(pml4_phys + hhdm->offset);

    serial_write("PrOS: PML4 virtual address: ");
    serial_write_hex((uint64_t)pml4);
    serial_write("\n");

    /*
     * The kernel is in the upper canonical half.
     * Its PML4 index is 511.
     */
    uint64_t kernel_pml4_index = 511;

    uint64_t pml4_entry = pml4[kernel_pml4_index];

    serial_write("PrOS: Kernel PML4 index: ");
    serial_write_dec(kernel_pml4_index);
    serial_write("\n");

    serial_write("PrOS: PML4[511]: ");
    serial_write_hex(pml4_entry);
    serial_write("\n");

    if (pml4_entry & 1) {
        serial_write("PrOS: Kernel PML4 entry is PRESENT\n");
    } else {
        serial_write("PrOS: ERROR: Kernel PML4 entry is not present\n");
        hcf();
    }

    /* Get the physical address of the PDPT. */
    uint64_t pdpt_phys =
        pml4_entry & 0x000FFFFFFFFFF000ULL;

    /* Convert the PDPT physical address through HHDM. */
    volatile uint64_t *pdpt =
        (volatile uint64_t *)(pdpt_phys + hhdm->offset);

    /*
     * Kernel VA: 0xFFFFFFFF80000000
     * PDPT index: 510
     */
    uint64_t kernel_pdpt_index = 510;

    uint64_t pdpt_entry = pdpt[kernel_pdpt_index];

    serial_write("PrOS: PDPT virtual address: ");
    serial_write_hex((uint64_t)pdpt);
    serial_write("\n");

    serial_write("PrOS: Kernel PDPT index: ");
    serial_write_dec(kernel_pdpt_index);
    serial_write("\n");

    serial_write("PrOS: PDPT[510]: ");
    serial_write_hex(pdpt_entry);
    serial_write("\n");

    if (!(pdpt_entry & 1)) {
        serial_write("PrOS: ERROR: PDPT entry not present\n");
        hcf();
    }

    if (pdpt_entry & (1ULL << 7)) {
        serial_write("PrOS: PDPT entry uses a 1 GiB page\n");
    } else {
        serial_write("PrOS: PDPT entry points to a page directory\n");
    }

    /*
 * Get the physical address of the page directory.
 */
    uint64_t pd_phys =
        pdpt_entry & 0x000FFFFFFFFFF000ULL;

    /*
     * Convert physical address to HHDM virtual address.
     */
    volatile uint64_t *pd =
        (volatile uint64_t *)(pd_phys + hhdm->offset);

    /*
     * Kernel VA: 0xFFFFFFFF80000000
     * Page Directory index: 0
     */
    uint64_t kernel_pd_index = 0;

    uint64_t pd_entry = pd[kernel_pd_index];

    serial_write("PrOS: Page Directory virtual address: ");
    serial_write_hex((uint64_t)pd);
    serial_write("\n");

    serial_write("PrOS: Kernel PD index: ");
    serial_write_dec(kernel_pd_index);
    serial_write("\n");

    serial_write("PrOS: PDE[0]: ");
    serial_write_hex(pd_entry);
    serial_write("\n");

    if (!(pd_entry & 1)) {
        serial_write("PrOS: ERROR: PDE not present\n");
        hcf();
    }

    if (pd_entry & (1ULL << 7)) {
        serial_write("PrOS: Kernel uses a 2 MiB page mapping\n");

        uint64_t page_2m_phys =
            pd_entry & 0x000FFFFFFFE00000ULL;

        serial_write("PrOS: 2 MiB page physical base: ");
        serial_write_hex(page_2m_phys);
        serial_write("\n");
    } else {
        serial_write("PrOS: PDE points to a page table (4 KiB pages)\n");

        /* Get the physical address of the page table. */
        uint64_t pt_phys =
            pd_entry & 0x000FFFFFFFFFF000ULL;

        /* Access the page table through the HHDM. */
        volatile uint64_t *pt =
            (volatile uint64_t *)(pt_phys + hhdm->offset);

        /*
         * Kernel VA: 0xFFFFFFFF80000000
         * Page Table index: 0
         */
        uint64_t kernel_pt_index = 0;

        uint64_t pt_entry = pt[kernel_pt_index];

        serial_write("PrOS: Page Table virtual address: ");
        serial_write_hex((uint64_t)pt);
        serial_write("\n");

        serial_write("PrOS: Kernel PT index: ");
        serial_write_dec(kernel_pt_index);
        serial_write("\n");

        serial_write("PrOS: PTE[0]: ");
        serial_write_hex(pt_entry);
        serial_write("\n");

        if (!(pt_entry & 1)) {
            serial_write("PrOS: ERROR: PTE not present\n");
            hcf();
        }

        uint64_t mapped_phys =
            pt_entry & 0x000FFFFFFFFFF000ULL;

        serial_write("PrOS: Kernel virtual address: ");
        serial_write_hex(0xFFFFFFFF80000000ULL);
        serial_write("\n");

        serial_write("PrOS: Mapped physical address: ");
        serial_write_hex(mapped_phys);
        serial_write("\n");
    }

    /* Print executable physical and virtual load bases. */
    struct limine_executable_address_response *exec_addr =
        executable_address_request.response;

    if (exec_addr == NULL) {
        serial_write("PrOS: ERROR: Executable address unavailable\n");
        hcf();
    }

    serial_write("PrOS: Executable physical base: ");
    serial_write_hex(exec_addr->physical_base);
    serial_write("\n");

    serial_write("PrOS: Executable virtual base:  ");
    serial_write_hex(exec_addr->virtual_base);
    serial_write("\n");

    /* Inspect physical memory before framebuffer initialization. */
    print_memory_map();

    if (!pmm_init(memmap_request.response)) {
        serial_write("PrOS: PMM initialization failed\n");
        hcf();
    }

    serial_write("PrOS: PMM initialized successfully\n");

    /* Initialize the virtual memory manager. */
    vmm_init(hhdm->offset);

    serial_write("PrOS: VMM initialized successfully\n");

    /* Initialize the kernel heap. */
    if (!heap_init()) {
        serial_write("PrOS: Kernel heap initialization FAILED\n");
        hcf();
    }

    serial_write("PrOS: Kernel heap initialized successfully\n");
    /* ========================================
   VMM mapping test
   ======================================== */

    serial_write("PrOS: Testing virtual memory mapping\n");

    /*
     * Allocate one physical page.
     */
    uint64_t test_phys = pmm_alloc_page();

    if (test_phys == 0) {
        serial_write("PrOS: ERROR: Physical allocation failed\n");
        hcf();
    }

    serial_write("PrOS: Test physical page: ");
    serial_write_hex(test_phys);
    serial_write("\n");

    /*
     * Dedicated test virtual address.
     * Must remain unused by the kernel.
     */
    uint64_t test_virt = 0xFFFF900000000000ULL;

    bool mapped = vmm_map_page(
        test_virt,
        test_phys,
        VMM_WRITABLE | VMM_NO_EXEC
    );

    if (!mapped) {
        serial_write("PrOS: ERROR: Virtual mapping failed\n");

        pmm_free_page(test_phys);
        hcf();
    }

    serial_write("PrOS: Virtual mapping successful\n");

    /*
     * Access the physical page through its new
     * virtual address.
     */
    volatile uint64_t *test_ptr =
        (volatile uint64_t *)test_virt;

    *test_ptr = 0x123456789ABCDEF0ULL;

    uint64_t readback = *test_ptr;

    serial_write("PrOS: Value written: ");
    serial_write_hex(0x123456789ABCDEF0ULL);
    serial_write("\n");

    serial_write("PrOS: Value read back: ");
    serial_write_hex(readback);
    serial_write("\n");

    if (readback == 0x123456789ABCDEF0ULL) {
        serial_write("PrOS: VMM mapping test PASSED\n");
    } else {
        serial_write("PrOS: VMM mapping test FAILED\n");
        hcf();
    }

    serial_write("PrOS: Total pages: ");
    serial_write_dec(pmm_get_total_pages());
    serial_write("\n");

    serial_write("PrOS: Free pages: ");
    serial_write_dec(pmm_get_free_pages());
    serial_write("\n");

    /* PMM allocation and free test. */
    uint64_t free_before = pmm_get_free_pages();

    serial_write("PrOS: Testing page allocation\n");

    uint64_t page = pmm_alloc_page();

    if (page == 0) {
        serial_write("PrOS: ERROR: Page allocation failed\n");
        hcf();
    }

    serial_write("PrOS: Allocated physical address: ");
    serial_write_hex(page);
    serial_write("\n");

    serial_write("PrOS: Free pages after allocation: ");
    serial_write_dec(pmm_get_free_pages());
    serial_write("\n");

    /* Free the allocated page. */
    if (!pmm_free_page(page)) {
        serial_write("PrOS: ERROR: Page free failed\n");
        hcf();
    }

    serial_write("PrOS: Page freed successfully\n");

    serial_write("PrOS: Free pages after freeing: ");
    serial_write_dec(pmm_get_free_pages());
    serial_write("\n");

    /* Verify that the original count was restored. */
    if (pmm_get_free_pages() != free_before) {
        serial_write("PrOS: ERROR: PMM counter mismatch\n");
        hcf();
    }

    serial_write("PrOS: PMM allocation/free test PASSED\n");

    serial_write("PrOS: Testing reserved-page protection\n");

    free_before = pmm_get_free_pages();
    if (pmm_free_page(0x1000)) {
        serial_write("PrOS: ERROR: Reserved page was freed!\n");
        hcf();
    }

    if (pmm_get_free_pages() != free_before) {
        serial_write("PrOS: ERROR: Free-page count changed!\n");
        hcf();
    }

    serial_write("PrOS: Reserved-page protection PASSED\n");

    /* Verify framebuffer response. */
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {

        serial_write("PrOS: ERROR: No framebuffer available\n");
        hcf();
    }

    /* Draw the pattern on every framebuffer. */
    for (uint64_t i = 0;
         i < framebuffer_request.response->framebuffer_count;
         i++) {

        struct limine_framebuffer *framebuffer =
            framebuffer_request.response->framebuffers[i];

        if (framebuffer->memory_model != LIMINE_FRAMEBUFFER_RGB ||
            framebuffer->bpp != 32) {

            serial_write("PrOS: ERROR: Unsupported framebuffer format\n");
            hcf();
        }

        fb_pattern(framebuffer);
    }

    serial_write("PrOS: Framebuffer initialized\n");

    /*
     * Reclaim bootloader memory only after all current
     * Limine-provided data has been used.
     */
    /*
     * Bootloader memory reclamation is temporarily deferred.
     *
     * The kernel is still executing on the boot-time stack.
     * We will reclaim bootloader memory after switching to
     * a PrOS-owned kernel stack.
     */
    serial_write(
        "PrOS: Bootloader memory reclamation deferred\\n"
    );
    serial_write("PrOS: Kernel initialization complete\n");

    // serial_write("PrOS: Triggering controlled page fault\n");
    //
    // /* Deliberately access an unmapped canonical kernel address. */
    // volatile uint64_t *fault_ptr =
    //     (volatile uint64_t *)0xFFFFA00000000000ULL;
    //
    // uint64_t value = *fault_ptr;
    //
    // /* Should never execute if the page fault is handled correctly. */
    // serial_write("ERROR: Page fault did not occur\n");
    // serial_write_hex(value);

    /* Halt the CPU. */
    // serial_write("PrOS: Triggering invalid opcode (#UD)\n");
    //
    // __asm__ volatile ("ud2");
    //
    // serial_write("ERROR: UD2 did not trigger an exception\n");

    serial_write("PrOS: Testing kernel heap\n");

    /* Test 1: Allocate two objects. */
    uint64_t *a = kmalloc(sizeof(uint64_t));
    uint64_t *b = kmalloc(128);

    if (a == NULL || b == NULL) {
        serial_write("PrOS: Heap allocation FAILED\n");
        hcf();
    }

    *a = 0x123456789ABCDEF0ULL;

    serial_write("PrOS: Allocation and write successful\n");

    /* Test 2: Verify stored data. */
    if (*a != 0x123456789ABCDEF0ULL) {
        serial_write("PrOS: Heap data verification FAILED\n");
        hcf();
    }

    serial_write("PrOS: Heap data verification PASSED\n");

    /* Test 3: Free and reuse memory. */
    kfree(a);
    kfree(b);

    serial_write("PrOS: Heap free successful\n");

    uint64_t *c = kmalloc(sizeof(uint64_t));

    if (c == NULL) {
        serial_write("PrOS: Heap reuse allocation FAILED\n");
        hcf();
    }

    *c = 0xCAFEBABE12345678ULL;

    if (*c != 0xCAFEBABE12345678ULL) {
        serial_write("PrOS: Heap reuse verification FAILED\n");
        hcf();
    }

    serial_write("PrOS: Heap reuse test PASSED\n");

    serial_write("PrOS: Heap total bytes: ");
    serial_write_dec(heap_get_total_bytes());
    serial_write("\n");

    serial_write("PrOS: Heap used bytes: ");
    serial_write_dec(heap_get_used_bytes());
    serial_write("\n");

    kfree(c);

    serial_write("PrOS: Kernel heap test PASSED\n");

    serial_write("PrOS: Testing kernel threads\n");

    thread_init();

    struct thread *thread_a =
        thread_create(test_thread_a);

    if (thread_a == 0) {
        serial_write("PrOS: Thread A creation FAILED\n");
        hcf();
    }

    serial_write("PrOS: Thread A created\n");

    struct thread *thread_b =
        thread_create(test_thread_b);

    if (thread_b == 0) {
        serial_write("PrOS: Thread B creation FAILED\n");
        hcf();
    }

    serial_write("PrOS: Thread B created\n");

    scheduler_set_threads(thread_a, thread_b);

    serial_write("PrOS: Scheduler initialized\n");

    thread_start(thread_a);

    hcf();
}