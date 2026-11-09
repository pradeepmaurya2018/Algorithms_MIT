#include <string.h>

#include "neighbor_table.h"

static bool mac_is_valid(const uint8_t mac[MAC_ADDR_LEN])
{
    if (mac == NULL || (mac[0] & 0x01U) != 0)
        return false;

    for (size_t i = 0; i < MAC_ADDR_LEN; i++) {
        if (mac[i] != 0)
            return true;
    }

    return false;
}

void neighbor_table_init(neighbor_table_t *table)
{
    if (table == NULL)
        return;

    memset(table, 0, sizeof(*table));
}

int neighbor_table_add(
    neighbor_table_t *table,
    uint32_t ip_address,
    uint16_t vlan_id,
    uint16_t port_id,
    const uint8_t mac[MAC_ADDR_LEN])
{
    if (table == NULL || ip_address == 0 ||
        vlan_id == 0 || vlan_id > NEIGHBOR_MAX_VLAN_ID ||
        port_id == 0 || !mac_is_valid(mac))
        return -1;

    neighbor_entry_t *free_entry = NULL;

    for (size_t i = 0; i < NEIGHBOR_TABLE_CAPACITY; i++) {
        neighbor_entry_t *entry = &table->entries[i];

        if (entry->valid &&
            entry->ip_address == ip_address &&
            entry->vlan_id == vlan_id) {

            entry->port_id = port_id;
            memcpy(entry->mac, mac, MAC_ADDR_LEN);
            return 0;
        }

        if (!entry->valid && free_entry == NULL)
            free_entry = entry;
    }

    if (free_entry == NULL)
        return -1;

    free_entry->ip_address = ip_address;
    free_entry->vlan_id = vlan_id;
    free_entry->port_id = port_id;
    memcpy(free_entry->mac, mac, MAC_ADDR_LEN);
    free_entry->valid = true;

    table->count++;

    return 0;
}

bool neighbor_table_lookup(
    const neighbor_table_t *table,
    uint32_t ip_address,
    uint16_t vlan_id,
    uint8_t mac[MAC_ADDR_LEN],
    uint16_t *port_id)
{
    if (table == NULL || mac == NULL || port_id == NULL ||
        ip_address == 0 || vlan_id == 0)
        return false;

    for (size_t i = 0; i < NEIGHBOR_TABLE_CAPACITY; i++) {
        const neighbor_entry_t *entry = &table->entries[i];

        if (entry->valid &&
            entry->ip_address == ip_address &&
            entry->vlan_id == vlan_id) {

            memcpy(mac, entry->mac, MAC_ADDR_LEN);
            *port_id = entry->port_id;
            return true;
        }
    }

    return false;
}

bool neighbor_table_remove(
    neighbor_table_t *table,
    uint32_t ip_address,
    uint16_t vlan_id)
{
    if (table == NULL || ip_address == 0 || vlan_id == 0)
        return false;

    for (size_t i = 0; i < NEIGHBOR_TABLE_CAPACITY; i++) {
        neighbor_entry_t *entry = &table->entries[i];

        if (entry->valid &&
            entry->ip_address == ip_address &&
            entry->vlan_id == vlan_id) {

            memset(entry, 0, sizeof(*entry));
            table->count--;
            return true;
        }
    }

    return false;
}
