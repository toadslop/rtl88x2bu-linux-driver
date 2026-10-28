/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-181 rtw_exec_lps + rtw_lps_rfon_ctrl (no LCLK/PG/LPS_ACK). */

#include "host_pwrctrl_lps_exec.h"

#include <string.h>

static struct host_pwrctrl_lps_exec_trace g_trace;

static void record_hwreg(u8 var, u8 val)
{
	if (g_trace.hwreg_count >= 8)
		return;
	g_trace.hwreg[g_trace.hwreg_count].var = var;
	g_trace.hwreg[g_trace.hwreg_count].val = val;
	g_trace.hwreg_count++;
}

static void host_hal_set_hwreg(PADAPTER padapter, u8 var, u8 *pval)
{
	(void)padapter;
	record_hwreg(var, *pval);
}

void host_pwrctrl_lps_exec_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
}

struct host_pwrctrl_lps_exec_trace *host_pwrctrl_lps_exec_get_trace(void)
{
	return &g_trace;
}

void rtw_exec_lps(_adapter *padapter, u8 ps_mode)
{
	if (ps_mode == PS_MODE_ACTIVE) {
		host_hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE, &ps_mode);
		host_hal_set_hwreg(padapter, HOST_HW_VAR_LPS_STATE_CHK, &ps_mode);
	} else {
		if (MLME_IS_ASOC(padapter))
			host_hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE, &ps_mode);
	}
}

void rtw_lps_rfon_ctrl(_adapter *padapter, u8 rfon_ctrl)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	if (!pwrpriv->bFwCurrentInPSMode || pwrpriv->pwr_mode == PS_MODE_ACTIVE)
		return;

	if (rfon_ctrl == rf_on) {
		host_hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL,
				   &rfon_ctrl);
		host_hal_set_hwreg(padapter, HOST_HW_VAR_LPS_RFON_CHK, &rfon_ctrl);
	} else {
		if (MLME_IS_ASOC(padapter))
			host_hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL,
					   &rfon_ctrl);
	}
}
