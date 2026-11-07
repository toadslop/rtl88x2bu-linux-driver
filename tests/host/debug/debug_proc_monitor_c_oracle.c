// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-220 monitor / simple_config / xmit_block (host L2 + L1 ref). */

#include "host_rtw_debug_proc_monitor_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

int proc_get_monitor(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;

	if (MLME_IS_MONITOR(padapter)) {
		RTW_PRINT_SEL(m, "Monitor mode : Enable\n");
		RTW_PRINT_SEL(m, "Device type  : %u\n", dev->type);
		RTW_PRINT_SEL(m, "ch=%d, ch_offset=%d, bw=%d\n",
			      rtw_get_oper_ch(padapter),
			      rtw_get_oper_choffset(padapter),
			      rtw_get_oper_bw(padapter));
	} else {
		RTW_PRINT_SEL(m, "Monitor mode : Disable\n");
	}

	return 0;
}

ssize_t proc_set_monitor(void *file, const void *buffer, size_t count,
			 long *pos, void *data)
{
	char tmp[32];
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	u16 target_type;
	u8 target_ch, target_offset, target_bw;

	(void)file;
	(void)pos;

	if (count < 3)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = 0;

		num = sscanf(tmp, "type %hu", &target_type);
		if ((num == 1) &&
		    ((target_type != ARPHRD_IEEE80211) &&
		     (target_type != ARPHRD_IEEE80211_RADIOTAP))) {
			dev->type = ARPHRD_IEEE80211_RADIOTAP;
			return (ssize_t)count;
		}

		num = sscanf(tmp, "%hhu %hhu %hhu", &target_ch, &target_offset,
			     &target_bw);
		if (num != 3)
			return (ssize_t)count;

		padapter->mlmeextpriv.cur_channel = target_ch;
		set_channel_bwmode(padapter, target_ch, target_offset, target_bw);
	}

	return (ssize_t)count;
}

#ifdef RTW_SIMPLE_CONFIG
int proc_get_simple_config(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;

	RTW_PRINT_SEL(m, "RTW Simple Config : %s\n",
		      padapter->rtw_simple_config ? "Enable" : "Disable");

	return 0;
}

ssize_t proc_set_simple_config(void *file, const void *buffer, size_t count,
			       long *pos, void *data)
{
	char tmp[32];
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	int ret;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &ret);

		(void)num;
		padapter->rtw_simple_config = ret ? _TRUE : _FALSE;
	}

	return (ssize_t)count;
}
#endif /* RTW_SIMPLE_CONFIG */

#ifdef DBG_XMIT_BLOCK
int proc_get_xmit_block(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;

	dump_xmit_block(m, padapter);

	return 0;
}

ssize_t proc_set_xmit_block(void *file, const void *buffer, size_t count,
			    long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	char tmp[32];
	u8 xb_mode, xb_reason;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx %hhx", &xb_mode, &xb_reason);

		if (num != 2)
			return (ssize_t)count;

		if (xb_mode == 0)
			rtw_set_xmit_block(padapter, xb_reason);
		else if (xb_mode == 1)
			rtw_clr_xmit_block(padapter, xb_reason);
	}

	return (ssize_t)count;
}
#endif /* DBG_XMIT_BLOCK */
