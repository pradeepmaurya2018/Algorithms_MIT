#ifndef PENDING_QUEUE_H
#define PENDING_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"

#define PENDING_QUEUE_CAPACITY 64

typedef struct {
    packet_t packet;
    uint32_t next_hop_ip;
    uint16_t vlan_id;
    uint64_t queued_at_ms;
    bool valid;
} pending_packet_t;

typedef struct {
    pending_packet_t entries[PENDING_QUEUE_CAPACITY];
    size_t count;
} pending_queue_t;

void pending_queue_init(pending_queue_t *queue);

int pending_queue_enqueue(
    pending_queue_t *queue,
    const packet_t *packet,
    uint32_t next_hop_ip,
    uint16_t vlan_id,
    uint64_t now_ms
);

bool pending_queue_contains(
    const pending_queue_t *queue,
    uint32_t next_hop_ip,
    uint16_t vlan_id
);

bool pending_queue_pop_match(
    pending_queue_t *queue,
    uint32_t next_hop_ip,
    uint16_t vlan_id,
    pending_packet_t *result
);

size_t pending_queue_age(
    pending_queue_t *queue,
    uint64_t now_ms,
    uint64_t timeout_ms
);

size_t pending_queue_count(const pending_queue_t *queue);

#endif
