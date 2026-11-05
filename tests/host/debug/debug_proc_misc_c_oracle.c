// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-214 proc malloc cnt + best channel (from core/rtw_debug_rest.c). */

#include "host_rtw_debug_proc_misc_types.h"

atomic_t _malloc_cnt;
atomic_t _malloc_size;

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

#ifdef DBG_MEMORY_LEAK
int proc_get_malloc_cnt(struct seq_file *m, void *v)
{
	(void)v;

	RTW_PRINT_SEL(m, "_malloc_cnt=%d\n", atomic_read(&_malloc_cnt));
	RTW_PRINT_SEL(m, "_malloc_size=%d\n", atomic_read(&_malloc_size));

	return 0;
}
#endif /* DBG_MEMORY_LEAK */

#ifdef CONFIG_FIND_BEST_CHANNEL
int proc_get_best_channel(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	u32 i, best_channel_24G = 1, best_channel_5G = 36, index_24G = 0, index_5G = 0;

	(void)v;

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

		RTW_PRINT_SEL(m, "The rx cnt of channel %3d = %d\n",
			      rfctl->channel_set[i].ChannelNum,
			      rfctl->channel_set[i].rx_count);
	}

	RTW_PRINT_SEL(m, "best_channel_5G = %d\n", best_channel_5G);
	RTW_PRINT_SEL(m, "best_channel_24G = %d\n", best_channel_24G);

	return 0;
}

ssize_t proc_set_best_channel(void *file, const void *buffer, size_t count,
			      long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	char tmp[32];

	(void)file;
	(void)pos;

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

	return (ssize_t)count;
}
#endif /* CONFIG_FIND_BEST_CHANNEL */
