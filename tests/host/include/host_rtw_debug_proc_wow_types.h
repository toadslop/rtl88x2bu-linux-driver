/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-217 WoWLAN registry / GPIO proc handlers. */
#ifndef HOST_RTW_DEBUG_PROC_WOW_TYPES_H
#define HOST_RTW_DEBUG_PROC_WOW_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

#ifndef _TRUE
#define _TRUE 1
#endif
#ifndef _FALSE
#define _FALSE 0
#endif

#define BIT(x) (1U << (x))

struct registry_priv {
	u8 wowlan_enable;
	u8 wakeup_event;
};

struct pwrctrl_priv {
	u8 wowlan_last_wake_reason;
	u8 default_patterns_en;
	u8 wowlan_gpio_index;
	u8 wowlan_gpio_output_state;
	u8 is_high_active;
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

#define adapter_to_pwrctl(adapter) (&(adapter)->pwrctrlpriv)

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

/* Stubs for GPIO setter side effects (host L2). */
static inline void rtw_wow_pattern_sw_reset(struct _adapter *adapter)
{
	(void)adapter;
}

static inline void rtw_ps_deny(struct _adapter *a, unsigned int reason)
{
	(void)a;
	(void)reason;
}

static inline void rtw_ps_deny_cancel(struct _adapter *a, unsigned int reason)
{
	(void)a;
	(void)reason;
}

static inline void LeaveAllPowerSaveModeDirect(struct _adapter *a)
{
	(void)a;
}

static inline void rtw_hal_switch_gpio_wl_ctrl(struct _adapter *a, u8 idx,
					       u8 en)
{
	(void)a;
	(void)idx;
	(void)en;
}

static inline void rtw_hal_set_output_gpio(struct _adapter *a, u8 idx,
					   u8 val)
{
	(void)a;
	(void)idx;
	(void)val;
}

static inline void rtw_hal_set_input_gpio(struct _adapter *a, u8 idx)
{
	(void)a;
	(void)idx;
}

#define GPIO_OUTPUT_LOW 0
#define PS_DENY_IOCTL 0U

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_proc_emit_wow_enable(void *sel, void *adapter);
long rtw_rust_debug_proc_set_wow_enable(void *adapter, const void *buffer,
					size_t count);
void rtw_rust_debug_proc_emit_wakeup_event(void *sel, void *adapter);
long rtw_rust_debug_proc_set_wakeup_event(void *adapter, const void *buffer,
					  size_t count);
void rtw_rust_debug_proc_emit_wakeup_reason(void *sel, void *adapter);
void rtw_rust_debug_proc_emit_wowlan_gpio_info(void *sel, void *adapter);
long rtw_rust_debug_proc_set_wowlan_gpio_info(void *adapter, const void *buffer,
					      size_t count);

int proc_get_wow_enable(struct seq_file *m, void *v);
ssize_t proc_set_wow_enable(void *file, const void *buffer, size_t count,
			    long *pos, void *data);
int proc_get_wakeup_event(struct seq_file *m, void *v);
ssize_t proc_set_wakeup_event(void *file, const void *buffer, size_t count,
			      long *pos, void *data);
int proc_get_wakeup_reason(struct seq_file *m, void *v);
int proc_get_wowlan_gpio_info(struct seq_file *m, void *v);
ssize_t proc_set_wowlan_gpio_info(void *file, const void *buffer, size_t count,
				  long *pos, void *data);

#endif /* HOST_RTW_DEBUG_PROC_WOW_TYPES_H */
