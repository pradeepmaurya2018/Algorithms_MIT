#include <stdio.h>
#include <string.h>

#include "mac_table.h"

static size_t mac_hash(
    const uint8_t mac[MAC_ADDR_LEN],
    uint16_t vlan_id)
{
    uint32_t hash = 2166136261u;

    for (size_t i = 0; i < MAC_ADDR_LEN; i++) {
        hash ^= mac[i];
        hash *= 16777619u;
    }

    hash ^= (uint8_t)(vlan_id >> 8);
    hash *= 16777619u;

    hash ^= (uint8_t)vlan_id;
    hash *= 16777619u;

    return hash % MAC_TABLE_CAPACITY;
}

static bool mac_entry_matches(
    const mac_entry_t *entry,
    const uint8_t mac[MAC_ADDR_LEN],
    uint16_t vlan_id)
{
    return entry->state == MAC_ENTRY_OCCUPIED &&
           entry->vlan_id == vlan_id &&
           memcmp(entry->mac, mac, MAC_ADDR_LEN) == 0;
}

void mac_table_init(mac_table_t *table)
{
    if (table == NULL)
        return;

    memset(table, 0, sizeof(*table));
}

int mac_table_learn(
    mac_table_t *table,
    const uint8_t mac[MAC_ADDR_LEN],
    uint16_t vlan_id,
    uint16_t port_id,
    uint64_t now_ms)
{
    if (table == NULL || mac == NULL)
        return -1;

    if (vlan_id > 4094)
        return -1;

    size_t index = mac_hash(mac, vlan_id);
    size_t first_deleted = MAC_TABLE_CAPACITY;

    for (size_t i = 0; i < MAC_TABLE_CAPACITY; i++) {

        size_t pos = (index + i) % MAC_TABLE_CAPACITY;
        mac_entry_t *entry = &table->entries[pos];

        if (entry->state == MAC_ENTRY_EMPTY) {

            if (first_deleted != MAC_TABLE_CAPACITY)
                entry = &table->entries[first_deleted];

            memcpy(entry->mac, mac, MAC_ADDR_LEN);

            entry->vlan_id = vlan_id;
            entry->port_id = port_id;
            entry->last_seen_ms = now_ms;
            entry->state = MAC_ENTRY_OCCUPIED;

            table->count++;

            return 0;
        }

        if (entry->state == MAC_ENTRY_DELETED) {

            if (first_deleted == MAC_TABLE_CAPACITY)
                first_deleted = pos;

            continue;
        }

        if (mac_entry_matches(entry, mac, vlan_id)) {

            /* MAC moved or refreshed. */
            entry->port_id = port_id;
            entry->last_seen_ms = now_ms;

            return 0;
        }
    }

    /*
     * Table may contain tombstones but no EMPTY entries.
     */
    if (first_deleted != MAC_TABLE_CAPACITY) {

        mac_entry_t *entry = &table->entries[first_deleted];

        memcpy(entry->mac, mac, MAC_ADDR_LEN);

        entry->vlan_id = vlan_id;
        entry->port_id = port_id;
        entry->last_seen_ms = now_ms;
        entry->state = MAC_ENTRY_OCCUPIED;

        table->count++;

        return 0;
    }

    return -1;
}

bool mac_table_lookup(
    const mac_table_t *table,
    const uint8_t mac[MAC_ADDR_LEN],
    uint16_t vlan_id,
    uint16_t *port_id)
{
    if (table == NULL || mac == NULL || port_id == NULL)
        return false;

    if (vlan_id > 4094)
        return false;

    size_t index = mac_hash(mac, vlan_id);

    for (size_t i = 0; i < MAC_TABLE_CAPACITY; i++) {

        size_t pos = (index + i) % MAC_TABLE_CAPACITY;

        const mac_entry_t *entry = &table->entries[pos];

        if (entry->state == MAC_ENTRY_EMPTY)
            return false;

        if (mac_entry_matches(entry, mac, vlan_id)) {

            *port_id = entry->port_id;

            return true;
        }
    }

    return false;
}

size_t mac_table_age(
    mac_table_t *table,
    uint64_t now_ms,
    uint64_t aging_time_ms)
{
    if (table == NULL)
        return 0;

    size_t removed = 0;

    for (size_t i = 0; i < MAC_TABLE_CAPACITY; i++) {

        mac_entry_t *entry = &table->entries[i];

        if (entry->state != MAC_ENTRY_OCCUPIED)
            continue;

        /*
         * Protect against timestamps moving backwards.
         */
        if (now_ms < entry->last_seen_ms)
            continue;

        if (now_ms - entry->last_seen_ms >= aging_time_ms) {

            entry->state = MAC_ENTRY_DELETED;

            table->count--;
            removed++;
        }
    }

    return removed;
}

void mac_table_print(const mac_table_t *table)
{
    if (table == NULL)
        return;

    printf("\nMAC Address Forwarding Database\n");

    printf("-----------------------------------------------\n");

    printf("%-20s %-8s %-8s\n", "MAC Address", "VLAN", "PORT");

    printf("-----------------------------------------------\n");

    for (size_t i = 0; i < MAC_TABLE_CAPACITY; i++) {

        const mac_entry_t *entry = &table->entries[i];

        if (entry->state != MAC_ENTRY_OCCUPIED)
            continue;

        printf(
            "%02x:%02x:%02x:%02x:%02x:%02x  %-8u %-8u\n",
            entry->mac[0],
            entry->mac[1],
            entry->mac[2],
            entry->mac[3],
            entry->mac[4],
            entry->mac[5],
            entry->vlan_id,
            entry->port_id
        );
    }

    printf("-----------------------------------------------\n");

    printf("Total entries: %zu\n", table->count);
}
