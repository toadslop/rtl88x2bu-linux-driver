/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_WOW_LPS_H
#define HOST_PWRCTRL_WOW_LPS_H

#include <stdint.h>

typedef uint8_t u8;

#define _TRUE 1
#define _FALSE 0

struct pwrctrl_priv {
	u8 wowlan_lps_level;
	u8 wowlan_lps_1t1r;
	u8 lps_level_bk;
	u8 lps_level;
	u8 lps_1t1r_bk;
	u8 lps_1t1r;
};

struct dvobj_priv {
	struct pwrctrl_priv pwrctl_priv;
};

struct adapter {
	struct dvobj_priv *dvobj;
};

typedef struct adapter _adapter;
typedef struct adapter *PADAPTER;

#define adapter_to_dvobj(a) ((a)->dvobj)
#define dvobj_to_pwrctl(d) (&(d)->pwrctl_priv)

void rtw_wow_lps_level_decide(PADAPTER adapter, u8 wow_en);

struct host_pwrctrl_wow_lps_state {
	u8 lps_level;
	u8 lps_level_bk;
	u8 lps_1t1r;
	u8 lps_1t1r_bk;
};

void host_pwrctrl_wow_lps_read_state(PADAPTER adapter,
				     struct host_pwrctrl_wow_lps_state *out);

#endif /* HOST_PWRCTRL_WOW_LPS_H */
