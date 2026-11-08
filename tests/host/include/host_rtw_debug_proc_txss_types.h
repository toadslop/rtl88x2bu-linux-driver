/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-224 proc TXSS by TP. */
#ifndef HOST_RTW_DEBUG_PROC_TXSS_TYPES_H
#define HOST_RTW_DEBUG_PROC_TXSS_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef long ssize_t;

#ifndef _TRUE
#define _TRUE 1
#endif
#ifndef _FALSE
#define _FALSE 0
#endif

#ifndef EFAULT
#define EFAULT 14
#endif
#ifndef EINVAL
#define EINVAL 22
#endif

struct mlme_ext_priv {
	u8 txss_ctrl_en;
	u16 txss_tp_th;
	u8 txss_tp_chk_cnt;
	unsigned char txss_1ss;
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

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

struct host_txss_dbg_log {
	unsigned dbg_ctrl_calls;
	u8 last_tx_1ss;
};

#ifndef HOST_TXSS_L1_REF
extern struct host_txss_dbg_log g_txss_dbg_log;

void host_debug_proc_txss_reset_log(void);
const struct host_txss_dbg_log *host_debug_proc_txss_get_log(void);

void dbg_ctrl_txss(struct _adapter *adapter, int tx_1ss);

void rtw_rust_debug_proc_emit_txss_tp(void *sel, struct _adapter *adapter);
ssize_t rtw_rust_debug_proc_set_txss_tp(struct _adapter *adapter,
					const void *buffer, size_t count);
void rtw_rust_debug_proc_emit_txss_ctrl(void *sel, struct _adapter *adapter);
ssize_t rtw_rust_debug_proc_set_txss_ctrl(struct _adapter *adapter,
					  const void *buffer, size_t count);
#endif /* HOST_TXSS_L1_REF */

#endif /* HOST_RTW_DEBUG_PROC_TXSS_TYPES_H */
