// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-214 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_misc_types.h"

atomic_t _malloc_cnt;
atomic_t _malloc_size;

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

void rtw_rust_debug_proc_emit_malloc_cnt(void *sel)
{
	RTW_PRINT_SEL(sel, "_malloc_cnt=%d\n", atomic_read(&_malloc_cnt));
	RTW_PRINT_SEL(sel, "_malloc_size=%d\n", atomic_read(&_malloc_size));
}

void rtw_rust_debug_proc_emit_best_channel(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	u32 i, best_channel_24G = 1, best_channel_5G = 36, index_24G = 0, index_5G = 0;

	for (i = 0; i < rfctl->max_chan_nums && rfctl->channel_set[i].ChannelNum != 0;
	     i++) {
		if (rfctl->channel_set[i].ChannelNum == 1)
			index_24G = i;
		if (rfctl->channel_set[i].ChannelNum == 36)
			index_5G = i;
	}

	for (i = 0; i < rfctl->max_chan_nums && rfctl->channel_set[i].ChannelNum != 0;
	     i++) {
		if (rfctl->channel_set[i].ChannelNum == 6) {
			if (rfctl->channel_set[i].rx_count <
			    rfctl->channel_set[index_24G].rx_count) {
				index_24G = i;
				best_channel_24G = rfctl->channel_set[i].ChannelNum;
			}
		}

		if (rfctl->channel_set[i].ChannelNum >= 36 &&
		    rfctl->channel_set[i].ChannelNum < 140) {
			if (((rfctl->channel_set[i].ChannelNum - 36) % 8 == 0) &&
			    (rfctl->channel_set[i].rx_count <
			     rfctl->channel_set[index_5G].rx_count)) {
				index_5G = i;
				best_channel_5G = rfctl->channel_set[i].ChannelNum;
			}
		}

		if (rfctl->channel_set[i].ChannelNum >= 149 &&
		    rfctl->channel_set[i].ChannelNum < 165) {
			if (((rfctl->channel_set[i].ChannelNum - 149) % 8 == 0) &&
			    (rfctl->channel_set[i].rx_count <
			     rfctl->channel_set[index_5G].rx_count)) {
				index_5G = i;
				best_channel_5G = rfctl->channel_set[i].ChannelNum;
			}
		}

		RTW_PRINT_SEL(sel, "The rx cnt of channel %3d = %d\n",
			      rfctl->channel_set[i].ChannelNum,
			      rfctl->channel_set[i].rx_count);
	}

	RTW_PRINT_SEL(sel, "best_channel_5G = %d\n", best_channel_5G);
	RTW_PRINT_SEL(sel, "best_channel_24G = %d\n", best_channel_24G);
}

long rtw_rust_debug_proc_set_best_channel(void *adapter, const void *buffer,
					  size_t count)
{
	struct _adapter *padapter = adapter;
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	char tmp[32];

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u32 i;

		for (i = 0; i < rfctl->max_chan_nums &&
			    rfctl->channel_set[i].ChannelNum != 0;
		     i++)
			rfctl->channel_set[i].rx_count = 0;

		RTW_INFO("set %s\n", "Clean Best Channel Count");
	}

	return (long)count;
}
