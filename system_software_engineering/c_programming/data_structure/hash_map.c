#include <stdio.h>
#include <stdint.h>
#define MAX_SIZE 100

typedef struct Entry {
    uint32_t key;
    uint32_t val;
    struct Entry* next;
}Entry_t;

typedef struct hash_map {
    Entry_t map[MAX_SIZE];
    uint32_t size;
} map_t;

int map_hash(uint32_t key) {
    return key%MAX_SIZE;
}

int map_put(map_t* map, uint32_t key, uint32_t value) {
    Entry_t entry={.key=key,.val=value,.next=NULL};
    map->map[map_hash(key)]=entry;
    map->size++;
    return 0;
}

int map_get(map_t* map, uint32_t key, Entry_t* entry) {
    printf("%d,%d,%d\n", map->size, key, entry->key, entry->val);
    Entry_t entry_1=map->map[map_hash(key)];
    entry=&entry_1;
}

int main(int argc,char*argv[]) {
    map_t table;
    for(int i=1;i<29;i++) {
        map_put(&table,i,i*i);
    }
    Entry_t temp;
    for(int i=1;i<29;i++) {
        map_get(&table,i, &temp);
        printf("Key %d, Val %d\n", temp.key, temp.val);
    }
}
