// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-217 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_wow_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size)
{
	if (count < 1)
		return -1;
	if (count > tmp_size)
		return -1;
	if (buffer)
		memcpy(tmp, buffer, count);
	if (count < tmp_size)
		tmp[count] = '\0';
	return (ssize_t)count;
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

void rtw_rust_debug_proc_emit_wow_enable(void *sel, void *adapter)
{
	struct registry_priv *registry_pair =
		&((struct _adapter *)adapter)->registrypriv;

	RTW_PRINT_SEL(sel, "wow - %s\n",
		      (registry_pair->wowlan_enable) ? "enable" : "disable");
}

long rtw_rust_debug_proc_set_wow_enable(void *adapter, const void *buffer,
					size_t count)
{
	struct registry_priv *registry_pair =
		&((struct _adapter *)adapter)->registrypriv;
	char tmp[8];
	int num = 0;
	int mode = 0;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		num = sscanf(tmp, "%d", &mode);
	} else {
		return -1;
	}

	if (num != 1)
		return -22;

	if (mode == 1 || mode == 0) {
		registry_pair->wowlan_enable = (u8)mode;
	} else {
		return -22;
	}

	return (long)count;
}

void rtw_rust_debug_proc_emit_wakeup_event(void *sel, void *adapter)
{
	struct registry_priv *registry_par =
		&((struct _adapter *)adapter)->registrypriv;

	RTW_PRINT_SEL(sel, "wakeup event: %#02x\n", registry_par->wakeup_event);
}

long rtw_rust_debug_proc_set_wakeup_event(void *adapter, const void *buffer,
					  size_t count)
{
	struct pwrctrl_priv *pwrctrlpriv =
		adapter_to_pwrctl((struct _adapter *)adapter);
	struct registry_priv *registry_par =
		&((struct _adapter *)adapter)->registrypriv;
	u32 wakeup_event = 0;
	char tmp[8] = {0};
	int num = 0;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		num = sscanf(tmp, "%u", &wakeup_event);
	} else {
		return -1;
	}

	if (num == 1 && wakeup_event <= 0x0f) {
		registry_par->wakeup_event = (u8)wakeup_event;

		if (wakeup_event & BIT(1))
			pwrctrlpriv->default_patterns_en = _TRUE;
		else
			pwrctrlpriv->default_patterns_en = _FALSE;
	} else {
		return -22;
	}

	return (long)count;
}

void rtw_rust_debug_proc_emit_wakeup_reason(void *sel, void *adapter)
{
	struct pwrctrl_priv *pwrpriv =
		adapter_to_pwrctl((struct _adapter *)adapter);
	u8 val = pwrpriv->wowlan_last_wake_reason;

	RTW_PRINT_SEL(sel, "last wake reason: %#02x\n", val);
}

void rtw_rust_debug_proc_emit_wowlan_gpio_info(void *sel, void *adapter)
{
	struct pwrctrl_priv *pwrpriv =
		adapter_to_pwrctl((struct _adapter *)adapter);
	u8 gpio_index = pwrpriv->wowlan_gpio_index;
	u8 gpio_output_state = pwrpriv->wowlan_gpio_output_state;
	u8 val = pwrpriv->is_high_active;

	RTW_PRINT_SEL(sel, "wakeup_gpio_idx: %d\n", gpio_index);
	RTW_PRINT_SEL(sel, "current_gpio_output_state: %d\n", gpio_output_state);
	RTW_PRINT_SEL(sel, "high_active: %d\n", val);
}

long rtw_rust_debug_proc_set_wowlan_gpio_info(void *adapter, const void *buffer,
					      size_t count)
{
	struct pwrctrl_priv *pwrpriv =
		adapter_to_pwrctl((struct _adapter *)adapter);
	char tmp[32] = {0};
	int num = 0;
	u32 is_high_active = 0;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		num = sscanf(tmp, "%u", &is_high_active);

		if (num != 1)
			return (long)count;

		is_high_active = is_high_active == 0 ? 0 : 1;
		pwrpriv->is_high_active = (u8)is_high_active;
	}

	return (long)count;
}

/*
 * Stubs so `rustc … rtw_debug_proc_wow.rs` links before PR2 lands the full
 * oracle mirror (strong definitions replace these at link time in PR2).
 */
__attribute__((weak)) void rtw_rust_debug_proc_emit_pattern_info(void *sel,
								 void *adapter)
{
	(void)sel;
	(void)adapter;
}

__attribute__((weak)) long rtw_rust_debug_proc_set_pattern_info(
	void *adapter, const void *buffer, size_t count)
{
	(void)adapter;
	(void)buffer;
	(void)count;
	return -1;
}
