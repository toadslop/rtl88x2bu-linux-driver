// SPDX-License-Identifier: GPL-2.0
/* Shared core for W3-224 TXSS proc L2 (C oracle + kernel helpers). */

#include "host_rtw_debug_proc_txss_types.h"

#ifdef HOST_TXSS_L1_REF
#define HOST_TXSS_SCOPE static
#else
#define HOST_TXSS_SCOPE
#endif

HOST_TXSS_SCOPE struct host_txss_dbg_log g_txss_dbg_log;

HOST_TXSS_SCOPE void host_debug_proc_txss_reset_log(void)
{
	memset(&g_txss_dbg_log, 0, sizeof(g_txss_dbg_log));
}

HOST_TXSS_SCOPE const struct host_txss_dbg_log *
host_debug_proc_txss_get_log(void)
{
	return &g_txss_dbg_log;
}

HOST_TXSS_SCOPE void dbg_ctrl_txss(struct _adapter *adapter, int tx_1ss)
{
	(void)adapter;
	g_txss_dbg_log.dbg_ctrl_calls++;
	g_txss_dbg_log.last_tx_1ss = (u8)(tx_1ss ? 1 : 0);
}

HOST_TXSS_SCOPE void rtw_rust_debug_proc_emit_txss_tp(void *sel,
						      struct _adapter *adapter)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;

	RTW_PRINT_SEL(sel, "TXSS  Control - %s\n",
		      pmlmeext->txss_ctrl_en ? "enable" : "disable");
	RTW_PRINT_SEL(sel, "TXSS  Tx TP TH - %d\n", (int)pmlmeext->txss_tp_th);
	RTW_PRINT_SEL(sel, "TXSS  check cnt - %d\n",
		      (int)pmlmeext->txss_tp_chk_cnt);
}

HOST_TXSS_SCOPE ssize_t rtw_rust_debug_proc_set_txss_tp(struct _adapter *adapter,
							const void *buffer,
							size_t count)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	char tmp[32];
	u32 enable = 0;
	u32 txss_tx_tp = 0;
	int txss_chk_cnt = 0;

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
		int num = sscanf(tmp, "%u %u %d", &enable, &txss_tx_tp,
				 &txss_chk_cnt);

		if (num < 1) {
			RTW_INFO("input parameters < 1\n");
			return -EINVAL;
		}
		pmlmeext->txss_ctrl_en = (u8)enable;

		if (txss_tx_tp)
			pmlmeext->txss_tp_th = (u16)txss_tx_tp;
		if (txss_chk_cnt)
			pmlmeext->txss_tp_chk_cnt = (u8)txss_chk_cnt;
	}

	return (ssize_t)count;
}

#ifdef DBG_CTRL_TXSS
HOST_TXSS_SCOPE void rtw_rust_debug_proc_emit_txss_ctrl(void *sel,
							 struct _adapter *adapter)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;

	RTW_PRINT_SEL(sel, "TXSS  1ss - %s\n", pmlmeext->txss_1ss ? "Y" : "N");
}

HOST_TXSS_SCOPE ssize_t rtw_rust_debug_proc_set_txss_ctrl(
	struct _adapter *adapter, const void *buffer, size_t count)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	char tmp[32];
	u32 tx_1ss = 0;

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
		int num = sscanf(tmp, "%u", &tx_1ss);

		if (num < 1) {
			RTW_INFO("input parameters < 1\n");
			return -EINVAL;
		}

		pmlmeext->txss_ctrl_en = _FALSE;
		dbg_ctrl_txss(adapter, (int)tx_1ss);
	}

	return (ssize_t)count;
}
#endif /* DBG_CTRL_TXSS */
