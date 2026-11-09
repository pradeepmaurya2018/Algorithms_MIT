#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include <rte_hash.h>
#include <rte_jhash.h>

/* 5-tuple key — packed for hash efficiency */
typedef struct __attribute__((packed)) {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t  proto;
} flow_key_t;

/* Per-flow counters */
typedef struct {
    uint64_t rx_pkts;
    uint64_t rx_bytes;
    uint64_t drop_pkts;
    uint8_t  tc;        /* traffic class assigned */
} flow_stats_t;

/* Per-TC drop counters (updated by QoS core) */
typedef struct {
    uint64_t tb_drops;    /* token bucket drops */
    uint64_t ring_drops;  /* ring full drops */
} tc_drops_t;

#define MAX_FLOWS       65536
#define STATS_HT_NAME   "flow_stats_ht"

extern tc_drops_t g_tc_drops[4];

int          stats_init(void);
void         stats_free(void);
flow_stats_t *stats_get_or_create(const flow_key_t *key, uint8_t tc);
void         stats_print_top(int n);

/* Telemetry registration */
int telemetry_init(void);

#endif
