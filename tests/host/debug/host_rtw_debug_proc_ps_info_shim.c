// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-219 L2 — mirrors debug_proc_ps_info_c_oracle.c output/logic. */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_ps_info_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

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

void rtw_rust_debug_proc_emit_new_bcn_max(void *sel)
{
	RTW_PRINT_SEL(sel, "%d", new_bcn_max);
}

ssize_t rtw_rust_debug_proc_set_new_bcn_max(const void *buffer, size_t count)
{
	char tmp[32];

	if (count < 1)
		return -1;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -1;
	}

	if (buffer && !copy_from_user(tmp, buffer, count))
		sscanf(tmp, "%d ", &new_bcn_max);

	return (ssize_t)count;
}

static int rtw_pm_set_lps_shim(void *adapter, u8 mode)
{
	struct _adapter *a = adapter;

	a->pwrctrlpriv.power_mgnt = mode;
	return 0;
}

static int rtw_pm_set_ips_shim(void *adapter, u8 mode)
{
	struct _adapter *a = adapter;

	a->pwrctrlpriv.ips_mode_req = mode;
	return 0;
}

#if defined(CONFIG_WOWLAN)
static int rtw_pm_set_wow_lps_shim(void *adapter, u8 mode)
{
	struct _adapter *a = adapter;

	a->registrypriv.wow_power_mgnt = mode;
	return 0;
}
#endif

void rtw_rust_debug_proc_emit_ps_info(void *sel, void *padapter)
{
	struct pwrctrl_priv *pwrpriv;

	if (!padapter)
		return;

	pwrpriv = adapter_to_pwrctl(padapter);
	u8 ips_mode = pwrpriv->ips_mode_req;
	u8 lps_mode = pwrpriv->power_mgnt;
	u8 lps_level = pwrpriv->lps_level;
	const char *str;

	RTW_PRINT_SEL(sel, "======Power Saving Info:======\n");
	RTW_PRINT_SEL(sel, "*IPS:\n");

	if (ips_mode == IPS_NORMAL) {
#ifdef CONFIG_FWLPS_IN_IPS
		str = "FW_LPS_IN_IPS";
#else
		str = "Card Disable";
#endif
	}
	else if (ips_mode == IPS_NONE)
		str = "NO IPS";
	else if (ips_mode == IPS_LEVEL_2)
		str = "IPS_LEVEL_2";
	else
		str = "invalid ips_mode";

	RTW_PRINT_SEL(sel, " IPS mode: %s\n", str);
	RTW_PRINT_SEL(sel, " IPS enter count:%d, IPS leave count:%d\n",
		      (int)pwrpriv->ips_enter_cnts,
		      (int)pwrpriv->ips_leave_cnts);
	RTW_PRINT_SEL(sel, "------------------------------\n");
	RTW_PRINT_SEL(sel, "*LPS:\n");

	if (lps_mode == PS_MODE_ACTIVE)
		str = "NO LPS";
	else if (lps_mode == PS_MODE_MIN)
		str = "MIN";
	else if (lps_mode == PS_MODE_MAX)
		str = "MAX";
	else if (lps_mode == PS_MODE_DTIM)
		str = "DTIM";
	else {
		RTW_PRINT_SEL(sel, " LPS mode: %d\n", lps_mode);
		str = NULL;
	}
	if (str)
		RTW_PRINT_SEL(sel, " LPS mode: %s\n", str);

	if (pwrpriv->dtim != 0)
		RTW_PRINT_SEL(sel, " DTIM: %d\n", pwrpriv->dtim);
	RTW_PRINT_SEL(sel, " LPS enter count:%d, LPS leave count:%d\n",
		      (int)pwrpriv->lps_enter_cnts,
		      (int)pwrpriv->lps_leave_cnts);

	if (lps_level == LPS_LCLK)
		str = "LPS_LCLK";
	else if (lps_level == LPS_PG)
		str = "LPS_PG";
	else
		str = "LPS_NORMAL";
	RTW_PRINT_SEL(sel, " LPS level: %s\n", str);

	RTW_PRINT_SEL(sel, "=============================\n");
}

ssize_t rtw_rust_debug_proc_set_ps_info(void *adapter, const void *buffer,
					size_t count)
{
	char tmp[8];
	int num = 0;
	int mode = 0;
	int en = 0;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -1;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%d %d", &mode, &en);
	if (num > 2) {
		RTW_ERR("invalid parameter\n");
		goto exit;
	}

	if (num == 1 && mode == 0) {
		rtw_pm_set_lps_shim(adapter,
				   ((struct _adapter *)adapter)->registrypriv
					   .power_mgnt);
		rtw_pm_set_ips_shim(adapter,
				   ((struct _adapter *)adapter)->registrypriv
					   .ips_mode);
#if defined(CONFIG_WOWLAN)
		rtw_pm_set_wow_lps_shim(adapter,
					((struct _adapter *)adapter)->registrypriv
					    .wow_power_mgnt);
#endif
		goto exit;
	}

	if (mode == 1) {
		if (rtw_pm_set_lps_shim(adapter, (u8)en) != 0)
			RTW_ERR("invalid lps\n");
	} else if (mode == 2) {
		if (rtw_pm_set_ips_shim(adapter, (u8)en) != 0)
			RTW_ERR("invalid ips\n");
	} else
		RTW_ERR("invalid mode\n");

exit:
	return (ssize_t)count;
}

void rtw_rust_debug_proc_emit_wmmps_info(void *sel, void *padapter)
{
	struct registry_priv *pregpriv;

	if (!padapter)
		return;

	pregpriv = &((struct _adapter *)padapter)->registrypriv;
	const char *uapsd_max_sp_str;

	switch (pregpriv->uapsd_max_sp_len) {
	case 0:
		uapsd_max_sp_str = "NO_LIMIT";
		break;
	case 1:
		uapsd_max_sp_str = "TWO_MSDU";
		break;
	case 2:
		uapsd_max_sp_str = "FOUR_MSDU";
		break;
	case 3:
		uapsd_max_sp_str = "SIX_MSDU";
		break;
	default:
		uapsd_max_sp_str = "UNSPECIFIED";
		break;
	}

	RTW_PRINT_SEL(sel, "====== WMMPS_STA Info:======\n");
	RTW_PRINT_SEL(sel, "uapsd_max_sp_len=0x%02x (%s)\n",
		      pregpriv->uapsd_max_sp_len, uapsd_max_sp_str);
	RTW_PRINT_SEL(sel, "uapsd_ac_enable=0x%02x\n", pregpriv->uapsd_ac_enable);
	RTW_PRINT_SEL(sel, "BIT0 - AC_VO UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_VO) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(sel, "BIT1 - AC_VI UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_VI) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(sel, "BIT2 - AC_BK UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_BK) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(sel, "BIT3 - AC_BE UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_BE) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(sel, "============================\n");
}

ssize_t rtw_rust_debug_proc_set_wmmps_info(void *adapter, const void *buffer,
					   size_t count)
{
	struct registry_priv *pregpriv =
		&((struct _adapter *)adapter)->registrypriv;
	char tmp[32];
	u8 uapsd_ac_setting;
	u8 uapsd_max_sp_len_setting;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -1;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhu %hhx", &uapsd_max_sp_len_setting,
				 &uapsd_ac_setting);

		if (num >= 1)
			pregpriv->uapsd_max_sp_len = uapsd_max_sp_len_setting;
		if (num >= 2)
			pregpriv->uapsd_ac_enable = uapsd_ac_setting;
	}

	return (ssize_t)count;
}
