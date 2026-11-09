#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define CAP 100
typedef struct {
    uint32_t key;
    uint32_t val;
}entry_t;

typedef struct node{
    entry_t data;
    struct node* next;
    struct node* prev;
} node_t;

typedef struct {
    node_t* head;
    node_t* tail;
}dll_t;

typedef struct {
    entry_t table[CAP];
}map_t;

void* new_node(entry_t data) {
    node_t* new_n=(node_t*)malloc(sizeof(node_t));
    new_n->next=NULL;
    new_n->prev=NULL;
    new_n->data=data;
    return new_n;
}


int dll_push_front(dll_t* dll, uint32_t k, uint32_t v) {
    entry_t entry={k,v};
    if(dll->head==NULL) {

        dll->head=new_node(entry);
    }
    else {
        node_t* new_n=new_node(entry);
        new_n->next=dll->head;
        dll->head=new_n;
    }
}


uint32_t hash(uint32_t k) {
    return k%CAP;
}
int map_put(map_t* map, uint32_t k, uint32_t v) {
    map->table[hash(k)]=(entry_t){k,v};
}
uint32_t map_get(map_t* map, uint32_t k) {
    return map->table[hash(k)].val;
}

typedef struct LRU {
    size_t capacity;
    dll_t dll;
    map_t map;
}lru_t;

int lru_init(lru_t* lru, int cap) {
    lru->capacity=cap;
    // lru->dll=malloc(sizeof(dll_t));
}

int lru_put(lru_t* lru, int k, int v) {
    map_put(&lru->map, k,v);
    dll_push_front(&lru->dll, k, v);
}
int print_lru(lru_t* lru) {
    printf("LRU list        \n");
    node_t* temp=lru->dll.head;
    while(temp!=NULL) {
        printf("%d %d|", temp->data.key, temp->data.val);
        temp=temp->next;
    }
    printf("\n");

    printf("MAP        \n");
}
int main(int argc,char*argv[]) {
    lru_t lru;
    lru_init(&lru, 3);
    lru_put(&lru,1,2);
    lru_put(&lru,2,3);
    lru_put(&lru,3,4);
    print_lru(&lru);
}


