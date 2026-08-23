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

#endif /* !CONFIG_RUST || HOST_AP_REST_TEST || !CONFIG_RUST_AP_REST */

#if defined(CONFIG_RUST) && !defined(HOST_AP_REST_TEST)

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

#endif /* CONFIG_RUST && !HOST_AP_REST_TEST */
