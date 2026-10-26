// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-171 dump_rx_bh_tk (from core/rtw_recv.c). */

#include "host_rtw_recv_bh_dbg_types.h"

void dump_rx_bh_tk(void *sel, struct recv_priv *recv)
{
	RTW_PRINT_SEL(sel,
		      "[RXBHTK]s:%u, buf_dqc:%u, lbuf:%p, cbuf:%p, dlen:%u, pos:%u, cframe:%p\n",
		      recv->rx_bh_stage, recv->rx_bh_buf_dq_cnt, recv->rx_bh_lbuf,
		      recv->rx_bh_cbuf, recv->rx_bh_cbuf_dlen, recv->rx_bh_cbuf_pos,
		      recv->rx_bh_cframe);
}
