//
// Created by 2025 on 14-09-2026.
//
#include<stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#define MAX_ENTRIES 10

struct mac_entry {
    unsigned char mac[6];
    uint8_t port;
};

struct mac_table {
    struct mac_entry entries[MAX_ENTRIES];
    int count;
};

void print_table(struct mac_table* table) {
    for(int mac_entries=0; mac_entries<MAX_ENTRIES; mac_entries++) {
        printf("\n %s", table->entries[mac_entries].mac);
        printf("\n %d", table->entries[mac_entries].port);
    }
}

int main(int argc,char*argv[]) {
    struct mac_table table;
    table.entries[0]=(struct mac_entry){"prade", 1};
    table.count=0;
    printf("%s", table.entries[0].mac);
    print_table(&table);
}
