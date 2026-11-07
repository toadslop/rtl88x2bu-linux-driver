/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-219 proc ps_info / wmmps / new_bcn_max. */
#ifndef HOST_RTW_DEBUG_PROC_PS_INFO_TYPES_H
#define HOST_RTW_DEBUG_PROC_PS_INFO_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

/* Mirrors include/rtw_pwrctrl.h enums used by the proc formatters. */
enum {
	IPS_NONE = 0,
	IPS_NORMAL,
	IPS_LEVEL_2,
};

enum {
	PS_MODE_ACTIVE = 0,
	PS_MODE_MIN,
	PS_MODE_MAX,
	PS_MODE_DTIM,
};

enum {
	LPS_NORMAL = 0,
	LPS_LCLK,
	LPS_PG,
};

/* include/rtw_qos.h */
#define DRV_CFG_UAPSD_VO (1u << 0)
#define DRV_CFG_UAPSD_VI (1u << 1)
#define DRV_CFG_UAPSD_BK (1u << 2)
#define DRV_CFG_UAPSD_BE (1u << 3)

struct registry_priv {
	u8 power_mgnt;
	u8 ips_mode;
	u8 wow_power_mgnt;
	u8 uapsd_max_sp_len;
	u8 uapsd_ac_enable;
};

struct pwrctrl_priv {
	u8 ips_mode_req;
	u8 power_mgnt;
	u8 lps_level;
	u32 ips_enter_cnts;
	u32 ips_leave_cnts;
	u32 lps_enter_cnts;
	u32 lps_leave_cnts;
	u8 dtim;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct pwrctrl_priv pwrctrlpriv;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

extern int new_bcn_max;

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

#define RTW_INFO(fmt, ...) ((void)0)
#define RTW_ERR(fmt, ...) ((void)0)

static inline void rtw_warn_on(int cond)
{
	(void)cond;
}

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

#define adapter_to_pwrctl(adapter) (&((struct _adapter *)(adapter))->pwrctrlpriv)

/* C reference (debug_proc_ps_info_c_oracle.c) */
int debug_proc_ps_info_c_ref_get_new_bcn_max(struct seq_file *m, void *v);
ssize_t debug_proc_ps_info_c_ref_set_new_bcn_max(void *file, const void *buffer,
						 size_t count, long *pos,
						 void *data);
int debug_proc_ps_info_c_ref_get_ps_info(struct seq_file *m, void *v);
ssize_t debug_proc_ps_info_c_ref_set_ps_info(void *file, const void *buffer,
					     size_t count, long *pos,
					     void *data);
int debug_proc_ps_info_c_ref_get_wmmps_info(struct seq_file *m, void *v);
ssize_t debug_proc_ps_info_c_ref_set_wmmps_info(void *file,
					       const void *buffer, size_t count,
					       long *pos, void *data);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_proc_emit_new_bcn_max(void *sel);
ssize_t rtw_rust_debug_proc_set_new_bcn_max(const void *buffer, size_t count);

void rtw_rust_debug_proc_emit_ps_info(void *sel, void *padapter);
ssize_t rtw_rust_debug_proc_set_ps_info(void *adapter, const void *buffer,
					size_t count);

void rtw_rust_debug_proc_emit_wmmps_info(void *sel, void *padapter);
ssize_t rtw_rust_debug_proc_set_wmmps_info(void *adapter, const void *buffer,
					   size_t count);

#endif /* HOST_RTW_DEBUG_PROC_PS_INFO_TYPES_H */
