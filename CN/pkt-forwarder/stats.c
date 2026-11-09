#include <stdio.h>
#include <string.h>
#include <rte_hash.h>
#include <rte_jhash.h>
#include <rte_malloc.h>
#include <rte_log.h>
#include <rte_telemetry.h>
#include "stats.h"
#include "ring.h"

#define RTE_LOGTYPE_STATS RTE_LOGTYPE_USER5

tc_drops_t g_tc_drops[4] = {0};

static struct rte_hash *g_flow_ht = NULL;
static flow_stats_t    *g_flow_stats = NULL;

int stats_init(void)
{
    struct rte_hash_parameters hp = {
        .name       = STATS_HT_NAME,
        .entries    = MAX_FLOWS,
        .key_len    = sizeof(flow_key_t),
        .hash_func  = rte_jhash,
        .socket_id  = rte_socket_id(),
        .extra_flag = RTE_HASH_EXTRA_FLAGS_RW_CONCURRENCY_LF,
    };

    g_flow_ht = rte_hash_create(&hp);
    if (!g_flow_ht) {
        RTE_LOG(ERR, STATS, "Failed to create flow hash table\n");
        return -1;
    }

    g_flow_stats = rte_zmalloc("flow_stats",
                               sizeof(flow_stats_t) * MAX_FLOWS, 64);
    if (!g_flow_stats) {
        RTE_LOG(ERR, STATS, "Failed to alloc flow stats\n");
        return -1;
    }

    RTE_LOG(INFO, STATS, "Flow table: %u entries\n", MAX_FLOWS);
    return 0;
}

void stats_free(void)
{
    rte_hash_free(g_flow_ht);
    rte_free(g_flow_stats);
}

flow_stats_t *stats_get_or_create(const flow_key_t *key, uint8_t tc)
{
    int32_t idx;

    idx = rte_hash_lookup(g_flow_ht, key);
    if (idx >= 0)
        return &g_flow_stats[idx];

    /* New flow */
    idx = rte_hash_add_key(g_flow_ht, key);
    if (unlikely(idx < 0))
        return NULL;    /* table full */

    memset(&g_flow_stats[idx], 0, sizeof(flow_stats_t));
    g_flow_stats[idx].tc = tc;
    return &g_flow_stats[idx];
}

void stats_print_top(int n)
{
    const void *key;
    void       *data;
    uint32_t    iter = 0;
    int         count = 0;

    printf("\n  %-16s %-16s %5s %8s %12s %10s  TC\n",
           "src_ip", "dst_ip", "proto", "pkts", "bytes", "drops");

    while (rte_hash_iterate(g_flow_ht, &key, &data, &iter) >= 0
           && count++ < n) {
        const flow_key_t  *fk = key;
        int32_t idx = rte_hash_lookup(g_flow_ht, fk);
        if (idx < 0) continue;
        flow_stats_t *fs = &g_flow_stats[idx];

        uint8_t *s = (uint8_t *)&fk->src_ip;
        uint8_t *d = (uint8_t *)&fk->dst_ip;

        printf("  %u.%u.%u.%u %*s %u.%u.%u.%u %*s %5u %8"PRIu64
               " %12"PRIu64" %10"PRIu64"  %u\n",
               s[0],s[1],s[2],s[3],
               (int)(16-snprintf(NULL,0,"%u.%u.%u.%u",s[0],s[1],s[2],s[3])),"",
               d[0],d[1],d[2],d[3],
               (int)(16-snprintf(NULL,0,"%u.%u.%u.%u",d[0],d[1],d[2],d[3])),"",
               fk->proto,
               fs->rx_pkts, fs->rx_bytes, fs->drop_pkts, fs->tc);
    }
    printf("  TC drops — HI:%"PRIu64" MED:%"PRIu64
           " LO:%"PRIu64" BG:%"PRIu64"\n",
           g_tc_drops[0].tb_drops, g_tc_drops[1].tb_drops,
           g_tc_drops[2].tb_drops, g_tc_drops[3].tb_drops);
}

/* ------------------------------------------------------------------ */
/* rte_telemetry callbacks                                             */
/* ------------------------------------------------------------------ */

static int telem_tc_drops(const char *cmd __rte_unused,
                           const char *params __rte_unused,
                           struct rte_tel_data *d)
{
    rte_tel_data_start_dict(d);
    rte_tel_data_add_dict_uint(d, "hi_drops",  g_tc_drops[0].tb_drops);
    rte_tel_data_add_dict_uint(d, "med_drops", g_tc_drops[1].tb_drops);
    rte_tel_data_add_dict_uint(d, "lo_drops",  g_tc_drops[2].tb_drops);
    rte_tel_data_add_dict_uint(d, "bg_drops",  g_tc_drops[3].tb_drops);
    return 0;
}

static int telem_flow_count(const char *cmd __rte_unused,
                             const char *params __rte_unused,
                             struct rte_tel_data *d)
{
    rte_tel_data_start_dict(d);
    rte_tel_data_add_dict_uint(d, "active_flows",
                               rte_hash_count(g_flow_ht));
    return 0;
}

int telemetry_init(void)
{
    rte_telemetry_register_cmd("/pf/tc_drops",  telem_tc_drops,
        "Per-TC token bucket drop counters");
    rte_telemetry_register_cmd("/pf/flow_count", telem_flow_count,
        "Number of active flows in hash table");
    RTE_LOG(INFO, STATS, "Telemetry registered: /pf/tc_drops /pf/flow_count\n");
    return 0;
}
