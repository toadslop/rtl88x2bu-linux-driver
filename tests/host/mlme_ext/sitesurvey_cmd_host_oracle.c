// SPDX-License-Identifier: GPL-2.0
/* Host L2 C oracle: sitesurvey_cmd_hdl enter/process (W3-89 part 1). */
#include "host_mlme_ext_sitesurvey_cmd_types.h"
#include <string.h>

static void sitesurvey_res_reset(_adapter *adapter, struct sitesurvey_parm *parm)
{
	struct ss_res *ss = &adapter->mlmeextpriv.sitesurvey_res;
	RT_CHANNEL_INFO *chset = adapter_to_chset(adapter);
	int i;

	ss->bss_cnt = 0;
	ss->activate_ch_cnt = 0;
	ss->channel_idx = 0;
	ss->force_ssid_scan = 0;
	ss->igi_scan = 0;
	ss->igi_before_scan = 0;
	ss->scan_cnt = 0;
	ss->ssid_num = 0;
	for (i = 0; i < RTW_SSID_SCAN_AMOUNT; i++) {
		if (parm->ssid[i].SsidLength) {
			memcpy(ss->ssid[i].Ssid, parm->ssid[i].Ssid, 32);
			ss->ssid[i].SsidLength = parm->ssid[i].SsidLength;
			ss->ssid_num++;
		} else {
			ss->ssid[i].SsidLength = 0;
		}
	}
	ss->ch_num = (u8)rtw_scan_ch_decision(adapter, ss->ch, RTW_CHANNEL_SCAN_AMOUNT,
					      parm->ch, parm->ch_num, parm->acs, parm->reason);
	for (i = 0; i < MAX_CHANNEL_NUM; i++)
		chset[i].hidden_bss_cnt = 0;
	ss->bw = parm->bw;
	ss->igi = parm->igi;
	ss->token = parm->token;
	ss->duration = parm->duration;
	ss->scan_mode = (u8)parm->scan_mode;
	ss->acs = parm->acs;
}

u8 sitesurvey_cmd_hdl(_adapter *padapter, u8 *pbuf)
{
	struct sitesurvey_parm *pparm = (struct sitesurvey_parm *)pbuf;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;
	u8 val8;

	if (mlmeext_chk_scan_state(pmlmeext, SCAN_PROCESS))
		ss->channel_idx++;

	if (mlmeext_scan_state(pmlmeext) != mlmeext_scan_next_state(pmlmeext))
		mlmeext_set_scan_state(pmlmeext, mlmeext_scan_next_state(pmlmeext));

operation_by_state:
	switch (mlmeext_scan_state(pmlmeext)) {
	case SCAN_DISABLE:
		sitesurvey_res_reset(padapter, pparm);
		mlmeext_set_scan_state(pmlmeext, SCAN_START);
		goto operation_by_state;
	case SCAN_START:
		if (ss->rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID ||
		    ss->rx_ampdu_size != RX_AMPDU_SIZE_INVALID)
			rtw_rx_ampdu_apply(padapter);
		rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, 0);
		rtw_hal_macid_sleep_all_used(padapter);
		if (rtw_ps_annc(padapter, 1)) {
			mlmeext_set_scan_state(pmlmeext, SCAN_PS_ANNC_WAIT);
			mlmeext_set_scan_next_state(pmlmeext, SCAN_ENTER);
			set_survey_timer(pmlmeext, 50);
		} else {
			mlmeext_set_scan_state(pmlmeext, SCAN_ENTER);
			goto operation_by_state;
		}
		break;
	case SCAN_ENTER:
		rtw_phydm_ability_backup(padapter);
		sitesurvey_set_igi(padapter);
		rtw_phydm_func_for_offchannel(padapter);
		sitesurvey_set_msr(padapter, _TRUE);
		val8 = 1;
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &val8);
		mlmeext_set_scan_state(pmlmeext, SCAN_PROCESS);
		goto operation_by_state;
	case SCAN_PROCESS: {
		u8 scan_ch;
		RT_SCAN_TYPE scan_type;
		u8 next_state;
		u32 scan_ms;

		next_state = sitesurvey_pick_ch_behavior(padapter, &scan_ch, &scan_type);
		if (next_state != SCAN_PROCESS) {
			mlmeext_set_scan_state(pmlmeext, next_state);
			goto operation_by_state;
		}
		site_survey(padapter, scan_ch, scan_type);
		scan_ms = ss->scan_ch_ms;
		set_survey_timer(pmlmeext, scan_ms);
		break;
	}
	default:
		break;
	}
	return 0;
}
