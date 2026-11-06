// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-217 (kernel + L2 Rust oracle). */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_wow_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

static int hex2num_i(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int hex2byte_i(const char *hex)
{
	int a, b;

	a = hex2num_i(*hex++);
	if (a < 0)
		return -1;
	b = hex2num_i(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}

int rtw_wowlan_parser_pattern_cmd(u8 *input_in, char *pattern, int *pattern_len,
				  char *bit_mask)
{
	char *cp;
	int pos = 0, mask_pos = 0;
	int hb;
	char *input = (char *)input_in;

	cp = strchr(input, '=');
	if (cp) {
		*cp = 0;
		cp++;
		input = cp;
	}

	cp = strchr(input, '\n');
	if (cp)
		*cp = 0;

	while (input) {
		cp = strsep(&input, ":");
		if (!cp)
			break;

		if (bit_mask &&
		    (strcmp(cp, "-") == 0 || strcmp(cp, "xx") == 0 ||
		     strcmp(cp, "--") == 0)) {
			/* skip byte */
		} else {
			if (strlen(cp) != 2)
				goto error;

			hb = hex2byte_i(cp);
			if (hb < 0)
				goto error;

			pattern[pos] = (char)(u8)hb;
			mask_pos = pos / 8;
			if (bit_mask)
				bit_mask[mask_pos] |= (char)(1 << (pos % 8));
		}
		pos++;
	}

	*pattern_len = pos;
	return _TRUE;
error:
	return _FALSE;
}

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

void rtw_rust_debug_proc_emit_pattern_info(void *sel, void *adapter)
{
	struct pwrctrl_priv *pwrpriv =
		adapter_to_pwrctl((struct _adapter *)adapter);
	u8 val8;
	char str_1[128];
	char *p_str;
	int i, j, k;
	int len, max_len, total;

	p_str = str_1;
	max_len = (int)sizeof(str_1);

	total = pwrpriv->wowlan_pattern_idx;

	rtw_set_default_pattern((struct _adapter *)adapter);

	RTW_PRINT_SEL(sel, "\n======[Pattern Info.]======\n");
	RTW_PRINT_SEL(sel, "pattern number: %d\n", total);
	RTW_PRINT_SEL(sel, "support default patterns: %c\n",
		      (pwrpriv->default_patterns_en) ? 'Y' : 'N');

	for (k = 0; k < total; k++) {
		RTW_PRINT_SEL(sel, "\npattern idx: %d\n", k);
		RTW_PRINT_SEL(sel, "pattern content:\n");

		p_str = str_1;
		max_len = (int)sizeof(str_1);
		for (i = 0; i < MAX_WKFM_PATTERN_SIZE / 8; i++) {
			memset(p_str, 0, (size_t)max_len);
			len = 0;
			for (j = 0; j < 8; j++) {
				val8 = (u8)pwrpriv->patterns[k].content[i * 8 + j];
				len += snprintf(p_str + len, (size_t)(max_len - len),
						"%02x ", val8);
			}
			RTW_PRINT_SEL(sel, "%s\n", p_str);
		}
		RTW_PRINT_SEL(sel, "\npattern mask:\n");
		for (i = 0; i < MAX_WKFM_SIZE / 8; i++) {
			memset(p_str, 0, (size_t)max_len);
			len = 0;
			for (j = 0; j < 8; j++) {
				val8 = (u8)pwrpriv->patterns[k].mask[i * 8 + j];
				len += snprintf(p_str + len, (size_t)(max_len - len),
						"%02x ", val8);
			}
			RTW_PRINT_SEL(sel, "%s\n", p_str);
		}

		RTW_PRINT_SEL(sel, "\npriv_pattern_len:\n");
		RTW_PRINT_SEL(sel, "pattern_len: %d\n", pwrpriv->patterns[k].len);
		RTW_PRINT_SEL(sel, "*****************\n");
	}
}

long rtw_rust_debug_proc_set_pattern_info(void *adapter, const void *buffer,
					  size_t count)
{
	struct pwrctrl_priv *pwrpriv =
		adapter_to_pwrctl((struct _adapter *)adapter);
	struct wowlan_ioctl_param poidparam;
	char tmp[MAX_WKFM_PATTERN_STR_LEN + 1] = {0};
	int ret = 0;
	u8 index = 0;

	poidparam.subcode = 0;

	if (count < 1)
		return -1;

	if (count >= sizeof(tmp))
		return -1;

	if (pwrpriv->wowlan_pattern_idx >= MAX_WKFM_CAM_NUM)
		return -12;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		if (strncmp(tmp, "clean", 5) == 0) {
			poidparam.subcode = WOWLAN_PATTERN_CLEAN;
			rtw_hal_set_hwreg((struct _adapter *)adapter,
					  HW_VAR_WOWLAN, (u8 *)&poidparam);
		} else {
			index = pwrpriv->wowlan_pattern_idx;
			ret = rtw_wowlan_parser_pattern_cmd(
				(u8 *)tmp,
				pwrpriv->patterns[index].content,
				&pwrpriv->patterns[index].len,
				pwrpriv->patterns[index].mask);
			if (ret == _TRUE)
				pwrpriv->wowlan_pattern_idx++;
		}
	} else {
		return -1;
	}

	return (long)count;
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
