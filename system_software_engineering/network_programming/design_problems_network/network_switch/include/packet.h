#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>
#include <stdint.h>

#define MAX_PACKET_SIZE 2048

typedef struct {
    uint8_t data[MAX_PACKET_SIZE];
    size_t len;
    uint16_t ingress_port;
    uint16_t egress_port;
} packet_t;

#endif
