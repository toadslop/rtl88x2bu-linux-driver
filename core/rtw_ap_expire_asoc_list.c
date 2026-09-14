/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_EXPIRE_ASOC_LIST_C_

#include <drv_types.h>

void rtw_ap_expire_asoc_sta_tick(_adapter *padapter, struct sta_info *psta);

void rtw_ap_expire_asoc_list_scan(_adapter *padapter, char *chk_alive_list, u8 *chk_alive_num)
{
	_irqL irqL;
	_list *phead, *plist;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;

	_enter_critical_bh(&pstapriv->asoc_list_lock, &irqL);

	phead = &pstapriv->asoc_list;
	plist = get_next(phead);

#ifdef DBG_EXPIRATION_CHK
	if (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		RTW_INFO(FUNC_ADPT_FMT" asoc_list, cnt:%u\n"
			, FUNC_ADPT_ARG(padapter), pstapriv->asoc_list_cnt);
	}
#endif
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);
#ifdef CONFIG_ATMEL_RC_PATCH
		RTW_INFO("%s:%d  psta=%p, %02x,%02x||%02x,%02x  \n\n", __func__,  __LINE__,
			psta, pstapriv->atmel_rc_pattern[0], pstapriv->atmel_rc_pattern[5], psta->cmn.mac_addr[0], psta->cmn.mac_addr[5]);
		if (_rtw_memcmp((void *)pstapriv->atmel_rc_pattern, (void *)(psta->cmn.mac_addr), ETH_ALEN) == _TRUE)
			continue;
		if (psta->flag_atmel_rc)
			continue;
		RTW_INFO("%s: debug line:%d\n", __func__, __LINE__);
#endif
#ifdef CONFIG_AUTO_AP_MODE
		if (psta->isrc)
			continue;
#endif
		rtw_ap_expire_asoc_sta_tick(padapter, psta);

#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
		if ((psta->flags & WLAN_STA_HT) && (psta->htpriv.agg_enable_bitmap || psta->under_exist_checking)) {
			if (psta->expire_to <= (pstapriv->expire_to - 50)) {
				RTW_INFO("asoc expire by DELBA/ADDBA! (%d s)\n", (pstapriv->expire_to - psta->expire_to) * 2);
				psta->under_exist_checking = 0;
				psta->expire_to = 0;
			} else if (psta->expire_to <= (pstapriv->expire_to - 3) && (psta->under_exist_checking == 0)) {
				RTW_INFO("asoc check by DELBA/ADDBA! (%d s)\n", (pstapriv->expire_to - psta->expire_to) * 2);
				psta->under_exist_checking = 1;
				send_delba(padapter, 1, psta->cmn.mac_addr);
				psta->htpriv.agg_enable_bitmap = 0x0;
				psta->htpriv.candidate_tid_bitmap = 0x0;
			}
		}
#endif

		if (psta->expire_to <= 0) {
			struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;

			if (padapter->registrypriv.wifi_spec == 1) {
				psta->expire_to = pstapriv->expire_to;
				continue;
			}

#ifndef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
#ifdef CONFIG_80211N_HT

#define KEEP_ALIVE_TRYCNT (3)

			if (psta->keep_alive_trycnt > 0 && psta->keep_alive_trycnt <= KEEP_ALIVE_TRYCNT) {
				if (psta->state & WIFI_STA_ALIVE_CHK_STATE)
					psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
				else
					psta->keep_alive_trycnt = 0;

			} else if ((psta->keep_alive_trycnt > KEEP_ALIVE_TRYCNT) && !(psta->state & WIFI_STA_ALIVE_CHK_STATE))
				psta->keep_alive_trycnt = 0;
			if ((psta->htpriv.ht_option == _TRUE) && (psta->htpriv.ampdu_enable == _TRUE)) {
				uint priority = 1;
				u8 issued = 0;

				issued |= (psta->htpriv.candidate_tid_bitmap >> priority) & 0x1;

				if (0 == issued) {
					if (!(psta->state & WIFI_STA_ALIVE_CHK_STATE)) {
						psta->htpriv.candidate_tid_bitmap |= BIT((u8)priority);

						if (psta->state & WIFI_SLEEP_STATE)
							psta->expire_to = 2;
						else
							psta->expire_to = 1;

						psta->state |= WIFI_STA_ALIVE_CHK_STATE;

						RTW_INFO("issue addba_req to check if sta alive, keep_alive_trycnt=%d\n", psta->keep_alive_trycnt);

						issue_addba_req(padapter, psta->cmn.mac_addr, (u8)priority);

						_set_timer(&psta->addba_retry_timer, ADDBA_TO);

						psta->keep_alive_trycnt++;

						continue;
					}
				}
			}
			if (psta->keep_alive_trycnt > 0 && psta->state & WIFI_STA_ALIVE_CHK_STATE) {
				psta->keep_alive_trycnt = 0;
				psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
				RTW_INFO("change to another methods to check alive if staion is at ps mode\n");
			}

#endif
#endif
			if (psta->state & WIFI_SLEEP_STATE) {
				if (!(psta->state & WIFI_STA_ALIVE_CHK_STATE)) {
					psta->expire_to = pstapriv->expire_to;
					psta->state |= WIFI_STA_ALIVE_CHK_STATE;

					rtw_tim_map_set(padapter, pstapriv->tim_bitmap, psta->cmn.aid);
					update_beacon(padapter, _TIM_IE_, NULL, _TRUE, 0);

					if (!pmlmeext->active_keep_alive_check)
						continue;
				}
			}

			{
				int stainfo_offset;

				stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
				if (stainfo_offset_valid(stainfo_offset))
					chk_alive_list[(*chk_alive_num)++] = stainfo_offset;
				continue;
			}
		} else {
			if (psta->sleepq_len > (NR_XMITFRAME / pstapriv->asoc_list_cnt)
			    && padapter->xmitpriv.free_xmitframe_cnt < ((NR_XMITFRAME / pstapriv->asoc_list_cnt) / 2)
			   ) {
				RTW_INFO(FUNC_ADPT_FMT" sta:"MAC_FMT", sleepq_len:%u, free_xmitframe_cnt:%u, asoc_list_cnt:%u, clear sleep_q\n"
					, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->cmn.mac_addr)
					, psta->sleepq_len, padapter->xmitpriv.free_xmitframe_cnt, pstapriv->asoc_list_cnt);
				wakeup_sta_to_xmit(padapter, psta);
			}
		}
	}

	_exit_critical_bh(&pstapriv->asoc_list_lock, &irqL);
}
