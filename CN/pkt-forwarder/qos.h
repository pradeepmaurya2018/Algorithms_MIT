#ifndef QOS_H
#define QOS_H

#include <stdint.h>

/*
 * Token bucket per traffic class.
 * Tokens accumulate at `rate_bytes_per_ns` up to `burst_bytes`.
 * A packet is forwarded only if enough tokens are available.
 */
typedef struct {
    uint64_t tokens;           /* current token count (in bytes) */
    uint64_t burst_bytes;      /* max burst size */
    uint64_t rate_bytes_per_ns;/* refill rate */
    uint64_t last_refill_tsc;  /* TSC timestamp of last refill */
} token_bucket_t;

/* QoS policy — rates in Mbps, burst in bytes */
typedef struct {
    uint32_t rate_mbps;
    uint32_t burst_bytes;
} qos_policy_t;

/* Scheduler priorities — higher index = lower priority */
extern const qos_policy_t g_qos_policy[4];

void  tb_init(token_bucket_t *tb, uint32_t rate_mbps, uint32_t burst_bytes);
int   tb_consume(token_bucket_t *tb, uint32_t pkt_bytes);
int   qos_loop(void *arg);

#endif
