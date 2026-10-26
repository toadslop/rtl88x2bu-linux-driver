// SPDX-License-Identifier: GPL-2.0
/* Kernel FFI for rust/rtw_recv_bh_dbg.rs (W3-171). */

#include <drv_types.h>

#if defined(CONFIG_RUST)

u32 rtw_rust_recv_bh_stage(struct recv_priv *recv)
{
#if DBG_RX_BH_TRACKING
	return recv->rx_bh_stage;
#else
	return 0;
#endif
}

u32 rtw_rust_recv_bh_buf_dq_cnt(struct recv_priv *recv)
{
#if DBG_RX_BH_TRACKING
	return recv->rx_bh_buf_dq_cnt;
#else
	return 0;
#endif
}

void *rtw_rust_recv_bh_lbuf(struct recv_priv *recv)
{
#if DBG_RX_BH_TRACKING
	return recv->rx_bh_lbuf;
#else
	return NULL;
#endif
}

void *rtw_rust_recv_bh_cbuf(struct recv_priv *recv)
{
#if DBG_RX_BH_TRACKING
	return recv->rx_bh_cbuf;
#else
	return NULL;
#endif
}

u32 rtw_rust_recv_bh_cbuf_dlen(struct recv_priv *recv)
{
#if DBG_RX_BH_TRACKING
	return recv->rx_bh_cbuf_dlen;
#else
	return 0;
#endif
}

u32 rtw_rust_recv_bh_cbuf_pos(struct recv_priv *recv)
{
#if DBG_RX_BH_TRACKING
	return recv->rx_bh_cbuf_pos;
#else
	return 0;
#endif
}

void *rtw_rust_recv_bh_cframe(struct recv_priv *recv)
{
#if DBG_RX_BH_TRACKING
	return recv->rx_bh_cframe;
#else
	return NULL;
#endif
}

void rtw_rust_recv_bh_print_sel(void *sel, const char *line)
{
#if DBG_RX_BH_TRACKING
	if (line)
		RTW_PRINT_SEL(sel, "%s", line);
#else
	(void)sel;
	(void)line;
#endif
}

#endif /* CONFIG_RUST */
