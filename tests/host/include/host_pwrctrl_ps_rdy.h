/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_PS_RDY_H
#define HOST_PWRCTRL_PS_RDY_H

#include "host_pwrctrl_types.h"

#define WIFI_UNDER_SURVEY 0x00000800
#define WIFI_UNDER_LINKING 0x00000080
#define WIFI_UNDER_WPS 0x00000100
#define WIFI_AP_STATE 0x00000010
#define WIFI_MESH_STATE 0x00000200
#define WIFI_MONITOR_STATE 0x80000000
#define WIFI_ADHOC_MASTER_STATE 0x00000040
#define WIFI_ADHOC_STATE 0x00000020

#define dot11AuthAlgrthm_Open 0
#define dot11AuthAlgrthm_8021X 2

typedef s32 sint;

struct security_priv {
	u32 dot11AuthAlgrthm;
	u8 binstallGrpkey;
};

struct tdls_info {
	u8 link_established;
};

struct rf_ctl_t {
	u8 radar_detect_enabled;
};

struct pwrctrl_priv_ps_rdy {
	u8 bInSuspend;
	systime lps_deny_time;
	u8 wowlan_mode;
	u8 wowlan_ap_mode;
};

struct _adapter_ps_rdy {
	struct mlme_priv mlmepriv;
	struct pwrctrl_priv_ps_rdy pwrctrlpriv;
	struct security_priv securitypriv;
	struct tdls_info tdlsinfo;
	struct rf_ctl_t *rfctl;
};

typedef struct _adapter_ps_rdy _adapter_ps_rdy;
typedef _adapter_ps_rdy *PADAPTER_PS_RDY;

#define adapter_to_pwrctl_ps_rdy(a) (&(a)->pwrctrlpriv)

void host_pwrctrl_ps_rdy_set_time(systime t);
void host_pwrctrl_ps_rdy_set_scan_deny(u8 v);
void host_pwrctrl_ps_rdy_set_roch(u8 v);
void host_pwrctrl_ps_rdy_set_pwr_mgmt(u8 v);

u8 PS_RDY_CHECK(PADAPTER_PS_RDY padapter);

#endif /* HOST_PWRCTRL_PS_RDY_H */
