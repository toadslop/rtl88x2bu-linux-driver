/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_AP_REST_C_

#ifdef HOST_AP_REST_TEST
#include "host_ap_rest_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_AP_REST_TEST) || !defined(CONFIG_RUST_AP_REST)

/*
 * Set TIM IE
 * return length of total TIM IE
 */
u8 rtw_set_tim_ie(u8 dtim_cnt, u8 dtim_period
	, const u8 *tim_bmp, u8 tim_bmp_len, u8 *tim_ie)
{
	u8 *p = tim_ie;
	u8 i, n1, n2;
	u8 bmp_len;

	if (rtw_bmp_not_empty(tim_bmp, tim_bmp_len)) {
		/* find the first nonzero octet in tim_bitmap */
		for (i = 0; i < tim_bmp_len; i++)
			if (tim_bmp[i])
				break;
		n1 = i & 0xFE;

		/* find the last nonzero octet in tim_bitmap, except octet 0 */
		for (i = tim_bmp_len - 1; i > 0; i--)
			if (tim_bmp[i])
				break;
		n2 = i;
		bmp_len = n2 - n1 + 1;
	} else {
		n1 = n2 = 0;
		bmp_len = 1;
	}

	*p++ = WLAN_EID_TIM;
	*p++ = 2 + 1 + bmp_len;
	*p++ = dtim_cnt;
	*p++ = dtim_period;
	*p++ = (rtw_bmp_is_set(tim_bmp, tim_bmp_len, 0) ? BIT0 : 0) | n1;
	_rtw_memcpy(p, tim_bmp + n1, bmp_len);

	return 2 + 2 + 1 + bmp_len;
}

#ifdef CONFIG_FW_HANDLE_TXBCN
u8 rtw_ap_allocate_vapid(struct dvobj_priv *dvobj)
{
	u8 vap_id;

	for (vap_id = 0; vap_id < CONFIG_LIMITED_AP_NUM; vap_id++) {
		if (!(dvobj->vap_map & BIT(vap_id)))
			break;
	}

	if (vap_id < CONFIG_LIMITED_AP_NUM)
		dvobj->vap_map |= BIT(vap_id);

	return vap_id;
}

u8 rtw_ap_release_vapid(struct dvobj_priv *dvobj, u8 vap_id)
{
	if (vap_id >= CONFIG_LIMITED_AP_NUM) {
		RTW_ERR("%s - vapid(%d) failed\n", __func__, vap_id);
		rtw_warn_on(1);
		return _FAIL;
	}
	dvobj->vap_map &= ~BIT(vap_id);
	return _SUCCESS;
}
#endif /* CONFIG_FW_HANDLE_TXBCN */

#ifdef CONFIG_BMC_TX_RATE_SELECT
u8 rtw_ap_find_bmc_rate(_adapter *adapter, u8 tx_rate)
{
	u8 tx_ini_rate = ODM_RATE6M;
	u8 band = GET_HAL_DATA(adapter)->current_band_type;

	switch (tx_rate) {
	case 0x49: case 0x48: case 0x47: case 0x46: case 0x45: case 0x44: case 0x43:
	case 0x3F: case 0x3E: case 0x3D: case 0x3C: case 0x3B: case 0x3A: case 0x39:
	case 0x35: case 0x34: case 0x33: case 0x32: case 0x31: case 0x30: case 0x2F:
	case 0x1B: case 0x1A: case 0x19: case 0x18: case 0x17: case 0x13: case 0x12:
	case 0x11: case 0x10: case 0x0F: case 0x0B: case 0x0A: case 0x09: case 0x08:
		tx_ini_rate = ODM_RATE24M;
		break;
	case 0x42: case 0x41: case 0x38: case 0x37: case 0x2E: case 0x2D:
	case 0x16: case 0x15: case 0x0E: case 0x0D: case 0x07: case 0x06:
		tx_ini_rate = ODM_RATE12M;
		break;
	case 0x40: case 0x36: case 0x2C: case 0x14: case 0x0C: case 0x05: case 0x04:
		tx_ini_rate = ODM_RATE6M;
		break;
	case 0x03: case 0x02: case 0x01: case 0x00:
		tx_ini_rate = ODM_RATE1M;
		break;
	default:
		break;
	}
	if (band == BAND_ON_5G && tx_ini_rate < ODM_RATE6M)
		tx_ini_rate = ODM_RATE6M;
	return tx_ini_rate;
}

u8 rtw_ap_find_mini_tx_rate(_adapter *adapter)
{
	_irqL irqL;
	_list	*phead, *plist;
	u8 miini_tx_rate = ODM_RATEVHTSS4MCS9, sta_tx_rate;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &adapter->stapriv;

	_enter_critical_bh(&pstapriv->asoc_list_lock, &irqL);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		sta_tx_rate = psta->cmn.ra_info.curr_tx_rate & 0x7F;
		if (sta_tx_rate < miini_tx_rate)
			miini_tx_rate = sta_tx_rate;
	}
	_exit_critical_bh(&pstapriv->asoc_list_lock, &irqL);

	return miini_tx_rate;
}
#endif /* CONFIG_BMC_TX_RATE_SELECT */

#endif /* !CONFIG_RUST || HOST_AP_REST_TEST || !CONFIG_RUST_AP_REST */

#if defined(CONFIG_RUST) && !defined(HOST_AP_REST_TEST)

#ifdef CONFIG_SUPPORT_MULTI_BCN
u8 rtw_rust_ap_get_vap_map(struct dvobj_priv *dvobj)
{
	return dvobj->vap_map;
}

void rtw_rust_ap_set_vap_map(struct dvobj_priv *dvobj, u8 vap_map)
{
	dvobj->vap_map = vap_map;
}
#endif /* CONFIG_SUPPORT_MULTI_BCN */

u8 rtw_rust_ap_limited_ap_num(void)
{
	return CONFIG_LIMITED_AP_NUM;
}

void rtw_rust_ap_vapid_fail_log(u8 vap_id)
{
	RTW_ERR("%s - vapid(%d) failed\n", __func__, vap_id);
}

void rtw_rust_ap_warn_on(int condition)
{
	rtw_warn_on(condition);
}

#ifdef CONFIG_BMC_TX_RATE_SELECT
BAND_TYPE rtw_rust_ap_current_band_type(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->current_band_type;
}

u8 rtw_rust_ap_find_mini_tx_rate(_adapter *adapter)
{
	_irqL irqL;
	_list	*phead, *plist;
	u8 miini_tx_rate = ODM_RATEVHTSS4MCS9, sta_tx_rate;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &adapter->stapriv;

	_enter_critical_bh(&pstapriv->asoc_list_lock, &irqL);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		sta_tx_rate = psta->cmn.ra_info.curr_tx_rate & 0x7F;
		if (sta_tx_rate < miini_tx_rate)
			miini_tx_rate = sta_tx_rate;
	}
	_exit_critical_bh(&pstapriv->asoc_list_lock, &irqL);

	return miini_tx_rate;
}
#endif /* CONFIG_BMC_TX_RATE_SELECT */

#endif /* CONFIG_RUST && !HOST_AP_REST_TEST */
