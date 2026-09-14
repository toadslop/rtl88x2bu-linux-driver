/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_EXPIRE_CHK_ALIVE_C_

#include <drv_types.h>

#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
int issue_aka_chk_frame(_adapter *adapter, struct sta_info *psta);
#endif

u8 rtw_ap_expire_chk_alive_process(_adapter *padapter, char *chk_alive_list, u8 chk_alive_num)
{
	_irqL irqL;
	u8 updated = _FALSE;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	int i;

#if defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK)
	u8 backup_ch = 0, backup_bw = 0, backup_offset = 0;
	u8 union_ch = 0, union_bw = 0, union_offset = 0;
	u8 switch_channel_by_drv = _TRUE;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
#endif
	char del_asoc_list[NUM_STA];

	_rtw_memset(del_asoc_list, NUM_STA, NUM_STA);

#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
	if (pmlmeext->active_keep_alive_check) {
#ifdef CONFIG_MCC_MODE
		if (MCC_EN(padapter)) {
			if (rtw_hal_check_mcc_status(padapter, MCC_STATUS_DOING_MCC))
				switch_channel_by_drv = _FALSE;
		}
#endif
		if (!rtw_mi_get_ch_setting_union(padapter, &union_ch, &union_bw, &union_offset)
		    || pmlmeext->cur_channel != union_ch)
			switch_channel_by_drv = _FALSE;

		if (switch_channel_by_drv == _TRUE && rtw_get_oper_ch(padapter) != pmlmeext->cur_channel) {
			backup_ch = rtw_get_oper_ch(padapter);
			backup_bw = rtw_get_oper_bw(padapter);
			backup_offset = rtw_get_oper_choffset(padapter);
			set_channel_bwmode(padapter, union_ch, union_offset, union_bw);
		}
	}
#endif

	for (i = 0; i < chk_alive_num; i++) {
#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
		int ret = _FAIL;
#endif

		psta = rtw_get_stainfo_by_offset(pstapriv, chk_alive_list[i]);

#ifdef CONFIG_ATMEL_RC_PATCH
		if (_rtw_memcmp(pstapriv->atmel_rc_pattern, psta->cmn.mac_addr, ETH_ALEN) == _TRUE)
			continue;
		if (psta->flag_atmel_rc)
			continue;
#endif

		if (!(psta->state & WIFI_ASOC_STATE))
			continue;

#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
		if (pmlmeext->active_keep_alive_check) {
			ret = issue_aka_chk_frame(padapter, psta);

			psta->keep_alive_trycnt++;
			if (ret == _SUCCESS) {
				RTW_INFO(FUNC_ADPT_FMT" asoc check, "MAC_FMT" is alive\n"
					, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->cmn.mac_addr));
				psta->expire_to = pstapriv->expire_to;
				psta->keep_alive_trycnt = 0;
				continue;
			} else if (psta->keep_alive_trycnt <= 3) {
				RTW_INFO(FUNC_ADPT_FMT" asoc check, "MAC_FMT" keep_alive_trycnt=%d\n"
					, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->cmn.mac_addr), psta->keep_alive_trycnt);
				psta->expire_to = 1;
				continue;
			}
		}
#endif

		psta->keep_alive_trycnt = 0;
		del_asoc_list[i] = chk_alive_list[i];
		_enter_critical_bh(&pstapriv->asoc_list_lock, &irqL);
		if (rtw_is_list_empty(&psta->asoc_list) == _FALSE) {
			rtw_list_delete(&psta->asoc_list);
			pstapriv->asoc_list_cnt--;
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
			if (psta->tbtx_enable)
				pstapriv->tbtx_asoc_list_cnt--;
#endif
			STA_SET_MESH_PLINK(psta, NULL);
		}
		_exit_critical_bh(&pstapriv->asoc_list_lock, &irqL);
	}

	for (i = 0; i < chk_alive_num; i++) {
		u8 sta_addr[ETH_ALEN];

		if (del_asoc_list[i] >= NUM_STA)
			continue;

		psta = rtw_get_stainfo_by_offset(pstapriv, del_asoc_list[i]);
		_rtw_memcpy(sta_addr, psta->cmn.mac_addr, ETH_ALEN);

		RTW_INFO(FUNC_ADPT_FMT" asoc expire "MAC_FMT", state=0x%x\n"
			, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->cmn.mac_addr), psta->state);
		updated |= ap_free_sta(padapter, psta, _FALSE, WLAN_REASON_DEAUTH_LEAVING, _FALSE);
#ifdef CONFIG_RTW_MESH
		if (MLME_IS_MESH(padapter))
			rtw_mesh_expire_peer(padapter, sta_addr);
#endif
	}

#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
	if (pmlmeext->active_keep_alive_check) {
		if (switch_channel_by_drv == _TRUE && backup_ch > 0)
			set_channel_bwmode(padapter, backup_ch, backup_offset, backup_bw);
	}
#endif

	return updated;
}
