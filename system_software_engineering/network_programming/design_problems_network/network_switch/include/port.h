#ifndef PORT_H
#define PORT_H

#include <stdint.h>

typedef struct {
    uint16_t port_id;
    uint8_t enabled;
} switch_port_t;

#endif
