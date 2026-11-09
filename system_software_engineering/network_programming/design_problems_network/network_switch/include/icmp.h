#ifndef ICMP_H
#define ICMP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"
#include "ipv4.h"

#define ICMP_PROTOCOL 1
#define ICMP_HEADER_LEN 8

#define ICMP_TYPE_ECHO_REPLY 0
#define ICMP_TYPE_DEST_UNREACHABLE 3
#define ICMP_TYPE_ECHO_REQUEST 8
#define ICMP_TYPE_TIME_EXCEEDED 11

#define ICMP_CODE_NET_UNREACHABLE 0
#define ICMP_CODE_HOST_UNREACHABLE 1
#define ICMP_CODE_PORT_UNREACHABLE 3
#define ICMP_CODE_TTL_EXCEEDED 0
#define ICMP_CODE_FRAGMENT_REASSEMBLY_TIMEOUT 1

typedef struct {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence;

    size_t payload_offset;
    size_t payload_length;
} icmp_header_t;

int icmp_parse(
    const packet_t *packet,
    const ipv4_header_t *ip,
    icmp_header_t *icmp
);

bool icmp_checksum_valid(
    const packet_t *packet,
    const icmp_header_t *icmp
);

int icmp_build_echo_reply(
    packet_t *reply,
    const packet_t *request,
    size_t ip_offset,
    const ipv4_header_t *ip
);

#endif
