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
#define _RTW_VHT_REST_C_

#ifdef HOST_VHT_TEST
#include <stdint.h>
typedef uint8_t u8;
#else
#include <drv_types.h>
#endif

#ifdef CONFIG_80211AC_VHT

void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target_mcs_map, u8 *cur_mcs_map)
{
	u8	i, j;
	u8	cur_rate, target_rate;

	for (i = 0; i < 2; i++) {
		target_mcs_map[i] = 0;
		for (j = 0; j < 8; j += 2) {
			cur_rate = (cur_mcs_map[i] >> j) & 3;
			if (cur_rate == 3) /* 0x3 indicates not supported that num of SS */
				target_rate = 3;
			else if (nss <= ((j / 2) + i * 4))
				target_rate = 3;
			else
				target_rate = cur_rate;

			target_mcs_map[i] |= (target_rate << j);
		}
	}
}

#ifdef ROKU_PRIVATE
u8 VHT_get_ss_from_map(u8 *vht_mcs_map)
{
	u8 i, j;
	u8 ss = 0;

	for (i = 0; i < 2; i++) {
		if (vht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				if (((vht_mcs_map[i] >> j) & 0x03) == 0x03)
					break;
				ss++;
			}
		}
	}

	return ss;
}
#endif /* ROKU_PRIVATE */

#endif /* CONFIG_80211AC_VHT */
