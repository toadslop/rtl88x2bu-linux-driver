/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RTW_DEBUG_PROC_AMSDU_TYPES_H
#define HOST_RTW_DEBUG_PROC_AMSDU_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef long ssize_t;

struct xmit_priv {
	u32 amsdu_debug_set_timer;
	u32 amsdu_debug_timeout;
	u32 amsdu_debug_coalesce_one;
	u32 amsdu_debug_coalesce_two;
};

struct _adapter {
	u8 tx_amsdu;
	u16 tx_amsdu_rate;
	struct xmit_priv xmitpriv;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

int proc_get_tx_amsdu(struct seq_file *m, void *v);
ssize_t proc_set_tx_amsdu(void *file, const void *buffer, size_t count,
			  long *pos, void *data);
int proc_get_tx_amsdu_rate(struct seq_file *m, void *v);
ssize_t proc_set_tx_amsdu_rate(void *file, const void *buffer, size_t count,
			       long *pos, void *data);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);
void rtw_rust_debug_print_sel(void *sel, const char *line);
void rtw_rust_debug_proc_emit_tx_amsdu(void *sel, void *adapter);
long rtw_rust_debug_proc_set_tx_amsdu(void *adapter, const void *buffer,
				       size_t count);
void rtw_rust_debug_proc_emit_tx_amsdu_rate(void *sel, void *adapter);
long rtw_rust_debug_proc_set_tx_amsdu_rate(void *adapter, const void *buffer,
					     size_t count);

#endif /* HOST_RTW_DEBUG_PROC_AMSDU_TYPES_H */
