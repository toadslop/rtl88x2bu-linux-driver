// SPDX-License-Identifier: GPL-2.0
#include "host_rson_types.h"

u8 rtw_cal_rson_score(struct rtw_rson_struct *cand_rson_data, NDIS_802_11_RSSI rssi)
{
	if ((cand_rson_data->hopcnt == RTW_RSON_HC_NOTREADY) ||
	    (cand_rson_data->connectible == RTW_RSON_DENYCONNECT))
		return RTW_RSON_SCORE_NOTCNNT;

	return RTW_RSON_SCORE_MAX - (cand_rson_data->hopcnt * 10) + (rssi / 10);
}

int is_match_bssid(u8 *mac, u8 bssid_array[][6], int num)
{
	int i;

	for (i = 0; i < num; i++)
		if (_rtw_memcmp(mac, bssid_array[i], 6) == _TRUE)
			return _TRUE;
	return _FALSE;
}
