// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-128 — mirrored from hal/hal_com_rest.c. */

#include "host_hal_com_rcr_chk_bssid_types.h"
#include <string.h>

void rtw_hal_get_hwreg(struct _adapter *padapter, u32 variable, u8 *val);
u8 rtw_hal_set_hwreg(struct _adapter *padapter, u32 variable, u8 *val);
void rtw_mi_status_no_self(struct _adapter *adapter, struct mi_state *mstate);
void rtw_mi_status_no_others(struct _adapter *adapter, struct mi_state *mstate);
void rtw_mi_status_merge(struct mi_state *d, struct mi_state *a);
u8 get_clt_num(struct _adapter *adapter);
void rtw_warn_on(int c);
struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter);
void rtw_hal_tsf_update_pause(struct _adapter *adapter);
void rtw_hal_tsf_update_restore(struct _adapter *adapter);

#define MSTATE_AP_NUM(m)         ((m)->ap_num)
#define MSTATE_MESH_NUM(m)       ((m)->mesh_num)
#define MSTATE_TDLS_LD_NUM(m)    ((m)->ld_tdls_num)
#define MSTATE_SCAN_ENTER_NUM(m) ((m)->scan_enter_num)
#define MSTATE_STA_LG_NUM(m)     ((m)->lg_sta_num)

static u8 hal_rx_tsf_filter(struct _adapter *adapter)
{
	return GET_HAL_SPEC(adapter)->rx_tsf_filter;
}

static void set_chk_bssid_oracle(struct _adapter *adapter, u8 self_action)
{
	struct hal_data *hal_data = GET_HAL_DATA(adapter);
	u32 rcr, rcr_new;
	struct mi_state mstate;
	struct mi_state mstate_s;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	memset(&mstate, 0, sizeof(mstate));
	memset(&mstate_s, 0, sizeof(mstate_s));

	rtw_hal_get_hwreg(adapter, HW_VAR_RCR, (u8 *)&rcr);
	rcr_new = rcr;

#if defined(CONFIG_MI_WITH_MBSSID_CAM) && !defined(CONFIG_CLIENT_PORT_CFG)
	rcr_new &= ~(RCR_CBSSID_BCN | RCR_CBSSID_DATA);
#else
	rtw_mi_status_no_self(adapter, &mstate);
	rtw_mi_status_no_others(adapter, &mstate_s);

	switch (self_action) {
	case MLME_SCAN_ENTER:
		mstate_s.scan_num = 1;
		mstate_s.scan_enter_num = 1;
		break;
	case MLME_SCAN_DONE:
		mstate_s.scan_enter_num = 0;
		break;
	case MLME_STA_CONNECTING:
		mstate_s.lg_sta_num = 1;
		mstate_s.ld_sta_num = 0;
		break;
	case MLME_STA_CONNECTED:
		mstate_s.lg_sta_num = 0;
		mstate_s.ld_sta_num = 1;
		break;
	case MLME_STA_DISCONNECTED:
		mstate_s.lg_sta_num = 0;
		mstate_s.ld_sta_num = 0;
		break;
#ifdef CONFIG_TDLS
	case MLME_TDLS_LINKED:
		mstate_s.ld_tdls_num = 1;
		break;
	case MLME_TDLS_NOLINK:
		mstate_s.ld_tdls_num = 0;
		break;
#endif
#ifdef CONFIG_AP_MODE
	case MLME_AP_STARTED:
		mstate_s.ap_num = 1;
		break;
	case MLME_AP_STOPPED:
		mstate_s.ap_num = 0;
		mstate_s.ld_ap_num = 0;
		break;
#endif
#ifdef CONFIG_RTW_MESH
	case MLME_MESH_STARTED:
		mstate_s.mesh_num = 1;
		break;
	case MLME_MESH_STOPPED:
		mstate_s.mesh_num = 0;
		mstate_s.ld_mesh_num = 0;
		break;
#endif
	case MLME_ACTION_NONE:
	case MLME_ADHOC_STARTED:
		break;
	default:
		rtw_warn_on(1);
	}

	rtw_mi_status_merge(&mstate, &mstate_s);

	if (MSTATE_AP_NUM(&mstate) || MSTATE_MESH_NUM(&mstate) ||
	    MSTATE_TDLS_LD_NUM(&mstate) ||
#ifdef CONFIG_FIND_BEST_CHANNEL
	    MSTATE_SCAN_ENTER_NUM(&mstate) ||
#endif
	    hal_data->in_cta_test)
		rcr_new &= ~RCR_CBSSID_DATA;
	else
		rcr_new |= RCR_CBSSID_DATA;

	if (MSTATE_SCAN_ENTER_NUM(&mstate) || hal_data->in_cta_test)
		rcr_new &= ~RCR_CBSSID_BCN;
	else if (MSTATE_STA_LG_NUM(&mstate) || dvobj->periodic_tsf_update_etime)
		rcr_new |= RCR_CBSSID_BCN;
	else if ((MSTATE_AP_NUM(&mstate) && host_hal_rcr_chk_wifi_spec(adapter)) ||
		 MSTATE_MESH_NUM(&mstate))
		rcr_new &= ~RCR_CBSSID_BCN;
	else
		rcr_new |= RCR_CBSSID_BCN;

	if (get_clt_num(adapter) > MAX_CLIENT_PORT_NUM)
		rcr_new &= ~RCR_CBSSID_BCN;
#endif

#ifdef CONFIG_RTW_MULTI_AP
	if (MSTATE_AP_NUM(&mstate) &&
	    rtw_unassoc_sta_src_chk(adapter, UNASOC_STA_SRC_RX_NMY_UC))
		rcr_new |= RCR_AAP;
	else
		rcr_new &= ~RCR_AAP;
#endif

	if (rcr == rcr_new)
		return;

	if (!hal_rx_tsf_filter(adapter) && (rcr & RCR_CBSSID_BCN) &&
	    !(rcr_new & RCR_CBSSID_BCN))
		rtw_hal_tsf_update_pause(adapter);

	rtw_hal_set_hwreg(adapter, HW_VAR_RCR, (u8 *)&rcr_new);

	if (!hal_rx_tsf_filter(adapter) && !(rcr & RCR_CBSSID_BCN) &&
	    (rcr_new & RCR_CBSSID_BCN) && self_action != MLME_STA_CONNECTING)
		rtw_hal_tsf_update_restore(adapter);
}

#ifndef RUST_HAL_COM_RCR_CHK_BSSID_ORACLE
void rtw_hal_rcr_set_chk_bssid(struct _adapter *adapter, u8 self_action)
{
	set_chk_bssid_oracle(adapter, self_action);
}

void rtw_hal_rcr_set_chk_bssid_act_non(struct _adapter *adapter)
{
	set_chk_bssid_oracle(adapter, MLME_ACTION_NONE);
}
#endif

void rtw_hal_rcr_set_chk_bssid_c_oracle(struct _adapter *adapter, u8 self_action)
{
	set_chk_bssid_oracle(adapter, self_action);
}

void rtw_hal_rcr_set_chk_bssid_act_non_c_oracle(struct _adapter *adapter)
{
	set_chk_bssid_oracle(adapter, MLME_ACTION_NONE);
}