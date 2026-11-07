// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-220 monitor / simple_config / xmit_block (kernel + L2 Rust). */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_monitor_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

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

void rtw_rust_debug_proc_emit_monitor(void *sel, void *adapter)
{
	struct seq_file *seq = sel;
	struct net_device *dev = seq ? seq->private : NULL;
	struct _adapter *padapter = adapter;

	if (!dev || !padapter)
		return;

	if (MLME_IS_MONITOR(padapter)) {
		RTW_PRINT_SEL(sel, "Monitor mode : Enable\n");
		RTW_PRINT_SEL(sel, "Device type  : %u\n", dev->type);
		RTW_PRINT_SEL(sel, "ch=%d, ch_offset=%d, bw=%d\n",
			      rtw_get_oper_ch(padapter),
			      rtw_get_oper_choffset(padapter),
			      rtw_get_oper_bw(padapter));
	} else {
		RTW_PRINT_SEL(sel, "Monitor mode : Disable\n");
	}
}

long rtw_rust_debug_proc_set_monitor(void *adapter, void *dev,
				     const void *buffer, size_t count)
{
	char tmp[32];
	struct _adapter *padapter = adapter;
	struct net_device *ndev = dev;
	u16 target_type;
	u8 target_ch, target_offset, target_bw;

	if (!padapter || !ndev)
		return -1;

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
			ndev->type = ARPHRD_IEEE80211_RADIOTAP;
			return (long)count;
		}

		num = sscanf(tmp, "%hhu %hhu %hhu", &target_ch, &target_offset,
			     &target_bw);
		if (num != 3)
			return (long)count;

		padapter->mlmeextpriv.cur_channel = target_ch;
		set_channel_bwmode(padapter, target_ch, target_offset, target_bw);
	}

	return (long)count;
}

void rtw_rust_debug_proc_emit_simple_config(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;

	if (!padapter)
		return;

	RTW_PRINT_SEL(sel, "RTW Simple Config : %s\n",
		      padapter->rtw_simple_config ? "Enable" : "Disable");
}

long rtw_rust_debug_proc_set_simple_config(void *adapter, const void *buffer,
					   size_t count)
{
	struct _adapter *padapter = adapter;
	char tmp[32];
	int ret;

	if (!padapter)
		return -1;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &ret);

		(void)num;
		padapter->rtw_simple_config = ret ? _TRUE : _FALSE;
	}

	return (long)count;
}

void rtw_rust_debug_proc_emit_xmit_block(void *sel, void *adapter)
{
	if (!adapter)
		return;
	dump_xmit_block(sel, adapter);
}

long rtw_rust_debug_proc_set_xmit_block(void *adapter, const void *buffer,
					size_t count)
{
	struct _adapter *padapter = adapter;
	char tmp[32];
	u8 xb_mode, xb_reason;

	if (!padapter)
		return -1;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx %hhx", &xb_mode, &xb_reason);

		if (num != 2)
			return (long)count;

		if (xb_mode == 0)
			rtw_set_xmit_block(padapter, xb_reason);
		else if (xb_mode == 1)
			rtw_clr_xmit_block(padapter, xb_reason);
	}

	return (long)count;
}
