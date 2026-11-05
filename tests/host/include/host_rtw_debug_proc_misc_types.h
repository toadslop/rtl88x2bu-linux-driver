/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-214 proc malloc cnt + best channel handlers. */
#ifndef HOST_RTW_DEBUG_PROC_MISC_TYPES_H
#define HOST_RTW_DEBUG_PROC_MISC_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

#define MAX_CHANNEL_NUM 42

typedef int atomic_t;

#define atomic_read(v) (*(v))

extern atomic_t _malloc_cnt;
extern atomic_t _malloc_size;

typedef struct _RT_CHANNEL_INFO {
	u8 ChannelNum;
	u8 flags;
#ifdef CONFIG_FIND_BEST_CHANNEL
	u32 rx_count;
#endif
} RT_CHANNEL_INFO;

struct rf_ctl_t {
	u8 max_chan_nums;
	RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
};

struct _adapter {
	struct rf_ctl_t rf_ctl;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define adapter_to_rfctl(adapter) (&(adapter)->rf_ctl)

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

#define RTW_INFO(fmt, ...) ((void)0)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

static inline void rtw_warn_on(int cond)
{
	(void)cond;
}

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_proc_emit_malloc_cnt(void *sel);
long rtw_rust_debug_proc_set_best_channel(void *adapter, const void *buffer,
					  size_t count);
void rtw_rust_debug_proc_emit_best_channel(void *sel, void *adapter);

int proc_get_malloc_cnt(struct seq_file *m, void *v);
int proc_get_best_channel(struct seq_file *m, void *v);
ssize_t proc_set_best_channel(void *file, const void *buffer, size_t count,
			      long *pos, void *data);

#endif /* HOST_RTW_DEBUG_PROC_MISC_TYPES_H */
