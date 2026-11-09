#ifndef RING_H
#define RING_H

#include <rte_ring.h>

/* One ring per traffic class */
#define TC_HI   0   /* voice, control plane */
#define TC_MED  1   /* interactive */
#define TC_LO   2   /* bulk data */
#define TC_BG   3   /* background */
#define TC_MAX  4

#define RING_RX_NAME     "ring_rx"
#define RING_TC_HI_NAME  "ring_tc_hi"
#define RING_TC_MED_NAME "ring_tc_med"
#define RING_TC_LO_NAME  "ring_tc_lo"
#define RING_TC_BG_NAME  "ring_tc_bg"
#define RING_SIZE        2048   /* must be power of 2 */

/* Global ring handles — set in main, read by all cores */
extern struct rte_ring *g_rx_ring;
extern struct rte_ring *g_tc_ring[TC_MAX];

int  rings_init(void);
void rings_free(void);

#endif
