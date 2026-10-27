/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-174 traffic_check_for_leave_lps_by_tp. */

#include "host_pwrctrl_lps_tp.h"

#include <string.h>

static struct host_pwrctrl_lps_tp_trace g_trace;
static u8 g_bt_control_lps;

static u8 rtw_btcoex_IsBtControlLps(PADAPTER padapter)
{
	(void)padapter;
	return g_bt_control_lps;
}

static u8 rtw_lps_ctrl_wk_cmd(PADAPTER padapter, u8 lps_ctrl_type, u8 flags)
{
	(void)padapter;
	(void)flags;
	g_trace.lps_ctrl_calls++;
	g_trace.last_lps_ctrl_type = lps_ctrl_type;
	return _TRUE;
}

void host_pwrctrl_lps_tp_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
	g_bt_control_lps = _FALSE;
}

struct host_pwrctrl_lps_tp_trace *host_pwrctrl_lps_tp_get_trace(void)
{
	return &g_trace;
}

void host_pwrctrl_lps_tp_set_bt_control_lps(u8 v)
{
	g_bt_control_lps = v;
}

void traffic_check_for_leave_lps_by_tp(PADAPTER padapter, u8 tx, struct sta_info *sta)
{
	struct stainfo_stats *pstats = &sta->sta_stats;
	u64 cur_acc_tx_bytes = 0, cur_acc_rx_bytes = 0;
	u32 tx_tp_kbyte = 0, rx_tp_kbyte = 0;
	u32 tx_tp_th = 0, rx_tp_th = 0;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	u8 leave_lps = _FALSE;

	if (tx) {
		cur_acc_tx_bytes = pstats->tx_bytes - pstats->acc_tx_bytes;
		tx_tp_kbyte = (u32)(cur_acc_tx_bytes >> 10);
		tx_tp_th = pwrpriv->lps_tx_tp_th * 1024 / 8 * 2;

		if (tx_tp_kbyte >= tx_tp_th ||
		    padapter->mlmepriv.LinkDetectInfo.NumTxOkInPeriod >=
			    pwrpriv->lps_tx_pkts) {
			if (pwrpriv->bLeisurePs && (pwrpriv->pwr_mode != PS_MODE_ACTIVE) &&
			    (rtw_btcoex_IsBtControlLps(padapter) == _FALSE))
				leave_lps = _TRUE;
		}
	} else {
		cur_acc_rx_bytes = pstats->rx_bytes - pstats->acc_rx_bytes;
		rx_tp_kbyte = (u32)(cur_acc_rx_bytes >> 10);
		rx_tp_th = pwrpriv->lps_rx_tp_th * 1024 / 8 * 2;

		if (rx_tp_kbyte >= rx_tp_th ||
		    padapter->mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod >=
			    pwrpriv->lps_rx_pkts) {
			if (pwrpriv->bLeisurePs && (pwrpriv->pwr_mode != PS_MODE_ACTIVE) &&
			    (rtw_btcoex_IsBtControlLps(padapter) == _FALSE))
				leave_lps = _TRUE;
		}
	}

	if (leave_lps) {
		pwrpriv->lps_chk_cnt = pwrpriv->lps_chk_cnt_th;
		rtw_lps_ctrl_wk_cmd(padapter,
				    tx ? LPS_CTRL_TX_TRAFFIC_LEAVE : LPS_CTRL_RX_TRAFFIC_LEAVE,
				    0);
	}
}
