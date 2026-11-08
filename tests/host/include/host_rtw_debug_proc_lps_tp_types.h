/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-225 proc LPS check-by-TP. */
#ifndef HOST_RTW_DEBUG_PROC_LPS_TP_TYPES_H
#define HOST_RTW_DEBUG_PROC_LPS_TP_TYPES_H

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

struct pwrctrl_priv {
	u8 lps_chk_by_tp;
	u16 lps_tx_tp_th;
	u16 lps_rx_tp_th;
	u16 lps_bi_tp_th;
	int lps_chk_cnt_th;
	int lps_chk_cnt;
	u32 lps_tx_pkts;
	u32 lps_rx_pkts;
};

struct _adapter {
	struct pwrctrl_priv pwrctrlpriv;
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

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

#ifndef HOST_LPS_TP_L1_REF
void rtw_rust_debug_proc_emit_lps_chk_tp(void *sel, struct _adapter *adapter);
ssize_t rtw_rust_debug_proc_set_lps_chk_tp(struct _adapter *adapter,
					   const void *buffer, size_t count);
#endif /* HOST_LPS_TP_L1_REF */

#endif /* HOST_RTW_DEBUG_PROC_LPS_TP_TYPES_H */
