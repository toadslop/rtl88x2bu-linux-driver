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

static void unregister_task_alive(struct pwrctrl_priv *pwrctrl, u32 tag)
{
	pwrctrl->alives &= ~tag;
}

static u8 unregister_pslv_task(PADAPTER padapter)
{
	(void)padapter;
	return PS_STATE_S0;
}

static u8 unregister_pslv_io(PADAPTER padapter)
{
	(void)padapter;
	return PS_STATE_S0;
}

static void unregister_ps_alive(PADAPTER padapter, u32 tag, u8 pslv)
{
	struct pwrctrl_priv *pwrctrl = adapter_to_pwrctl(padapter);

	_enter_pwrlock(&pwrctrl->lock);

	unregister_task_alive(pwrctrl, tag);

	if ((pwrctrl->pwr_mode != PS_MODE_ACTIVE)
	    && (pwrctrl->bFwCurrentInPSMode == _TRUE)) {
		if (pwrctrl->cpwm > pslv) {
			if ((pslv >= PS_STATE_S2) || (pwrctrl->alives == 0))
				rtw_set_rpwm(padapter, pslv);
		}
	}

	_exit_pwrlock(&pwrctrl->lock);
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

void rtw_unregister_task_alive(PADAPTER padapter, u32 task)
{
	unregister_ps_alive(padapter, task, unregister_pslv_task(padapter));
}

void rtw_unregister_tx_alive(PADAPTER padapter)
{
	unregister_ps_alive(padapter, XMIT_ALIVE, unregister_pslv_io(padapter));
}

void rtw_unregister_cmd_alive(PADAPTER padapter)
{
	unregister_ps_alive(padapter, CMD_ALIVE, unregister_pslv_io(padapter));
}

void rtw_unregister_rx_alive(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrctrl = adapter_to_pwrctl(padapter);

	_enter_pwrlock(&pwrctrl->lock);

	unregister_task_alive(pwrctrl, RECV_ALIVE);

	_exit_pwrlock(&pwrctrl->lock);
}

void rtw_unregister_evt_alive(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrctrl = adapter_to_pwrctl(padapter);

	unregister_task_alive(pwrctrl, EVT_ALIVE);

	_exit_pwrlock(&pwrctrl->lock);
}
