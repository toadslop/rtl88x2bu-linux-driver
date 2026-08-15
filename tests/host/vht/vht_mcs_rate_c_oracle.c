// SPDX-License-Identifier: GPL-2.0
/* C oracle slice: VHT MCS map helpers from core/rtw_vht.c (W3-45 PR1). */

#include <stdint.h>

typedef uint8_t u8;

#define MGN_VHT1SS_MCS7 0xA7

u8 rtw_get_vht_highest_rate(u8 *pvht_mcs_map)
{
	u8 i, j;
	u8 bit_map;
	u8 vht_mcs_rate = 0;

	for (i = 0; i < 2; i++) {
		if (pvht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				bit_map = (pvht_mcs_map[i] >> j) & 3;

				if (bit_map != 3)
					vht_mcs_rate = MGN_VHT1SS_MCS7 + 10 * j / 2 +
						       i * 40 + bit_map;
			}
		}
	}

	return vht_mcs_rate;
}

u8 rtw_vht_mcsmap_to_nss(u8 *pvht_mcs_map)
{
	u8 i, j;
	u8 bit_map;
	u8 nss = 0;

	for (i = 0; i < 2; i++) {
		if (pvht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				bit_map = (pvht_mcs_map[i] >> j) & 3;

				if (bit_map != 3)
					nss++;
			}
		}
	}

	return nss;
}
