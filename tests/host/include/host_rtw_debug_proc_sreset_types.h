/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-216 proc rf4ce_state + sreset handlers. */
#ifndef HOST_RTW_DEBUG_PROC_SRESET_TYPES_H
#define HOST_RTW_DEBUG_PROC_SRESET_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef int32_t s32;
typedef uint32_t u32;
typedef unsigned long long u64;
typedef long ssize_t;

#define _TRUE 1
#define _FALSE 0

enum {
	SRESET_TGP_NULL = 0,
	SRESET_TGP_XMIT_STATUS = 1,
	SRESET_TGP_LINK_STATUS = 2,
	SRESET_TGP_INFO = 99,
};

struct sreset_priv {
	u8 dbg_sreset_ctrl;
	u64 self_dect_tx_cnt;
	u64 self_dect_rx_cnt;
	u64 self_dect_fw_cnt;
	u64 tx_dma_status_cnt;
	u64 rx_dma_status_cnt;
	u8 self_dect_case;
};

struct debug_priv {
	u32 dbg_sreset_cnt;
};

struct dvobj_priv {
	struct debug_priv drv_dbg;
};

struct hal_data {
	struct sreset_priv srestpriv;
};

struct _adapter {
	struct dvobj_priv *dvobj;
	struct hal_data hal;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define GET_HAL_DATA(padapter) (&(padapter)->hal)

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

void rtw_btcoex_SetRf4ceLinkState(struct _adapter *padapter, u8 state);
u8 rtw_btcoex_GetRf4ceLinkState(struct _adapter *padapter);
void rtw_hal_sreset_reset(struct _adapter *padapter);
void sreset_set_trigger_point(struct _adapter *padapter, s32 tgp);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_proc_emit_rf4ce_state(void *sel, void *adapter);
long rtw_rust_debug_proc_set_rf4ce_state(void *adapter, const void *buffer,
					 size_t count);
void rtw_rust_debug_proc_emit_sreset(void *sel, void *adapter);
long rtw_rust_debug_proc_set_sreset(void *adapter, const void *buffer,
				    size_t count);

int proc_get_rf4ce_state(struct seq_file *m, void *v);
ssize_t proc_set_rf4ce_state(void *file, const void *buffer, size_t count,
			     long *pos, void *data);
int proc_get_sreset(struct seq_file *m, void *v);
ssize_t proc_set_sreset(void *file, const void *buffer, size_t count,
			long *pos, void *data);

void host_debug_proc_sreset_reset_mocks(void);
int host_debug_proc_sreset_reset_calls(void);
s32 host_debug_proc_sreset_last_trigger(void);
void host_debug_proc_sreset_set_rf4ce(u8 state);

#endif /* HOST_RTW_DEBUG_PROC_SRESET_TYPES_H */
