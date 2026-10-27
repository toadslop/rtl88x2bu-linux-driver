/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-179 register *alive helpers. */

#include "host_pwrctrl_alive.h"

#include <string.h>

static struct host_pwrctrl_alive_trace g_trace;
static u8 g_set_rpwm_raises_cpwm;

void _enter_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

void _exit_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

static u8 rtw_set_rpwm(PADAPTER padapter, u8 pslv)
{
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(padapter);

	g_trace.set_rpwm_calls++;
	g_trace.last_set_rpwm_pslv = pslv;
	pwr->rpwm = pslv;
	if (g_set_rpwm_raises_cpwm)
		pwr->cpwm = pslv;
	return pslv;
}

static void register_task_alive(struct pwrctrl_priv *pwrctrl, u32 tag)
{
	pwrctrl->alives |= tag;
}

void host_pwrctrl_alive_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
	g_set_rpwm_raises_cpwm = _FALSE;
}

struct host_pwrctrl_alive_trace *host_pwrctrl_alive_get_trace(void)
{
	return &g_trace;
}

void host_pwrctrl_alive_set_rpwm_raises_cpwm(u8 v)
{
	g_set_rpwm_raises_cpwm = v;
}

s32 rtw_register_task_alive(PADAPTER padapter, u32 task)
{
	s32 res;
	struct pwrctrl_priv *pwrctrl;
	u8 pslv;

	res = _SUCCESS;
	pwrctrl = adapter_to_pwrctl(padapter);
	pslv = PS_STATE_S2;

	_enter_pwrlock(&pwrctrl->lock);

	register_task_alive(pwrctrl, task);

	if (pwrctrl->bFwCurrentInPSMode == _TRUE) {
		if (pwrctrl->cpwm < pslv) {
			if (pwrctrl->cpwm < PS_STATE_S2)
				res = _FAIL;
			if (pwrctrl->rpwm < pslv)
				rtw_set_rpwm(padapter, pslv);
		}
	}

	_exit_pwrlock(&pwrctrl->lock);

#ifdef CONFIG_DETECT_CPWM_BY_POLLING
	if (_FAIL == res) {
		if (pwrctrl->cpwm >= PS_STATE_S2)
			res = _SUCCESS;
	}
#endif

	return res;
}

s32 rtw_register_tx_alive(PADAPTER padapter)
{
	s32 res;
	struct pwrctrl_priv *pwrctrl;
	u8 pslv;

	res = _SUCCESS;
	pwrctrl = adapter_to_pwrctl(padapter);
	pslv = PS_STATE_S2;

	_enter_pwrlock(&pwrctrl->lock);

	register_task_alive(pwrctrl, XMIT_ALIVE);

	if (pwrctrl->bFwCurrentInPSMode == _TRUE) {
		if (pwrctrl->cpwm < pslv) {
			if (pwrctrl->cpwm < PS_STATE_S2)
				res = _FAIL;
			if (pwrctrl->rpwm < pslv)
				rtw_set_rpwm(padapter, pslv);
		}
	}

	_exit_pwrlock(&pwrctrl->lock);

#ifdef CONFIG_DETECT_CPWM_BY_POLLING
	if (_FAIL == res) {
		if (pwrctrl->cpwm >= PS_STATE_S2)
			res = _SUCCESS;
	}
#endif

	return res;
}

s32 rtw_register_cmd_alive(PADAPTER padapter)
{
	s32 res;
	struct pwrctrl_priv *pwrctrl;
	u8 pslv;

	res = _SUCCESS;
	pwrctrl = adapter_to_pwrctl(padapter);
	pslv = PS_STATE_S2;

	_enter_pwrlock(&pwrctrl->lock);

	register_task_alive(pwrctrl, CMD_ALIVE);

	if (pwrctrl->bFwCurrentInPSMode == _TRUE) {
		if (pwrctrl->cpwm < pslv) {
			if (pwrctrl->cpwm < PS_STATE_S2)
				res = _FAIL;
			if (pwrctrl->rpwm < pslv)
				rtw_set_rpwm(padapter, pslv);
		}
	}

	_exit_pwrlock(&pwrctrl->lock);

#ifdef CONFIG_DETECT_CPWM_BY_POLLING
	if (_FAIL == res) {
		if (pwrctrl->cpwm >= PS_STATE_S2)
			res = _SUCCESS;
	}
#endif

	return res;
}

s32 rtw_register_rx_alive(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrctrl;

	pwrctrl = adapter_to_pwrctl(padapter);

	_enter_pwrlock(&pwrctrl->lock);

	register_task_alive(pwrctrl, RECV_ALIVE);

	_exit_pwrlock(&pwrctrl->lock);

	return _SUCCESS;
}

s32 rtw_register_evt_alive(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrctrl;

	pwrctrl = adapter_to_pwrctl(padapter);

	_enter_pwrlock(&pwrctrl->lock);

	register_task_alive(pwrctrl, EVT_ALIVE);

	_exit_pwrlock(&pwrctrl->lock);

	return _SUCCESS;
}
