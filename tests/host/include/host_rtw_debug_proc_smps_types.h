/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-226 proc static SMPS get/set. */
#ifndef HOST_RTW_DEBUG_PROC_SMPS_TYPES_H
#define HOST_RTW_DEBUG_PROC_SMPS_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef long ssize_t;

struct mlme_ext_priv {
	u8 ssmps_en;
	u16 ssmps_tx_tp_th;
	u16 ssmps_rx_tp_th;
#ifdef DBG_STATIC_SMPS
	u8 ssmps_test;
	u8 ssmps_test_en;
#endif
};

struct _adapter {
	struct mlme_ext_priv mlmeextpriv;
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

#define RTW_INFO(fmt, ...) ((void)0)

static inline void rtw_warn_on(int cond)
{
	(void)cond;
}

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_proc_emit_smps(void *sel, void *adapter);
long rtw_rust_debug_proc_set_smps(void *adapter, const void *buffer,
				  size_t count);

/* Stub for future enable-toggle workqueue paths (not used by proc_set_smps today). */
void rtw_ssmps_wk_cmd_stub(void *adapter, void *sta, u8 smps, u8 enqueue);

int proc_get_smps(struct seq_file *m, void *v);
ssize_t proc_set_smps(void *file, const void *buffer, size_t count,
		      long *pos, void *data);

#endif /* HOST_RTW_DEBUG_PROC_SMPS_TYPES_H */
