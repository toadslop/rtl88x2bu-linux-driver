// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-176 rx_bh_tk_set_* (from core/rtw_recv.c). */

#include "host_rtw_recv_bh_dbg_types.h"

void rtw_warn_on(int cond)
{
	(void)cond;
}

void rx_bh_tk_set_stage(struct recv_priv *recv, u32 s)
{
	recv->rx_bh_stage = s;
}

void rx_bh_tk_set_buf(struct recv_priv *recv, void *buf, void *data, u32 dlen)
{
	if (recv->rx_bh_cbuf)
		recv->rx_bh_lbuf = recv->rx_bh_cbuf;
	recv->rx_bh_cbuf = buf;
	if (buf) {
		recv->rx_bh_cbuf_data = data;
		recv->rx_bh_cbuf_dlen = dlen;
		recv->rx_bh_buf_dq_cnt++;
	} else {
		recv->rx_bh_cbuf_data = NULL;
		recv->rx_bh_cbuf_dlen = 0;
	}
}

void rx_bh_tk_set_buf_pos(struct recv_priv *recv, void *pos)
{
	if (recv->rx_bh_cbuf) {
		recv->rx_bh_cbuf_pos = (u32)((char *)pos - (char *)recv->rx_bh_cbuf_data);
	} else {
		rtw_warn_on(1);
		recv->rx_bh_cbuf_pos = 0;
	}
}

void rx_bh_tk_set_frame(struct recv_priv *recv, void *frame)
{
	recv->rx_bh_cframe = frame;
}
