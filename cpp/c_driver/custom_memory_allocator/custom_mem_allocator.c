//
// Created by 2025 on 4/12/2026.
//
#include <stdio.h>
#include <unistd.h>
typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block_t;

#define BLOCK_SIZE sizeof(block_t)

block_t *head = NULL;

block_t *find_free_block(size_t size) {
    block_t *curr = head;
    while (curr) {
        if (curr->free && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

block_t *request_space(block_t *last, size_t size) {
    block_t *block = sbrk(0);
    void *req = sbrk(size + BLOCK_SIZE);
    if (req == (void*) -1)
        return NULL;

    block->size = size;
    block->free = 0;
    block->next = NULL;

    if (last)
        last->next = block;

    return block;
}
void *my_malloc(size_t size) {
    if (size <= 0) return NULL;

    block_t *block;

    if (!head) {
        block = request_space(NULL, size);
        if (!block) return NULL;
        head = block;
    } else {
        block_t *last = head;
        block = find_free_block(size);

        if (!block) {
            while (last->next) last = last->next;
            block = request_space(last, size);
            if (!block) return NULL;
        } else {
            block->free = 0;
        }
    }

    return (block + 1); // move past metadata
}

void my_free(void *ptr) {
    if (!ptr) return;

    block_t *block = (block_t*)ptr - 1;
    block->free = 1;
}

int main() {
    int *arr = my_malloc(5 * sizeof(int));

    for(int i = 0; i < 5; i++)
        arr[i] = i;

    for(int i = 0; i < 5; i++)
        printf("%d ", arr[i]);

    my_free(arr);
}