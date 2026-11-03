// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-207 proc tx AMSDU handlers (host L2). */

#include "host_rtw_debug_proc_amsdu_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

int proc_get_tx_amsdu(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	(void)v;

	if (padapter) {
		RTW_PRINT_SEL(m, "tx amsdu = %d\n", padapter->tx_amsdu);
		RTW_PRINT_SEL(m, "amsdu set timer conut = %u\n",
			      pxmitpriv->amsdu_debug_set_timer);
		RTW_PRINT_SEL(m, "amsdu  time out count = %u\n",
			      pxmitpriv->amsdu_debug_timeout);
		RTW_PRINT_SEL(m, "amsdu coalesce one count = %u\n",
			      pxmitpriv->amsdu_debug_coalesce_one);
		RTW_PRINT_SEL(m, "amsdu coalesce two count = %u\n",
			      pxmitpriv->amsdu_debug_coalesce_two);
	}

	return 0;
}

ssize_t proc_set_tx_amsdu(void *file, const void *buffer, size_t count,
			  long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	char tmp[32];
	u32 amsdu;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		int num = sscanf(tmp, "%d ", &amsdu);

		if (padapter && (num == 1)) {
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

	return (ssize_t)count;
}

int proc_get_tx_amsdu_rate(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;

	if (padapter)
		RTW_PRINT_SEL(m, "tx amsdu rate = %d Mbps\n",
			      padapter->tx_amsdu_rate);

	return 0;
}

ssize_t proc_set_tx_amsdu_rate(void *file, const void *buffer, size_t count,
			       long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	char tmp[32];
	u32 amsdu_rate;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		int num = sscanf(tmp, "%d ", &amsdu_rate);

		if (padapter && (num == 1))
			padapter->tx_amsdu_rate = (u16)amsdu_rate;
	}

	return (ssize_t)count;
}
