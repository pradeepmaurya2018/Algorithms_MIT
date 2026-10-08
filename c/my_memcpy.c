#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* Your implementation */
void *my_memcopy(void *dest, const void *src, size_t n) {
    // n is the size in bytes
    unsigned char *s=src;
    unsigned char *d=dest;

    for(int i=0;i<n;i++) {
        *d++=*s++;
    }
}

/* -------------------------------------------------- */
/* Test helper                                         */
/* -------------------------------------------------- */

static int test_case(const char *name, size_t size)
{
    uint8_t *src1  = malloc(size);
    uint8_t *dst1  = malloc(size);
    uint8_t *src2  = malloc(size);
    uint8_t *dst2  = malloc(size);

    if (!src1 || !dst1 || !src2 || !dst2) {
        printf("[FAIL] %s: malloc failed\n", name);
        return 0;
    }

    /* Fill source with deterministic data */
    for (size_t i = 0; i < size; i++)
        src1[i] = (uint8_t)(i * 37 + 11);

    memcpy(src2, src1, size);

    /* Reference implementation */
    memcpy(dst1, src1, size);

    /* Your implementation */
    my_memcopy(dst2, src2, size);

    int pass = memcmp(dst1, dst2, size) == 0;

    printf("[%s] %s (%zu bytes)\n",
           pass ? "PASS" : "FAIL",
           name,
           size);

    if (!pass) {
        for (size_t i = 0; i < size; i++) {
            if (dst1[i] != dst2[i]) {
                printf("  First mismatch at byte %zu: "
                       "expected=0x%02x got=0x%02x\n",
                       i, dst1[i], dst2[i]);
                break;
            }
        }
    }

    free(src1);
    free(dst1);
    free(src2);
    free(dst2);

    return pass;
}

/* -------------------------------------------------- */
/* Main test harness                                   */
/* -------------------------------------------------- */

int main(void)
{
    int passed = 0;
    int total  = 0;

    size_t tests[] = {
        0,
        1,
        2,
        3,
        4,
        7,
        8,
        15,
        16,
        31,
        32,
        63,
        64,
        127,
        128,
        255,
        256,
        1024,
        4096,
        65536,
        1024 * 1024
    };

    size_t count = sizeof(tests) / sizeof(tests[0]);

    for (size_t i = 0; i < count; i++) {
        total++;
        passed += test_case("basic copy", tests[i]);
    }

    printf("\n=================================\n");
    printf("Result: %d/%d tests passed\n", passed, total);
    printf("=================================\n");

    return passed == total ? 0 : 1;
}