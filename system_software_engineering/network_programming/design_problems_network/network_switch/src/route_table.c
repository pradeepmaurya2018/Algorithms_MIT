#include <stdlib.h>
#include <string.h>

#include "route_table.h"

static route_node_t *route_node_create(void)
{
    return calloc(1, sizeof(route_node_t));
}

static uint32_t prefix_mask(uint8_t prefix_length)
{
    if (prefix_length == 0)
        return 0;

    return UINT32_MAX << (32 - prefix_length);
}

void route_table_init(route_table_t *table)
{
    if (table == NULL)
        return;

    memset(table, 0, sizeof(*table));

    table->root = route_node_create();
}

int route_table_insert(
    route_table_t *table,
    uint32_t network,
    uint8_t prefix_length,
    uint32_t next_hop,
    uint16_t egress_port,
    uint16_t vlan_id)
{
    if (table == NULL || table->root == NULL)
        return -1;

    if (prefix_length > IPV4_PREFIX_MAX)
        return -1;

    uint32_t mask = prefix_mask(prefix_length);

    /* Normalize network address. */
    network &= mask;

    route_node_t *node = table->root;

    for (uint8_t bit = 0; bit < prefix_length; bit++) {

        uint8_t direction =
            (uint8_t)((network >> (31 - bit)) & 1U);

        if (node->child[direction] == NULL) {

            node->child[direction] = route_node_create();

            if (node->child[direction] == NULL)
                return -1;
        }

        node = node->child[direction];
    }

    if (!node->has_route)
        table->route_count++;

    node->route.network = network;
    node->route.prefix_length = prefix_length;
    node->route.next_hop = next_hop;
    node->route.egress_port = egress_port;
    node->route.vlan_id = vlan_id;

    node->has_route = true;

    return 0;
}

bool route_table_lookup(
    const route_table_t *table,
    uint32_t destination_ip,
    route_entry_t *result)
{
    if (table == NULL || table->root == NULL || result == NULL)
        return false;

    const route_node_t *node = table->root;

    bool found = false;

    /*
     * A /0 route is stored at the root.
     */
    if (node->has_route) {
        *result = node->route;
        found = true;
    }

    for (uint8_t bit = 0; bit < IPV4_PREFIX_MAX; bit++) {

        uint8_t direction =
            (uint8_t)((destination_ip >> (31 - bit)) & 1U);

        node = node->child[direction];

        if (node == NULL)
            break;

        if (node->has_route) {
            *result = node->route;
            found = true;
        }
    }

    return found;
}

static void route_node_destroy(route_node_t *node)
{
    if (node == NULL)
        return;

    route_node_destroy(node->child[0]);
    route_node_destroy(node->child[1]);

    free(node);
}

void route_table_destroy(route_table_t *table)
{
    if (table == NULL)
        return;

    route_node_destroy(table->root);

    table->root = NULL;
    table->route_count = 0;
}
