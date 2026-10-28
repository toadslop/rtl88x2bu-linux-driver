// SPDX-License-Identifier: GPL-2.0
/* Host C oracle for W3-183 PS_RDY_CHECK — mirrors core/rtw_pwrctrl.c. */

#include "host_pwrctrl_ps_rdy.h"

static systime g_time;
static u8 g_scan_deny;
static u8 g_roch;
static u8 g_pwr_mgmt = _TRUE;

void host_pwrctrl_ps_rdy_set_time(systime t) { g_time = t; }
void host_pwrctrl_ps_rdy_set_scan_deny(u8 v) { g_scan_deny = v; }
void host_pwrctrl_ps_rdy_set_roch(u8 v) { g_roch = v; }
void host_pwrctrl_ps_rdy_set_pwr_mgmt(u8 v) { g_pwr_mgmt = v; }

static systime rtw_get_current_time(void) { return g_time; }
static int rtw_time_after(systime a, systime b) { return (s32)(a - b) > 0; }

static sint check_fwstate(struct mlme_priv *m, sint s)
{
	if (!s && !m->fw_state)
		return _TRUE;
	return (m->fw_state & (u32)s) ? _TRUE : _FALSE;
}

#define MLME_IS_AP(a) (((a)->mlmepriv.fw_state & WIFI_AP_STATE) != 0)
#define MLME_IS_MESH(a) (((a)->mlmepriv.fw_state & WIFI_MESH_STATE) != 0)
#define MLME_IS_MONITOR(a) (((a)->mlmepriv.fw_state & WIFI_MONITOR_STATE) != 0)

static u8 host_ps_rdy_scan_deny(_adapter_ps_rdy *padapter)
{
	(void)padapter;
	return g_scan_deny;
}

static u8 rtw_cfg80211_get_is_roch(_adapter_ps_rdy *padapter)
{
	(void)padapter;
	return g_roch;
}

static u8 rtw_cfg80211_pwr_mgmt(_adapter_ps_rdy *padapter)
{
	(void)padapter;
	return g_pwr_mgmt;
}

u8 PS_RDY_CHECK(PADAPTER_PS_RDY padapter)
{
	struct pwrctrl_priv_ps_rdy *pwrpriv = adapter_to_pwrctl_ps_rdy(padapter);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
	if (pwrpriv->bInSuspend == _TRUE && pwrpriv->wowlan_mode)
		return _TRUE;
	if (pwrpriv->bInSuspend == _TRUE && pwrpriv->wowlan_ap_mode)
		return _TRUE;
	if (pwrpriv->bInSuspend == _TRUE)
		return _FALSE;
#else
	if (pwrpriv->bInSuspend == _TRUE)
		return _FALSE;
#endif

	if (rtw_time_after(pwrpriv->lps_deny_time, rtw_get_current_time()))
		return _FALSE;

	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY)
	    || check_fwstate(pmlmepriv, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)
	    || MLME_IS_AP(padapter)
	    || MLME_IS_MESH(padapter)
	    || MLME_IS_MONITOR(padapter)
	    || check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE)
	    || rtw_cfg80211_get_is_roch(padapter) == _TRUE
	    || host_ps_rdy_scan_deny(padapter)
	    || padapter->tdlsinfo.link_established == _TRUE
	    || (padapter->rfctl && padapter->rfctl->radar_detect_enabled))
		return _FALSE;

	if (padapter->securitypriv.dot11AuthAlgrthm == dot11AuthAlgrthm_8021X
	    && padapter->securitypriv.binstallGrpkey == _FALSE)
		return _FALSE;

	if (!rtw_cfg80211_pwr_mgmt(padapter))
		return _FALSE;

	return _TRUE;
}
