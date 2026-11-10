/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-228 fw_offload + W3-229 fw_tbtt_rpt proc handlers. */
#ifndef HOST_RTW_DEBUG_PROC_FW_OFFLOAD_TYPES_H
#define HOST_RTW_DEBUG_PROC_FW_OFFLOAD_TYPES_H

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

struct hal_data {
	u8 RegIQKFWOffload;
	u8 ch_switch_offload;
};

struct dvobj_priv {
	struct _adapter *padapters[1];
	u8 fw_bcn_offload;
	u8 vap_tbtt_rpt_map;
};

struct _adapter {
	struct dvobj_priv dvobj_storage;
	struct dvobj_priv *dvobj;
	struct hal_data hal_data;
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
#define rtw_warn_on(x) ((void)(x))

#define GET_HAL_DATA(a) (&((struct _adapter *)(a))->hal_data)
#define GET_PRIMARY_ADAPTER(a)                                               \
	(((struct _adapter *)(a))->dvobj->padapters[0])
#define adapter_to_dvobj(a) (((struct _adapter *)(a))->dvobj)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

void rtw_run_in_thread_cmd(void *adapter, void *func, void *ctx);
unsigned host_fw_offload_thread_cmd_count(void);
void host_fw_offload_reset_thread_cmd_count(void);

void rtw_hal_set_fw_ap_bcn_offload_cmd(void *adapter, int fw_bcn_en,
				       u8 tbtt_rpt_map);
unsigned host_fw_tbtt_hal_cmd_count(void);
void host_fw_tbtt_reset_hal_cmd_count(void);

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_proc_emit_fw_offload(void *sel, void *adapter);
long rtw_rust_debug_proc_set_fw_offload(void *adapter, const void *buffer,
					size_t count);

int proc_get_fw_offload(struct seq_file *m, void *v);
ssize_t proc_set_fw_offload(void *file, const void *buffer, size_t count,
			    long *pos, void *data);

int proc_get_fw_tbtt_rpt(struct seq_file *m, void *v);
ssize_t proc_set_fw_tbtt_rpt(void *file, const void *buffer, size_t count,
			     long *pos, void *data);

void rtw_rust_debug_proc_emit_fw_tbtt_rpt(void *sel, void *adapter);
long rtw_rust_debug_proc_set_fw_tbtt_rpt(void *adapter, const void *buffer,
				       size_t count);

#endif /* HOST_RTW_DEBUG_PROC_FW_OFFLOAD_TYPES_H */
