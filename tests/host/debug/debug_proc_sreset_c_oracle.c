// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-216 proc rf4ce_state + sreset (from core/rtw_debug_rest.c). */

#include "host_rtw_debug_proc_sreset_types.h"

static u8 host_rf4ce_link_state;
static int host_sreset_reset_calls;
static s32 host_sreset_last_trigger;

void rtw_btcoex_SetRf4ceLinkState(struct _adapter *padapter, u8 state)
{
	(void)padapter;
	host_rf4ce_link_state = state;
}

u8 rtw_btcoex_GetRf4ceLinkState(struct _adapter *padapter)
{
	(void)padapter;
	return host_rf4ce_link_state;
}

void rtw_hal_sreset_reset(struct _adapter *padapter)
{
	(void)padapter;
	host_sreset_reset_calls++;
}

void sreset_set_trigger_point(struct _adapter *padapter, s32 tgp)
{
	(void)padapter;
	host_sreset_last_trigger = tgp;
}

void host_debug_proc_sreset_reset_mocks(void)
{
	host_rf4ce_link_state = 0;
	host_sreset_reset_calls = 0;
	host_sreset_last_trigger = -1;
}

int host_debug_proc_sreset_reset_calls(void)
{
	return host_sreset_reset_calls;
}

s32 host_debug_proc_sreset_last_trigger(void)
{
	return host_sreset_last_trigger;
}

void host_debug_proc_sreset_set_rf4ce(u8 state)
{
	host_rf4ce_link_state = state;
}

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

#ifdef CONFIG_BT_COEXIST
#ifdef CONFIG_RF4CE_COEXIST
int proc_get_rf4ce_state(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	u8 state;

	(void)v;
	state = rtw_btcoex_GetRf4ceLinkState(adapter);

	RTW_PRINT_SEL(m, "RF4CE %s\n", state ? "Connected" : "Disconnect");

	return 0;
}

ssize_t proc_set_rf4ce_state(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	char tmp[32];
	u8 state;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx", &state);

		if (num >= 1)
			rtw_btcoex_SetRf4ceLinkState(adapter, state);
	}

	return (ssize_t)count;
}
#endif /* CONFIG_RF4CE_COEXIST */
#endif /* CONFIG_BT_COEXIST */

#if defined(DBG_CONFIG_ERROR_DETECT)
int proc_get_sreset(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *psdpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	struct hal_data *pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	(void)v;

	if (psrtpriv->dbg_sreset_ctrl == _TRUE) {
		RTW_PRINT_SEL(m, "self_dect_tx_cnt:%llu\n",
			      (unsigned long long)psrtpriv->self_dect_tx_cnt);
		RTW_PRINT_SEL(m, "self_dect_rx_cnt:%llu\n",
			      (unsigned long long)psrtpriv->self_dect_rx_cnt);
		RTW_PRINT_SEL(m, "self_dect_fw_cnt:%llu\n",
			      (unsigned long long)psrtpriv->self_dect_fw_cnt);
		RTW_PRINT_SEL(m, "tx_dma_status_cnt:%llu\n",
			      (unsigned long long)psrtpriv->tx_dma_status_cnt);
		RTW_PRINT_SEL(m, "rx_dma_status_cnt:%llu\n",
			      (unsigned long long)psrtpriv->rx_dma_status_cnt);
		RTW_PRINT_SEL(m, "self_dect_case:%d\n", psrtpriv->self_dect_case);
		RTW_PRINT_SEL(m, "dbg_sreset_cnt:%d\n", pdbgpriv->dbg_sreset_cnt);
	}
	return 0;
}

ssize_t proc_set_sreset(void *file, const void *buffer, size_t count, long *pos,
			void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);
	struct hal_data *pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;
	char tmp[32];
	s32 trigger_point;

	(void)file;
	(void)pos;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &trigger_point);

		if (num < 1)
			return (ssize_t)count;

		if (trigger_point == SRESET_TGP_NULL)
			rtw_hal_sreset_reset(padapter);
		else if (trigger_point == SRESET_TGP_INFO)
			psrtpriv->dbg_sreset_ctrl = _TRUE;
		else
			sreset_set_trigger_point(padapter, trigger_point);
	}

	return (ssize_t)count;
}
#endif /* DBG_CONFIG_ERROR_DETECT */
