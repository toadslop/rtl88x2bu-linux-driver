/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#define _RTW_RM_UTIL_REST_C_

#ifdef HOST_RM_TEST
#include "host_rm_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if (!defined(CONFIG_RUST) || defined(HOST_RM_TEST)) && defined(CONFIG_RTW_80211K)

/* 802.11-2012 Table E-1 Operating classes in United States */
static RT_OPERATING_CLASS RTW_OP_CLASS_US[] = {
	/* 0, OP_CLASS_NULL */	{  0,  0, {}},
	/* 1, OP_CLASS_1 */	{115,  4, {36, 40, 44, 48}},
	/* 2, OP_CLASS_2 */	{118,  4, {52, 56, 60, 64}},
	/* 3, OP_CLASS_3 */	{124,  4, {149, 153, 157, 161}},
	/* 4, OP_CLASS_4 */	{121, 11, {100, 104, 108, 112, 116, 120, 124,
						128, 132, 136, 140}},
	/* 5, OP_CLASS_5 */	{125,  5, {149, 153, 157, 161, 165}},
	/* 6, OP_CLASS_12 */	{ 81, 11, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}}
};

u8 rm_get_ch_set(
	struct rtw_ieee80211_channel *pch_set, u8 op_class, u8 ch_num)
{
	int i, j, sz;
	u8 ch_amount = 0;

	sz = sizeof(RTW_OP_CLASS_US) / sizeof(struct _RT_OPERATING_CLASS);

	if (ch_num != 0) {
		pch_set[0].hw_value = ch_num;
		ch_amount = 1;
		RTW_INFO("RM: meas_ch->hw_value = %u\n", pch_set->hw_value);
		goto done;
	}

	for (i = 0; i < sz; i++) {
		if (RTW_OP_CLASS_US[i].global_op_class == op_class) {
			for (j = 0; j < RTW_OP_CLASS_US[i].Len; j++) {
				pch_set[j].hw_value =
					RTW_OP_CLASS_US[i].Channel[j];
				RTW_INFO("RM: meas_ch[%d].hw_value = %u\n",
					j, pch_set[j].hw_value);
			}
			ch_amount = RTW_OP_CLASS_US[i].Len;
			break;
		}
	}
done:
	return ch_amount;
}

u8 rm_get_oper_class_via_ch(u8 ch)
{
	int i, j, sz;

	sz = sizeof(RTW_OP_CLASS_US) / sizeof(struct _RT_OPERATING_CLASS);

	for (i = 0; i < sz; i++) {
		for (j = 0; j < RTW_OP_CLASS_US[i].Len; j++) {
			if (ch == RTW_OP_CLASS_US[i].Channel[j]) {
				RTW_INFO("RM: ch %u in oper_calss %u\n",
					ch, RTW_OP_CLASS_US[i].global_op_class);
				return RTW_OP_CLASS_US[i].global_op_class;
			}
		}
	}
	return 0;
}

int is_wildcard_bssid(u8 *bssid)
{
	int i;
	u8 val8 = 0xff;

	for (i = 0; i < 6; i++)
		val8 &= bssid[i];

	if (val8 == 0xff)
		return _SUCCESS;
	return _FALSE;
}

u8 translate_dbm_to_rcpi(s8 SignalPower)
{
	/* RCPI = Int{(Power in dBm + 110)*2} for 0dBm > Power > -110dBm
	 *    0	: power <= -110.0 dBm
	 *    1	: power =  -109.5 dBm
	 *    2	: power =  -109.0 dBm
	 */
	return (SignalPower + 110) * 2;
}

u8 translate_percentage_to_rcpi(u32 SignalStrengthIndex)
{
	/* Translate to dBm (x=y-100) */
	return translate_dbm_to_rcpi(SignalStrengthIndex - 100);
}

#endif /* (!CONFIG_RUST || HOST_RM_TEST) && CONFIG_RTW_80211K */
