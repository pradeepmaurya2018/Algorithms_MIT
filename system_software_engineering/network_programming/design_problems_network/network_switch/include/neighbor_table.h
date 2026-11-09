#ifndef NEIGHBOR_TABLE_H
#define NEIGHBOR_TABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mac_table.h"

#define NEIGHBOR_TABLE_CAPACITY 128
#define NEIGHBOR_MAX_VLAN_ID 4094

typedef struct {
    uint32_t ip_address;
    uint16_t vlan_id;
    uint16_t port_id;
    uint8_t mac[MAC_ADDR_LEN];
    bool valid;
} neighbor_entry_t;

typedef struct {
    neighbor_entry_t entries[NEIGHBOR_TABLE_CAPACITY];
    size_t count;
} neighbor_table_t;

void neighbor_table_init(neighbor_table_t *table);

int neighbor_table_add(
    neighbor_table_t *table,
    uint32_t ip_address,
    uint16_t vlan_id,
    uint16_t port_id,
    const uint8_t mac[MAC_ADDR_LEN]
);

bool neighbor_table_lookup(
    const neighbor_table_t *table,
    uint32_t ip_address,
    uint16_t vlan_id,
    uint8_t mac[MAC_ADDR_LEN],
    uint16_t *port_id
);

bool neighbor_table_remove(
    neighbor_table_t *table,
    uint32_t ip_address,
    uint16_t vlan_id
);

#endif
