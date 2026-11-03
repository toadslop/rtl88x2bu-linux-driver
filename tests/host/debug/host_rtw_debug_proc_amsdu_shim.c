// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-207 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_amsdu_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size)
{
	if (count < 1)
		return -1;
	if (count > tmp_size)
		return -1;
	if (buffer)
		memcpy(tmp, buffer, count);
	if (count < tmp_size)
		tmp[count] = '\0';
	return (ssize_t)count;
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

void rtw_rust_debug_print_sel(void *sel, const char *line)
{
	struct seq_file *seq = sel;

	if (!line)
		return;
	if (seq && seq->out)
		fputs(line, seq->out);
}

void rtw_rust_debug_proc_emit_tx_amsdu(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	if (padapter) {
		RTW_PRINT_SEL(sel, "tx amsdu = %d\n", padapter->tx_amsdu);
		RTW_PRINT_SEL(sel, "amsdu set timer conut = %u\n",
			      pxmitpriv->amsdu_debug_set_timer);
		RTW_PRINT_SEL(sel, "amsdu  time out count = %u\n",
			      pxmitpriv->amsdu_debug_timeout);
		RTW_PRINT_SEL(sel, "amsdu coalesce one count = %u\n",
			      pxmitpriv->amsdu_debug_coalesce_one);
		RTW_PRINT_SEL(sel, "amsdu coalesce two count = %u\n",
			      pxmitpriv->amsdu_debug_coalesce_two);
	}
}

long rtw_rust_debug_proc_set_tx_amsdu(void *adapter, const void *buffer,
				      size_t count)
{
	struct _adapter *padapter = adapter;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	char tmp[32];
	u32 amsdu;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &amsdu) == 1 && padapter) {
			if (amsdu > 3)
				padapter->tx_amsdu = 0;
			else if (amsdu == 3) {
				pxmitpriv->amsdu_debug_set_timer = 0;
				pxmitpriv->amsdu_debug_timeout = 0;
				pxmitpriv->amsdu_debug_coalesce_one = 0;
				pxmitpriv->amsdu_debug_coalesce_two = 0;
			} else
				padapter->tx_amsdu = (u8)amsdu;
		}
	}
	return (long)count;
}

void rtw_rust_debug_proc_emit_tx_amsdu_rate(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;

	if (padapter)
		RTW_PRINT_SEL(sel, "tx amsdu rate = %d Mbps\n",
			      padapter->tx_amsdu_rate);
}

long rtw_rust_debug_proc_set_tx_amsdu_rate(void *adapter, const void *buffer,
					   size_t count)
{
	struct _adapter *padapter = adapter;
	char tmp[32];
	u32 amsdu_rate;

	if (count < 1)
		return -1;
	if (count > sizeof(tmp))
		return -1;
	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (sscanf(tmp, "%d ", &amsdu_rate) == 1 && padapter)
			padapter->tx_amsdu_rate = (u16)amsdu_rate;
	}
	return (long)count;
}
