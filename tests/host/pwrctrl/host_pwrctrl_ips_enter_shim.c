// SPDX-License-Identifier: GPL-2.0
#include "host_pwrctrl_ips.h"

static void rtw_ips_pwr_down(PADAPTER padapter)
{
	(void)padapter;
}

static void rtw_btcoex_IpsNotify(PADAPTER a, u8 m)
{
	(void)a;
	(void)m;
}

void _ips_enter(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	pwrpriv->bips_processing = _TRUE;
	pwrpriv->ips_mode = pwrpriv->ips_mode_req;
	pwrpriv->ips_enter_cnts++;
	if (rf_off == pwrpriv->change_rfpwrstate) {
		pwrpriv->bpower_saving = _TRUE;
		if (pwrpriv->ips_mode == IPS_LEVEL_2)
			pwrpriv->bkeepfwalive = _TRUE;
		rtw_ips_pwr_down(padapter);
		pwrpriv->rf_pwrstate = rf_off;
	}
	pwrpriv->bips_processing = _FALSE;
}

void ips_enter(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	rtw_btcoex_IpsNotify(padapter, pwrpriv->ips_mode_req);
	_enter_pwrlock(&pwrpriv->lock);
	_ips_enter(padapter);
	_exit_pwrlock(&pwrpriv->lock);
}
