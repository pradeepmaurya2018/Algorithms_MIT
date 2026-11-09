#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct free_node{
    struct free_node* next;
}

free_node_t;
typedef struct {
    u_char *start_address;
    u_char *end_address;
    u_char *current_address;
    size_t current_size;
    size_t max_pool_size;

} pool_t;

uint32_t pool_init(pool_t* pool, size_t pool_size) {
    pool->start_address=(u_char*)malloc(pool_size);

    pool->end_address=pool->start_address+pool_size;
    pool->current_address=pool->start_address;
    pool->current_size=0;
    pool->max_pool_size=pool_size;

    return 0;
}

void* pool_alloc(pool_t* p, int memory_size) {
    if(p->current_size+memory_size>p->max_pool_size) return NULL;
    u_char* temp_current_address=p->current_address;
    p->current_address=p->current_address+memory_size;
    p->current_size+=memory_size;
    return temp_current_address;
}
int pool_free(pool_t* p, void* start_addess) {

}

int main(int argc,char*argv[]) {

}
