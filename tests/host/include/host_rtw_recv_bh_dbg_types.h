/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-171 dump_rx_bh_tk (DBG_RX_BH_TRACKING). */
#ifndef HOST_RTW_RECV_BH_DBG_TYPES_H
#define HOST_RTW_RECV_BH_DBG_TYPES_H

#include <stdio.h>
#include <stdint.h>

typedef uint32_t u32;

struct recv_priv {
	u32 rx_bh_stage;
	u32 rx_bh_buf_dq_cnt;
	void *rx_bh_lbuf;
	void *rx_bh_cbuf;
	void *rx_bh_cbuf_data;
	u32 rx_bh_cbuf_dlen;
	u32 rx_bh_cbuf_pos;
	void *rx_bh_cframe;
};

#define RTW_PRINT_SEL(sel, fmt, ...) fprintf((FILE *)(sel), fmt, ##__VA_ARGS__)

void dump_rx_bh_tk(void *sel, struct recv_priv *recv);

#endif /* HOST_RTW_RECV_BH_DBG_TYPES_H */
