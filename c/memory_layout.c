// heap_internals.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void print_heap_top() {
    // sbrk(0) returns current program break (top of heap)
    printf("  heap top (brk)    : %p\n", sbrk(0));
}

int main() {
    printf("===== HEAP INTERNALS =====\n\n");

    printf("[1] Before any malloc:\n");
    print_heap_top();

    // small allocation — uses brk() internally
    printf("\n[2] After malloc(1024):\n");
    void *p1 = malloc(1024);
    print_heap_top();
    printf("  p1 returned        : %p\n", p1);

    printf("\n[3] After malloc(1024) again:\n");
    void *p2 = malloc(1024);
    print_heap_top();
    printf("  p2 returned        : %p\n", p2);
    printf("  p2 - p1            : %ld bytes\n",
           (char*)p2 - (char*)p1);

    // large allocation — uses mmap() internally (>128KB default)
    printf("\n[4] After malloc(200KB) — uses mmap:\n");
    void *p3 = malloc(200 * 1024);
    print_heap_top();    // brk doesn't move — mmap used instead
    printf("  p3 returned        : %p\n", p3);
    printf("  (notice: brk didn't move — mmap was used)\n");

    // free does NOT always return memory to OS immediately
    printf("\n[5] After free(p1):\n");
    free(p1);
    print_heap_top();    // brk usually doesn't shrink
    printf("  (brk same — freed memory stays in malloc's free list)\n");

    // malloc reuses freed memory
    printf("\n[6] After malloc(512) — reuses freed block:\n");
    void *p4 = malloc(512);
    print_heap_top();
    printf("  p4 returned        : %p\n", p4);
    printf("  p4 == p1?          : %s\n",
           (p4 == p1) ? "YES — reused!" : "no");

    free(p2);
    free(p3);
    free(p4);

    printf("\n==========================\n");
    return 0;
}