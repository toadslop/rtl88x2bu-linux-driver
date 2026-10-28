/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-182 rtw_wow_lps_level_decide. */

#include "host_pwrctrl_wow_lps.h"

void host_pwrctrl_wow_lps_read_state(PADAPTER adapter,
				     struct host_pwrctrl_wow_lps_state *out)
{
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(adapter_to_dvobj(adapter));

	out->lps_level = pwrpriv->lps_level;
	out->lps_level_bk = pwrpriv->lps_level_bk;
	out->lps_1t1r = pwrpriv->lps_1t1r;
	out->lps_1t1r_bk = pwrpriv->lps_1t1r_bk;
}

void rtw_wow_lps_level_decide(PADAPTER adapter, u8 wow_en)
{
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(adapter_to_dvobj(adapter));

	if (wow_en) {
		pwrpriv->lps_level_bk = pwrpriv->lps_level;
		pwrpriv->lps_level = pwrpriv->wowlan_lps_level;
		pwrpriv->lps_1t1r_bk = pwrpriv->lps_1t1r;
		pwrpriv->lps_1t1r = pwrpriv->wowlan_lps_1t1r;
	} else {
		pwrpriv->lps_level = pwrpriv->lps_level_bk;
		pwrpriv->lps_1t1r = pwrpriv->lps_1t1r_bk;
	}
}
