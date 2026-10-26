// SPDX-License-Identifier: GPL-2.0
/* Host C oracle for W3-172 rtw_fw_ps_state (8822B path). */

#include <string.h>
#include "host_pwrctrl_fw_ps.h"

#define RTW_INFO(fmt, ...) ((void)0)

static u16 g_fw_ps_state;

void host_pwrctrl_fw_ps_set_hal_state(u16 state)
{
	g_fw_ps_state = state;
}

void _enter_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

void _exit_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

static void rtw_hal_get_hwreg_fw_ps(PADAPTER padapter, u16 *fw_ps_state)
{
	(void)padapter;
	*fw_ps_state = g_fw_ps_state;
}

int rtw_fw_ps_state(PADAPTER padapter)
{
	struct dvobj_priv *psdpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	int ret = _FAIL;
	u16 fw_ps_state = 0;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct registry_priv *registry_par = &padapter->registrypriv;

	if (registry_par->check_fw_ps != 1)
		return _SUCCESS;

	_enter_pwrlock(&pwrpriv->check_32k_lock);

	if (padapter->bSurpriseRemoved || padapter->bDriverStopped) {
		RTW_INFO("%s: cannot run\n", __func__);
		goto exit_fw_ps_state;
	}

	rtw_hal_get_hwreg_fw_ps(padapter, &fw_ps_state);
	if ((fw_ps_state & BIT_LPS_STATUS) == 0)
		ret = _SUCCESS;
	else {
		pdbgpriv->dbg_poll_fail_cnt++;
		RTW_INFO("%s: fw_ps_state=%04x\n", __func__, fw_ps_state);
	}

exit_fw_ps_state:
	_exit_pwrlock(&pwrpriv->check_32k_lock);
	return ret;
}
