/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_PM_H
#define HOST_PWRCTRL_PM_H

#include <stdint.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef int32_t s32;

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 0
#define _FAIL (-1)

#define PS_MODE_ACTIVE 0
#define PS_MODE_MAX 8
#define PS_MODE_NUM 9

#define IPS_NONE 0
#define IPS_NORMAL 1
#define IPS_LEVEL_2 2

#define LPS_NORMAL 0
#define LPS_LEVEL_MAX 3

struct pwrctrl_priv_pm {
	u8 ips_mode_req;
	u8 power_mgnt;
	u8 bLeisurePs;
	u8 LpsIdleCount;
	u8 lps_level;
#ifdef CONFIG_WOWLAN
	u8 wowlan_power_mgmt;
	u8 wowlan_lps_level;
#ifdef CONFIG_LPS_1T1R
	u8 wowlan_lps_1t1r;
#endif
#endif
};

struct adapter_pm {
	struct pwrctrl_priv_pm pwrctrlpriv;
	u8 bSurpriseRemoved;
};

typedef struct adapter_pm _adapter_pm;
typedef struct adapter_pm *PADAPTER_PM;

#define adapter_to_pwrctl_pm(a) (&(a)->pwrctrlpriv)

struct host_pwrctrl_pm_trace {
	int leave_all_ps;
	int ips_mode_req_calls;
	u8 last_ips_mode_req;
	int pwr_wakeup_calls;
};

void host_pwrctrl_pm_reset_trace(void);
struct host_pwrctrl_pm_trace *host_pwrctrl_pm_get_trace(void);
void host_pwrctrl_pm_set_pwr_wakeup_result(s32 result);

int rtw_pm_set_ips(PADAPTER_PM padapter, u8 mode);
int rtw_pm_set_lps(PADAPTER_PM padapter, u8 mode);
int rtw_pm_set_lps_level(PADAPTER_PM padapter, u8 level);

#ifdef CONFIG_WOWLAN
int rtw_pm_set_wow_lps(PADAPTER_PM padapter, u8 mode);
int rtw_pm_set_wow_lps_level(PADAPTER_PM padapter, u8 level);
#ifdef CONFIG_LPS_1T1R
int rtw_pm_set_wow_lps_1t1r(PADAPTER_PM padapter, u8 en);
#endif
#endif

#endif /* HOST_PWRCTRL_PM_H */
