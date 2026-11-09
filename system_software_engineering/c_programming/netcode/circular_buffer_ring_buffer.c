#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*struc ring buffer*/
// #define RING_SIZE 10
int RING_SIZE;

typedef struct {
    int *ring;
    int front;
    int rear;
} ring_t;

int ring_init(ring_t* r, int size) {
    r->ring=malloc(sizeof(int)*size);
    r->front=-1;
    r->rear=-1;
    return 0;
}
int ring_is_empty(ring_t* ring) {
    if(ring->front==ring->rear) return 1;
    return 0;
}
int ring_is_full(ring_t* ring) {
    if((ring->rear+1)%RING_SIZE==ring->front) return 1;
    return 0;
}
int ring_push(ring_t *ring, int val) {
    if(ring_is_empty(ring)) return 1;
    ring->ring[ring->rear]=val;
    ring->rear=(ring->rear+1)%RING_SIZE;
}

int ring_pop(ring_t* ring, int *val) {
    if(ring_is_empty(ring)) return 1;
    *val=ring->ring[ring->front];
    ring->front=(ring->front+1)%RING_SIZE;
}

/* ========================================================= */
/* Test helpers                                               */
/* ========================================================= */

static void test_empty_buffer(void)
{
    printf("[TEST] Empty buffer\n");

    ring_t r;
    assert(ring_init(&r, 4) == 0);

    assert(ring_is_empty(&r));
    assert(!ring_is_full(&r));

    int value;

    assert(ring_pop(&r, &value) == -1);

    // ring_destroy(&r);

    printf("       PASS\n");
}


static void test_push_pop(void)
{
    printf("[TEST] Basic push/pop\n");

    ring_t r;
    assert(ring_init(&r, 4) == 0);

    assert(ring_push(&r, 10) == 0);
    assert(ring_push(&r, 20) == 0);
    assert(ring_push(&r, 30) == 0);

    int value;

    assert(ring_pop(&r, &value) == 0);
    assert(value == 10);

    assert(ring_pop(&r, &value) == 0);
    assert(value == 20);

    assert(ring_pop(&r, &value) == 0);
    assert(value == 30);

    assert(ring_is_empty(&r));

    // ring_destroy(&r);

    printf("       PASS\n");
}


static void test_full_buffer(void)
{
    printf("[TEST] Full buffer\n");

    ring_t r;
    assert(ring_init(&r, 4) == 0);

    assert(ring_push(&r, 1) == 0);
    assert(ring_push(&r, 2) == 0);
    assert(ring_push(&r, 3) == 0);
    assert(ring_push(&r, 4) == 0);

    assert(ring_is_full(&r));

    /* Fifth element must fail */
    assert(ring_push(&r, 5) == -1);

    // ring_destroy(&r);

    printf("       PASS\n");
}


static void test_fifo_order(void)
{
    printf("[TEST] FIFO ordering\n");

    ring_t r;
    assert(ring_init(&r, 8) == 0);

    for (int i = 0; i < 8; i++)
        assert(ring_push(&r, i) == 0);

    for (int i = 0; i < 8; i++) {
        int value;

        assert(ring_pop(&r, &value) == 0);
        assert(value == i);
    }

    assert(ring_is_empty(&r));

    // ring_destroy(&r);

    printf("       PASS\n");
}


/*
 * This is the important test.
 *
 * It forces head/tail to wrap around the end
 * of the underlying array.
 */
static void test_wrap_around(void)
{
    printf("[TEST] Wrap-around\n");

    ring_t r;
    assert(ring_init(&r, 4) == 0);

    /*
     * Fill:
     *
     * [10][20][30][40]
     *  H              T
     */
    assert(ring_push(&r, 10) == 0);
    assert(ring_push(&r, 20) == 0);
    assert(ring_push(&r, 30) == 0);
    assert(ring_push(&r, 40) == 0);

    int value;

    /* Remove two */
    assert(ring_pop(&r, &value) == 0);
    assert(value == 10);

    assert(ring_pop(&r, &value) == 0);
    assert(value == 20);

    /*
     * Now there are two free slots.
     *
     * Push these values.
     *
     * tail should wrap to the beginning.
     */
    assert(ring_push(&r, 50) == 0);
    assert(ring_push(&r, 60) == 0);

    /*
     * Expected FIFO:
     *
     * 30
     * 40
     * 50
     * 60
     */

    assert(ring_pop(&r, &value) == 0);
    assert(value == 30);

    assert(ring_pop(&r, &value) == 0);
    assert(value == 40);

    assert(ring_pop(&r, &value) == 0);
    assert(value == 50);

    assert(ring_pop(&r, &value) == 0);
    assert(value == 60);

    assert(ring_is_empty(&r));

    // ring_destroy(&r);

    printf("       PASS\n");
}


/*
 * Repeated wrap-around.
 *
 * This catches bugs that only appear after many
 * head/tail rotations.
 */
static void test_repeated_wrap(void)
{
    printf("[TEST] Repeated wrap-around\n");

    ring_t r;
    assert(ring_init(&r, 8) == 0);

    for (int round = 0; round < 10000; round++) {

        int input = round;
        int output;

        assert(ring_push(&r, input) == 0);

        assert(ring_pop(&r, &output) == 0);

        assert(output == input);
    }

    assert(ring_is_empty(&r));

    // ring_destroy(&r);

    printf("       PASS\n");
}


/*
 * Randomized test against a simple reference queue.
 */
static void test_randomized(void)
{
    printf("[TEST] Randomized operations\n");

    enum {
        CAPACITY = 32,
        OPERATIONS = 100000
    };

    ring_t r;
    assert(ring_init(&r, CAPACITY) == 0);

    int reference[CAPACITY];

    size_t ref_head = 0;
    size_t ref_tail = 0;
    size_t ref_count = 0;

    for (int i = 0; i < OPERATIONS; i++) {

        /*
         * If empty, must push.
         * If full, must pop.
         * Otherwise randomly choose.
         */
        int operation;

        if (ref_count == 0)
            operation = 0;
        else if (ref_count == CAPACITY)
            operation = 1;
        else
            operation = rand() % 2;

        if (operation == 0) {

            /* PUSH */

            int value = rand();

            assert(ring_push(&r, value) == 0);

            reference[ref_tail] = value;
            ref_tail = (ref_tail + 1) % CAPACITY;
            ref_count++;

        } else {

            /* POP */

            int expected = reference[ref_head];
            int actual;

            assert(ring_pop(&r, &actual) == 0);

            assert(actual == expected);

            ref_head = (ref_head + 1) % CAPACITY;
            ref_count--;
        }

        // assert(r.count == ref_count);
    }

    /* Drain remaining elements */

    while (ref_count > 0) {

        int actual;

        assert(ring_pop(&r, &actual) == 0);
        assert(actual == reference[ref_head]);

        ref_head = (ref_head + 1) % CAPACITY;
        ref_count--;
    }

    assert(ring_is_empty(&r));

    // ring_destroy(&r);

    printf("       PASS\n");
}


/* ========================================================= */
/* Main                                                      */
/* ========================================================= */

int main(void)
{
    printf("\n");
    printf("====================================\n");
    printf(" Circular Ring Buffer Test Harness\n");
    printf("====================================\n\n");

    test_empty_buffer();
    test_push_pop();
    test_full_buffer();
    test_fifo_order();
    test_wrap_around();
    test_repeated_wrap();
    test_randomized();

    printf("\n====================================\n");
    printf("ALL TESTS PASSED\n");
    printf("====================================\n");

    return 0;
}