// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-219 (from core/rtw_debug_rest.c, host-trimmed). */

#include <stdio.h>

#include "host_rtw_debug_proc_ps_info_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

int rtw_pm_set_lps(void *adapter, u8 mode)
{
	struct _adapter *a = adapter;

	a->pwrctrlpriv.power_mgnt = mode;
	return 0;
}

int rtw_pm_set_ips(void *adapter, u8 mode)
{
	struct _adapter *a = adapter;

	a->pwrctrlpriv.ips_mode_req = mode;
	return 0;
}

#if defined(CONFIG_WOWLAN)
int rtw_pm_set_wow_lps(void *adapter, u8 mode)
{
	struct _adapter *a = adapter;

	a->registrypriv.wow_power_mgnt = mode;
	return 0;
}
#endif

int debug_proc_ps_info_c_ref_get_new_bcn_max(struct seq_file *m, void *v)
{
	(void)v;

	RTW_PRINT_SEL(m, "%d", new_bcn_max);
	return 0;
}

ssize_t debug_proc_ps_info_c_ref_set_new_bcn_max(void *file,
						 const void *buffer,
						 size_t count, long *pos,
						 void *data)
{
	char tmp[32];

	(void)file;
	(void)pos;
	(void)data;

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

int debug_proc_ps_info_c_ref_get_ps_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	u8 ips_mode = pwrpriv->ips_mode_req;
	u8 lps_mode = pwrpriv->power_mgnt;
	u8 lps_level = pwrpriv->lps_level;
	char lps_mode_buf[12];
	const char *str;

	(void)v;

	RTW_PRINT_SEL(m, "======Power Saving Info:======\n");
	RTW_PRINT_SEL(m, "*IPS:\n");

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

	RTW_PRINT_SEL(m, " IPS mode: %s\n", str);
	RTW_PRINT_SEL(m, " IPS enter count:%d, IPS leave count:%d\n",
		      (int)pwrpriv->ips_enter_cnts,
		      (int)pwrpriv->ips_leave_cnts);
	RTW_PRINT_SEL(m, "------------------------------\n");
	RTW_PRINT_SEL(m, "*LPS:\n");

	if (lps_mode == PS_MODE_ACTIVE)
		str = "NO LPS";
	else if (lps_mode == PS_MODE_MIN)
		str = "MIN";
	else if (lps_mode == PS_MODE_MAX)
		str = "MAX";
	else if (lps_mode == PS_MODE_DTIM)
		str = "DTIM";
	else {
		snprintf(lps_mode_buf, sizeof(lps_mode_buf), "%d", lps_mode);
		str = lps_mode_buf;
	}
	RTW_PRINT_SEL(m, " LPS mode: %s\n", str);

	if (pwrpriv->dtim != 0)
		RTW_PRINT_SEL(m, " DTIM: %d\n", pwrpriv->dtim);
	RTW_PRINT_SEL(m, " LPS enter count:%d, LPS leave count:%d\n",
		      (int)pwrpriv->lps_enter_cnts,
		      (int)pwrpriv->lps_leave_cnts);

	if (lps_level == LPS_LCLK)
		str = "LPS_LCLK";
	else if (lps_level == LPS_PG)
		str = "LPS_PG";
	else
		str = "LPS_NORMAL";
	RTW_PRINT_SEL(m, " LPS level: %s\n", str);

	RTW_PRINT_SEL(m, "=============================\n");
	return 0;
}

ssize_t debug_proc_ps_info_c_ref_set_ps_info(void *file,
					     const void *buffer, size_t count,
					     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	char tmp[8];
	int num = 0;
	int mode = 0;
	int en = 0;

	(void)file;
	(void)pos;

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
		rtw_pm_set_lps(adapter, adapter->registrypriv.power_mgnt);
		rtw_pm_set_ips(adapter, adapter->registrypriv.ips_mode);
#if defined(CONFIG_WOWLAN)
		rtw_pm_set_wow_lps(adapter, adapter->registrypriv.wow_power_mgnt);
#endif
		goto exit;
	}

	if (mode == 1) {
		if (rtw_pm_set_lps(adapter, (u8)en) != 0)
			RTW_ERR("invalid lps\n");
	} else if (mode == 2) {
		if (rtw_pm_set_ips(adapter, (u8)en) != 0)
			RTW_ERR("invalid ips\n");
	} else
		RTW_ERR("invalid mode\n");

exit:
	return (ssize_t)count;
}

int debug_proc_ps_info_c_ref_get_wmmps_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	const char *uapsd_max_sp_str;

	(void)v;

	if (!pregpriv)
		return 0;

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

	RTW_PRINT_SEL(m, "====== WMMPS_STA Info:======\n");
	RTW_PRINT_SEL(m, "uapsd_max_sp_len=0x%02x (%s)\n",
		      pregpriv->uapsd_max_sp_len, uapsd_max_sp_str);
	RTW_PRINT_SEL(m, "uapsd_ac_enable=0x%02x\n", pregpriv->uapsd_ac_enable);
	RTW_PRINT_SEL(m, "BIT0 - AC_VO UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_VO) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(m, "BIT1 - AC_VI UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_VI) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(m, "BIT2 - AC_BK UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_BK) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(m, "BIT3 - AC_BE UAPSD: %s\n",
		      (pregpriv->uapsd_ac_enable & DRV_CFG_UAPSD_BE) ? "Enabled" :
									 "Disabled");
	RTW_PRINT_SEL(m, "============================\n");

	return 0;
}

ssize_t debug_proc_ps_info_c_ref_set_wmmps_info(void *file,
					       const void *buffer, size_t count,
					       long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u8 uapsd_ac_setting;
	u8 uapsd_max_sp_len_setting;

	(void)file;
	(void)pos;

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
