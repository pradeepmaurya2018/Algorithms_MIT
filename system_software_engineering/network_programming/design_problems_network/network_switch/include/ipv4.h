#ifndef IPV4_H
#define IPV4_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"

#define IPV4_MIN_HEADER_LEN 20
#define IPV4_MAX_HEADER_LEN 60

typedef struct {
    uint8_t version;
    uint8_t ihl;
    uint8_t ttl;
    uint8_t protocol;

    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint16_t header_checksum;

    uint32_t src_ip;
    uint32_t dst_ip;

    size_t header_length;
    size_t payload_offset;
    size_t payload_length;
} ipv4_header_t;

int ipv4_parse(
    const packet_t *packet,
    size_t offset,
    ipv4_header_t *header
);

bool ipv4_checksum_valid(
    const packet_t *packet,
    size_t offset,
    const ipv4_header_t *header
);

int ipv4_decrement_ttl(
    packet_t *packet,
    size_t offset,
    ipv4_header_t *header
);

void ipv4_print(const ipv4_header_t *header);

#endif
