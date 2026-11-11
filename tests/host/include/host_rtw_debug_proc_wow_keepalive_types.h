/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-235 proc wow_keep_alive_info dump. */
#ifndef HOST_RTW_DEBUG_PROC_WOW_KEEPALIVE_TYPES_H
#define HOST_RTW_DEBUG_PROC_WOW_KEEPALIVE_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define WLAN_MAX_KEEP_ALIVE_IE_LEN 256

struct pwrctrl_priv {
	u8 wowlan_keep_alive_mode;
	u8 keep_alive_pattern_loc;
	u16 wowlan_keep_alive_period;
	u8 wowlan_keep_alive_retry_interval;
	u8 wowlan_keep_alive_retry_counter;
	u8 keep_alive_pattern[WLAN_MAX_KEEP_ALIVE_IE_LEN];
	u32 keep_alive_pattern_len;
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

#define adapter_to_pwrctl(adapter) (&(adapter)->pwrctrlpriv)

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

void *rtw_rust_debug_proc_adapter(void *m);

void rtw_rust_debug_proc_emit_wow_keep_alive_info(void *sel, void *adapter);

int proc_dump_wow_keep_alive_info(struct seq_file *m, void *v);

#endif /* HOST_RTW_DEBUG_PROC_WOW_KEEPALIVE_TYPES_H */
