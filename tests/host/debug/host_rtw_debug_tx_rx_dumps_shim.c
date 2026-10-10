// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-134 C reference object (L1 only). */

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef struct _adapter _adapter;
typedef struct sta_info sta_info;
typedef struct dvobj_priv dvobj_priv;

void rtw_sink_rtp_seq_dbg(_adapter *adapter, u8 *ehdr_pos)
{
	(void)adapter;
	(void)ehdr_pos;
}

void sta_rx_reorder_ctl_dump(void *sel, struct sta_info *sta)
{
	(void)sel;
	(void)sta;
}

void dump_tx_rate_bmp(void *sel, struct dvobj_priv *dvobj)
{
	(void)sel;
	(void)dvobj;
}
