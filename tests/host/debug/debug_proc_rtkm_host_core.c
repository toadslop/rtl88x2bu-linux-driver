// SPDX-License-Identifier: GPL-2.0
#include "host_rtw_debug_proc_rtkm_types.h"

u8 rtw_rtkm_get_nr_recv_skb(void)
{
	return HOST_RTKM_NR_PREALLOC_RECV_SKB;
}

u16 rtw_rtkm_get_buff_size(void)
{
	return HOST_RTKM_RECVBUF_SZ;
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *s = m;

	if (!s)
		return NULL;
	return rtw_netdev_priv((struct net_device *)s->private);
}

void rtw_rust_debug_proc_emit_rtkm_info(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct recv_priv *precvpriv = &padapter->recvpriv;
	struct recv_buf *precvbuf;

	precvbuf = (struct recv_buf *)precvpriv->precv_buf;

	RTW_PRINT_SEL(sel, "============[RTKM Info]============\n");
	RTW_PRINT_SEL(sel, "MAX_RTKM_NR_PREALLOC_RECV_SKB: %d\n", rtw_rtkm_get_nr_recv_skb());
	RTW_PRINT_SEL(sel, "MAX_RTKM_RECVBUF_SZ: %d\n", rtw_rtkm_get_buff_size());

	RTW_PRINT_SEL(sel, "============[Driver Info]============\n");
	RTW_PRINT_SEL(sel, "NR_PREALLOC_RECV_SKB: %d\n", NR_PREALLOC_RECV_SKB);
	RTW_PRINT_SEL(sel, "MAX_RECVBUF_SZ: %d\n", precvbuf->alloc_sz);
}
