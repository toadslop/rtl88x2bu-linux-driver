// SPDX-License-Identifier: GPL-2.0
/* Shared core for W3-225 LPS chk-by-TP proc L2 (C oracle + kernel helpers). */

#include "host_rtw_debug_proc_lps_tp_types.h"

#ifdef HOST_LPS_TP_L1_REF
#define HOST_LPS_TP_SCOPE static
#else
#define HOST_LPS_TP_SCOPE
#endif

HOST_LPS_TP_SCOPE void rtw_rust_debug_proc_emit_lps_chk_tp(void *sel,
							 struct _adapter *adapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(adapter);

	RTW_PRINT_SEL(sel, "LPS chk by tp - %s\n",
		      pwrpriv->lps_chk_by_tp ? "enable" : "disable");
	RTW_PRINT_SEL(sel, "LPS Tx TP TH - %d(Mbps)\n", (int)pwrpriv->lps_tx_tp_th);
	RTW_PRINT_SEL(sel, "LPS Rx TP TH - %d(Mbps)\n", (int)pwrpriv->lps_rx_tp_th);
	RTW_PRINT_SEL(sel, "LPS BI TP TH - %d(Mbps)\n", (int)pwrpriv->lps_bi_tp_th);
	RTW_PRINT_SEL(sel, "LPS CHK CNT - %d\n", pwrpriv->lps_chk_cnt_th);
	RTW_PRINT_SEL(sel, "LPS Tx PKTs - %d\n", (int)pwrpriv->lps_tx_pkts);
	RTW_PRINT_SEL(sel, "LPS Rx PKTs - %d\n", (int)pwrpriv->lps_rx_pkts);
}

HOST_LPS_TP_SCOPE ssize_t rtw_rust_debug_proc_set_lps_chk_tp(
	struct _adapter *adapter, const void *buffer, size_t count)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(adapter);
	char tmp[32];
	u32 enable = 0;
	u32 lps_tx_tp = 0, lps_rx_tp = 0, lps_bi_tp = 0;
	int lps_chk_cnt_th = 0;
	u32 lps_tx_pkts = 0, lps_rx_pkts = 0;

	if (!buffer) {
		RTW_INFO("input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO("input length is 0!\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO("input length is too large\n");
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u %u %u %u %d %u %u",
				 &enable, &lps_tx_tp, &lps_rx_tp, &lps_bi_tp,
				 &lps_chk_cnt_th, &lps_tx_pkts, &lps_rx_pkts);

		if (num < 1) {
			RTW_INFO("input parameters < 1\n");
			return -EINVAL;
		}
		pwrpriv->lps_chk_by_tp = (u8)enable;

		if (lps_tx_tp) {
			pwrpriv->lps_tx_tp_th = (u16)lps_tx_tp;
			pwrpriv->lps_rx_tp_th = (u16)lps_tx_tp;
			pwrpriv->lps_bi_tp_th = (u16)lps_tx_tp;
		}
		if (lps_rx_tp)
			pwrpriv->lps_rx_tp_th = (u16)lps_rx_tp;
		if (lps_bi_tp)
			pwrpriv->lps_bi_tp_th = (u16)lps_bi_tp;

		if (lps_chk_cnt_th)
			pwrpriv->lps_chk_cnt_th = lps_chk_cnt_th;

		if (lps_tx_pkts)
			pwrpriv->lps_tx_pkts = lps_tx_pkts;

		if (lps_rx_pkts)
			pwrpriv->lps_rx_pkts = lps_rx_pkts;
	}

	return (ssize_t)count;
}
