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
#define _RTW_RF_C_

#include <drv_types.h>
#include <hal_data.h>

u8 center_ch_5g_all[CENTER_CH_5G_ALL_NUM] = {
/* G00 */36, 38, 40,
	42,
/* G01 */44, 46, 48,
	/* 50, */
/* G02 */52, 54, 56,
	58,
/* G03 */60, 62, 64,
/* G04 */100, 102, 104,
	106,
/* G05 */108, 110, 112,
	/* 114, */
/* G06 */116, 118, 120,
	122,
/* G07 */124, 126, 128,
/* G08 */132, 134, 136,
	138,
/* G09 */140, 142, 144,
/* G10 */149, 151, 153,
	155,
/* G11 */157, 159, 161,
	/* 163, */
/* G12 */165, 167, 169,
	171,
/* G13 */173, 175, 177
};


u8 center_ch_5g_20m_40m[CENTER_CH_5G_20M_NUM + CENTER_CH_5G_40M_NUM] = {
/* G00 */36, 38, 40,
/* G01 */44, 46, 48,
/* G02 */52, 54, 56,
/* G03 */60, 62, 64,
/* G04 */100, 102, 104,
/* G05 */108, 110, 112,
/* G06 */116, 118, 120,
/* G07 */124, 126, 128,
/* G08 */132, 134, 136,
/* G09 */140, 142, 144,
/* G10 */149, 151, 153,
/* G11 */157, 159, 161,
/* G12 */165, 167, 169,
/* G13 */173, 175, 177
};

/* Channel layout, frequency, lookup tables, and op-class DB live in core/rtw_rf_rest.c (W3-19..W3-22). */
/* Op-class debug dump helpers live in core/rtw_rf_op_class_dump.c (W3-57). */
/* dump_txpwr_lmt lives in core/rtw_rf_dump_txpwr_lmt.c (W3-58). */
/* kfree TX gain get helper lives in core/rtw_rf_kfree_tx_gain.c (W3-59). */

const u8 _rf_type_to_rf_tx_cnt[RF_TYPE_MAX] = {
	[RF_1T1R] = 1,
	[RF_1T2R] = 1,
	[RF_1T3R] = 1,
	[RF_1T4R] = 1,
	[RF_2T1R] = 2,
	[RF_2T2R] = 2,
	[RF_2T3R] = 2,
	[RF_2T4R] = 2,
	[RF_3T1R] = 3,
	[RF_3T2R] = 3,
	[RF_3T3R] = 3,
	[RF_3T4R] = 3,
	[RF_4T1R] = 4,
	[RF_4T2R] = 4,
	[RF_4T3R] = 4,
	[RF_4T4R] = 4,
};

const u8 _rf_type_to_rf_rx_cnt[RF_TYPE_MAX] = {
	[RF_1T1R] = 1,
	[RF_1T2R] = 2,
	[RF_1T3R] = 3,
	[RF_1T4R] = 4,
	[RF_2T1R] = 1,
	[RF_2T2R] = 2,
	[RF_2T3R] = 3,
	[RF_2T4R] = 4,
	[RF_3T1R] = 1,
	[RF_3T2R] = 2,
	[RF_3T3R] = 3,
	[RF_3T4R] = 4,
	[RF_4T1R] = 1,
	[RF_4T2R] = 2,
	[RF_4T3R] = 3,
	[RF_4T4R] = 4,
};

const char *const _rf_type_to_rfpath_str[RF_TYPE_MAX] = {
	[RF_1T1R] = "RF_1T1R",
	[RF_1T2R] = "RF_1T2R",
	[RF_1T3R] = "RF_1T3R",
	[RF_1T4R] = "RF_1T4R",
	[RF_2T1R] = "RF_2T1R",
	[RF_2T2R] = "RF_2T2R",
	[RF_2T3R] = "RF_2T3R",
	[RF_2T4R] = "RF_2T4R",
	[RF_3T1R] = "RF_3T1R",
	[RF_3T2R] = "RF_3T2R",
	[RF_3T3R] = "RF_3T3R",
	[RF_3T4R] = "RF_3T4R",
	[RF_4T1R] = "RF_4T1R",
	[RF_4T2R] = "RF_4T2R",
	[RF_4T3R] = "RF_4T3R",
	[RF_4T4R] = "RF_4T4R",
};

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
/* config to non N-TX value, path with lower index prefer */
void tx_path_nss_set_default(enum bb_path txpath_nss[], u8 txpath_num_nss[], u8 txpath)
{
	int i, j;
	u8 cnt;

	for (i = 4; i > 0; i--) {
		cnt = 0;
		txpath_nss[i - 1] = 0;
		for (j = 0; j < RF_PATH_MAX; j++) {
			if (txpath & BIT(j)) {
				txpath_nss[i - 1] |= BIT(j);
				if (++cnt == i)
					break;
			}
		}
		txpath_num_nss[i - 1] = i;
	}
}

/* config to full N-TX value */
void tx_path_nss_set_full_tx(enum bb_path txpath_nss[], u8 txpath_num_nss[], u8 txpath)
{
	u8 tx_num = 0;
	int i;

	for (i = 0; i < RF_PATH_MAX; i++)
		if (txpath & BIT(i))
			tx_num++;

	for (i = 4; i > 0; i--) {
		txpath_nss[i - 1] = txpath;
		txpath_num_nss[i - 1] = tx_num;
	}
}
#endif /* !CONFIG_RUST || HOST_RF_TEST */

const char *const _regd_str[] = {
	"NONE",
	"FCC",
	"MKK",
	"ETSI",
	"IC",
	"KCC",
	"NCC",
	"ACMA",
	"CHILE",
	"UKRAINE",
	"MEXICO",
	"CN",
	"WW",
};

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
int rtw_ch_to_bb_gain_sel(int ch)
{
	int sel = -1;

	if (ch >= 1 && ch <= 14)
		sel = BB_GAIN_2G;
#if CONFIG_IEEE80211_BAND_5GHZ
	else if (ch >= 36 && ch < 48)
		sel = BB_GAIN_5GLB1;
	else if (ch >= 52 && ch <= 64)
		sel = BB_GAIN_5GLB2;
	else if (ch >= 100 && ch <= 120)
		sel = BB_GAIN_5GMB1;
	else if (ch >= 124 && ch <= 144)
		sel = BB_GAIN_5GMB2;
	else if (ch >= 149 && ch <= 177)
		sel = BB_GAIN_5GHB;
#endif

	return sel;
}
#endif /* !CONFIG_RUST || HOST_RF_TEST */

