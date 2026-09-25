// SPDX-License-Identifier: GPL-2.0
/* Host L2 C oracle: sitesurvey_cmd_hdl enter/process (W3-89 part 1). */
#include "host_mlme_ext_sitesurvey_cmd_types.h"
#include <string.h>

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
		host_sitesurvey_res_reset(padapter, pparm);
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
