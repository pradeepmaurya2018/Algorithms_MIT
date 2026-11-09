#ifndef ROUTE_TABLE_H
#define ROUTE_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#define IPV4_PREFIX_MAX 32

typedef struct {
    uint32_t network;
    uint8_t prefix_length;

    uint32_t next_hop;
    uint16_t egress_port;
    uint16_t vlan_id;
} route_entry_t;

typedef struct route_node {
    struct route_node *child[2];

    bool has_route;
    route_entry_t route;
} route_node_t;

typedef struct {
    route_node_t *root;
    uint32_t route_count;
} route_table_t;

void route_table_init(route_table_t *table);

int route_table_insert(
    route_table_t *table,
    uint32_t network,
    uint8_t prefix_length,
    uint32_t next_hop,
    uint16_t egress_port,
    uint16_t vlan_id
);

bool route_table_lookup(
    const route_table_t *table,
    uint32_t destination_ip,
    route_entry_t *result
);

void route_table_destroy(route_table_t *table);

#endif
