#ifndef MAC_TABLE_H
#define MAC_TABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAC_ADDR_LEN 6
#define MAC_TABLE_CAPACITY 256

typedef enum {
    MAC_ENTRY_EMPTY = 0,
    MAC_ENTRY_OCCUPIED,
    MAC_ENTRY_DELETED
} mac_entry_state_t;

typedef struct {
    uint8_t mac[MAC_ADDR_LEN];
    uint16_t vlan_id;
    uint16_t port_id;
    uint64_t last_seen_ms;
    mac_entry_state_t state;
} mac_entry_t;

typedef struct {
    mac_entry_t entries[MAC_TABLE_CAPACITY];
    size_t count;
} mac_table_t;

void mac_table_init(mac_table_t *table);

int mac_table_learn(
    mac_table_t *table,
    const uint8_t mac[MAC_ADDR_LEN],
    uint16_t vlan_id,
    uint16_t port_id,
    uint64_t now_ms
);

bool mac_table_lookup(
    const mac_table_t *table,
    const uint8_t mac[MAC_ADDR_LEN],
    uint16_t vlan_id,
    uint16_t *port_id
);

size_t mac_table_age(
    mac_table_t *table,
    uint64_t now_ms,
    uint64_t aging_time_ms
);

void mac_table_print(const mac_table_t *table);

#endif
