/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-211 proc PHY cap registry handlers. */
#ifndef HOST_RTW_DEBUG_PROC_PHY_CAP_REGISTRY_TYPES_H
#define HOST_RTW_DEBUG_PROC_PHY_CAP_REGISTRY_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

struct registry_priv {
	u8 stbc_cap;
	u8 rx_stbc;
	u8 ldpc_cap;
	u8 beamform_cap;
};

struct _adapter {
	struct registry_priv registrypriv;
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

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_proc_emit_stbc_cap(void *sel, void *adapter);
long rtw_rust_debug_proc_set_stbc_cap(void *adapter, const void *buffer,
				      size_t count);
void rtw_rust_debug_proc_emit_rx_stbc(void *sel, void *adapter);
long rtw_rust_debug_proc_set_rx_stbc(void *adapter, const void *buffer,
				     size_t count);
void rtw_rust_debug_proc_emit_ldpc_cap(void *sel, void *adapter);
long rtw_rust_debug_proc_set_ldpc_cap(void *adapter, const void *buffer,
				      size_t count);
#ifdef CONFIG_BEAMFORMING
void rtw_rust_debug_proc_emit_txbf_cap(void *sel, void *adapter);
long rtw_rust_debug_proc_set_txbf_cap(void *adapter, const void *buffer,
				      size_t count);
#endif

int proc_get_stbc_cap(struct seq_file *m, void *v);
ssize_t proc_set_stbc_cap(void *file, const void *buffer, size_t count,
			  long *pos, void *data);
int proc_get_rx_stbc(struct seq_file *m, void *v);
ssize_t proc_set_rx_stbc(void *file, const void *buffer, size_t count,
			 long *pos, void *data);
int proc_get_ldpc_cap(struct seq_file *m, void *v);
ssize_t proc_set_ldpc_cap(void *file, const void *buffer, size_t count,
			  long *pos, void *data);
#ifdef CONFIG_BEAMFORMING
int proc_get_txbf_cap(struct seq_file *m, void *v);
ssize_t proc_set_txbf_cap(void *file, const void *buffer, size_t count,
			  long *pos, void *data);
#endif

#endif /* HOST_RTW_DEBUG_PROC_PHY_CAP_REGISTRY_TYPES_H */
