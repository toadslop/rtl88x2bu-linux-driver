// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-216 (kernel + L2 Rust oracle). */

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

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

void rtw_rust_debug_proc_emit_rf4ce_state(void *sel, void *adapter)
{
	u8 state = rtw_btcoex_GetRf4ceLinkState(adapter);

	RTW_PRINT_SEL(sel, "RF4CE %s\n", state ? "Connected" : "Disconnect");
}

long rtw_rust_debug_proc_set_rf4ce_state(void *adapter, const void *buffer,
					 size_t count)
{
	char tmp[32];
	u8 state;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx", &state);

		if (num >= 1)
			rtw_btcoex_SetRf4ceLinkState(adapter, state);
	}

	return (long)count;
}

void rtw_rust_debug_proc_emit_sreset(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct dvobj_priv *psdpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	struct hal_data *pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	if (psrtpriv->dbg_sreset_ctrl == _TRUE) {
		RTW_PRINT_SEL(sel, "self_dect_tx_cnt:%llu\n",
			      (unsigned long long)psrtpriv->self_dect_tx_cnt);
		RTW_PRINT_SEL(sel, "self_dect_rx_cnt:%llu\n",
			      (unsigned long long)psrtpriv->self_dect_rx_cnt);
		RTW_PRINT_SEL(sel, "self_dect_fw_cnt:%llu\n",
			      (unsigned long long)psrtpriv->self_dect_fw_cnt);
		RTW_PRINT_SEL(sel, "tx_dma_status_cnt:%llu\n",
			      (unsigned long long)psrtpriv->tx_dma_status_cnt);
		RTW_PRINT_SEL(sel, "rx_dma_status_cnt:%llu\n",
			      (unsigned long long)psrtpriv->rx_dma_status_cnt);
		RTW_PRINT_SEL(sel, "self_dect_case:%d\n", psrtpriv->self_dect_case);
		RTW_PRINT_SEL(sel, "dbg_sreset_cnt:%d\n", pdbgpriv->dbg_sreset_cnt);
	}
}

long rtw_rust_debug_proc_set_sreset(void *adapter, const void *buffer,
				    size_t count)
{
	struct _adapter *padapter = adapter;
	struct hal_data *pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;
	char tmp[32];
	s32 trigger_point;

	if (count < 1)
		return -1;

	if (count > sizeof(tmp))
		return -1;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &trigger_point);

		if (num < 1)
			return (long)count;

		if (trigger_point == SRESET_TGP_NULL)
			rtw_hal_sreset_reset(padapter);
		else if (trigger_point == SRESET_TGP_INFO)
			psrtpriv->dbg_sreset_ctrl = _TRUE;
		else
			sreset_set_trigger_point(padapter, trigger_point);
	}

	return (long)count;
}
