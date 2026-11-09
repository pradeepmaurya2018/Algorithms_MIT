#include <string.h>

#include "pending_queue.h"

void pending_queue_init(pending_queue_t *queue)
{
    if (queue == NULL)
        return;

    memset(queue, 0, sizeof(*queue));
}

int pending_queue_enqueue(
    pending_queue_t *queue,
    const packet_t *packet,
    uint32_t next_hop_ip,
    uint16_t vlan_id,
    uint64_t now_ms)
{
    if (queue == NULL || packet == NULL ||
        packet->len == 0 || packet->len > MAX_PACKET_SIZE ||
        next_hop_ip == 0 || vlan_id == 0)
        return -1;

    for (size_t i = 0; i < PENDING_QUEUE_CAPACITY; i++) {
        pending_packet_t *entry = &queue->entries[i];

        if (entry->valid)
            continue;

        entry->packet = *packet;
        entry->next_hop_ip = next_hop_ip;
        entry->vlan_id = vlan_id;
        entry->queued_at_ms = now_ms;
        entry->valid = true;

        queue->count++;
        return 0;
    }

    return -1;
}

bool pending_queue_contains(
    const pending_queue_t *queue,
    uint32_t next_hop_ip,
    uint16_t vlan_id)
{
    if (queue == NULL)
        return false;

    for (size_t i = 0; i < PENDING_QUEUE_CAPACITY; i++) {
        const pending_packet_t *entry = &queue->entries[i];

        if (entry->valid &&
            entry->next_hop_ip == next_hop_ip &&
            entry->vlan_id == vlan_id)
            return true;
    }

    return false;
}

bool pending_queue_pop_match(
    pending_queue_t *queue,
    uint32_t next_hop_ip,
    uint16_t vlan_id,
    pending_packet_t *result)
{
    if (queue == NULL || result == NULL)
        return false;

    size_t oldest_index = PENDING_QUEUE_CAPACITY;
    uint64_t oldest_time = UINT64_MAX;

    for (size_t i = 0; i < PENDING_QUEUE_CAPACITY; i++) {
        const pending_packet_t *entry = &queue->entries[i];

        if (!entry->valid ||
            entry->next_hop_ip != next_hop_ip ||
            entry->vlan_id != vlan_id)
            continue;

        if (oldest_index == PENDING_QUEUE_CAPACITY ||
            entry->queued_at_ms < oldest_time) {
            oldest_index = i;
            oldest_time = entry->queued_at_ms;
        }
    }

    if (oldest_index == PENDING_QUEUE_CAPACITY)
        return false;

    *result = queue->entries[oldest_index];

    memset(&queue->entries[oldest_index], 0,
           sizeof(queue->entries[oldest_index]));

    queue->count--;
    return true;
}

size_t pending_queue_age(
    pending_queue_t *queue,
    uint64_t now_ms,
    uint64_t timeout_ms)
{
    if (queue == NULL)
        return 0;

    size_t removed = 0;

    for (size_t i = 0; i < PENDING_QUEUE_CAPACITY; i++) {
        pending_packet_t *entry = &queue->entries[i];

        if (!entry->valid || now_ms < entry->queued_at_ms)
            continue;

        if (now_ms - entry->queued_at_ms < timeout_ms)
            continue;

        memset(entry, 0, sizeof(*entry));
        queue->count--;
        removed++;
    }

    return removed;
}

size_t pending_queue_count(const pending_queue_t *queue)
{
    return queue == NULL ? 0 : queue->count;
}
