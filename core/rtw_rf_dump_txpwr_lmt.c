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
#define _RTW_RF_DUMP_TXPWR_LMT_C_

#ifdef HOST_RF_DUMP_TXPWR_LMT_TEST
#include "host_rf_dump_txpwr_lmt_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if CONFIG_TXPWR_LIMIT
#if !defined(CONFIG_RUST) || defined(HOST_RF_DUMP_TXPWR_LMT_TEST) || !defined(CONFIG_RUST_RF_DUMP_TXPWR_LMT)

void _dump_regd_exc_list(void *sel, struct rf_ctl_t *rfctl);

void dump_txpwr_lmt(void *sel, _adapter *adapter)
{
#define TMP_STR_LEN 16
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(adapter);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(adapter);
	_irqL irqL;
	char fmt[16];
	char tmp_str[TMP_STR_LEN];
	s8 *lmt_idx = NULL;
	int bw, band, ch_num, tlrs, ntx_idx, rs, i, path;
	u8 ch, n, rfpath_num;

	_enter_critical_mutex(&rfctl->txpwr_lmt_mutex, &irqL);

	_dump_regd_exc_list(sel, rfctl);
	RTW_PRINT_SEL(sel, "\n");

	if (!rfctl->txpwr_regd_num)
		goto release_lock;

	lmt_idx = rtw_malloc(sizeof(s8) * RF_PATH_MAX * rfctl->txpwr_regd_num);
	if (!lmt_idx) {
		RTW_ERR("%s alloc fail\n", __func__);
		goto release_lock;
	}

	RTW_PRINT_SEL(sel, "txpwr_lmt_2g_cck_ofdm_state:0x%02x\n", rfctl->txpwr_lmt_2g_cck_ofdm_state);
	#if CONFIG_IEEE80211_BAND_5GHZ
	if (IS_HARDWARE_TYPE_JAGUAR_ALL(adapter)) {
		RTW_PRINT_SEL(sel, "txpwr_lmt_5g_cck_ofdm_state:0x%02x\n", rfctl->txpwr_lmt_5g_cck_ofdm_state);
		RTW_PRINT_SEL(sel, "txpwr_lmt_5g_20_40_ref:0x%02x\n", rfctl->txpwr_lmt_5g_20_40_ref);
	}
	#endif
	RTW_PRINT_SEL(sel, "\n");

	for (band = BAND_ON_2_4G; band <= BAND_ON_5G; band++) {
		if (!hal_is_band_support(adapter, band))
			continue;

		rfpath_num = (band == BAND_ON_2_4G ? hal_spec->rfpath_num_2g : hal_spec->rfpath_num_5g);

		for (bw = 0; bw < MAX_5G_BANDWIDTH_NUM; bw++) {

			if (bw >= CHANNEL_WIDTH_160)
				break;
			if (band == BAND_ON_2_4G && bw >= CHANNEL_WIDTH_80)
				break;

			if (band == BAND_ON_2_4G)
				ch_num = CENTER_CH_2G_NUM;
			else
				ch_num = center_chs_5g_num(bw);

			if (ch_num == 0) {
				rtw_warn_on(1);
				break;
			}

			for (tlrs = TXPWR_LMT_RS_CCK; tlrs < TXPWR_LMT_RS_NUM; tlrs++) {

				if (band == BAND_ON_2_4G && tlrs == TXPWR_LMT_RS_VHT)
					continue;
				if (band == BAND_ON_5G && tlrs == TXPWR_LMT_RS_CCK)
					continue;
				if (bw > CHANNEL_WIDTH_20 && (tlrs == TXPWR_LMT_RS_CCK || tlrs == TXPWR_LMT_RS_OFDM))
					continue;
				if (bw > CHANNEL_WIDTH_40 && tlrs == TXPWR_LMT_RS_HT)
					continue;
				if (tlrs == TXPWR_LMT_RS_VHT && !IS_HARDWARE_TYPE_JAGUAR_ALL(adapter))
					continue;

				for (ntx_idx = RF_1TX; ntx_idx < MAX_TX_COUNT; ntx_idx++) {
					struct txpwr_lmt_ent *ent;
					_list *cur, *head;

					if (ntx_idx + 1 > hal_data->max_tx_cnt)
						continue;

					/* bypass CCK multi-TX is not defined */
					if (tlrs == TXPWR_LMT_RS_CCK && ntx_idx > RF_1TX) {
						if (band == BAND_ON_2_4G
							&& !(rfctl->txpwr_lmt_2g_cck_ofdm_state & (TXPWR_LMT_HAS_CCK_1T << ntx_idx)))
							continue;
					}

					/* bypass OFDM multi-TX is not defined */
					if (tlrs == TXPWR_LMT_RS_OFDM && ntx_idx > RF_1TX) {
						if (band == BAND_ON_2_4G
							&& !(rfctl->txpwr_lmt_2g_cck_ofdm_state & (TXPWR_LMT_HAS_OFDM_1T << ntx_idx)))
							continue;
						#if CONFIG_IEEE80211_BAND_5GHZ
						if (band == BAND_ON_5G
							&& !(rfctl->txpwr_lmt_5g_cck_ofdm_state & (TXPWR_LMT_HAS_OFDM_1T << ntx_idx)))
							continue;
						#endif
					}

					/* bypass 5G 20M, 40M pure reference */
					#if CONFIG_IEEE80211_BAND_5GHZ
					if (band == BAND_ON_5G && (bw == CHANNEL_WIDTH_20 || bw == CHANNEL_WIDTH_40)) {
						if (rfctl->txpwr_lmt_5g_20_40_ref == TXPWR_LMT_REF_HT_FROM_VHT) {
							if (tlrs == TXPWR_LMT_RS_HT)
								continue;
						} else if (rfctl->txpwr_lmt_5g_20_40_ref == TXPWR_LMT_REF_VHT_FROM_HT) {
							if (tlrs == TXPWR_LMT_RS_VHT && bw <= CHANNEL_WIDTH_40)
								continue;
						}
					}
					#endif

					/* choose n-SS mapping rate section to get lmt diff value */
					if (tlrs == TXPWR_LMT_RS_CCK)
						rs = CCK;
					else if (tlrs == TXPWR_LMT_RS_OFDM)
						rs = OFDM;
					else if (tlrs == TXPWR_LMT_RS_HT)
						rs = HT_1SS + ntx_idx;
					else if (tlrs == TXPWR_LMT_RS_VHT)
						rs = VHT_1SS + ntx_idx;
					else {
						RTW_ERR("%s invalid tlrs %u\n", __func__, tlrs);
						continue;
					}

					RTW_PRINT_SEL(sel, "[%s][%s][%s][%uT]\n"
						, band_str(band)
						, ch_width_str(bw)
						, txpwr_lmt_rs_str(tlrs)
						, ntx_idx + 1
					);

					/* header for limit in db */
					RTW_PRINT_SEL(sel, "%3s ", "ch");

					head = &rfctl->txpwr_lmt_list;
					cur = get_next(head);
					while ((rtw_end_of_queue_search(head, cur)) == _FALSE) {
						ent = LIST_CONTAINOR(cur, struct txpwr_lmt_ent, list);
						cur = get_next(cur);

						sprintf(fmt, "%%%zus%%s ", strlen(ent->regd_name) >= 6 ? 1 : 6 - strlen(ent->regd_name));
						snprintf(tmp_str, TMP_STR_LEN, fmt
							, strcmp(ent->regd_name, rfctl->regd_name) == 0 ? "*" : ""
							, ent->regd_name);
						_RTW_PRINT_SEL(sel, "%s", tmp_str);
					}
					sprintf(fmt, "%%%zus%%s ", strlen(regd_str(TXPWR_LMT_WW)) >= 6 ? 1 : 6 - strlen(regd_str(TXPWR_LMT_WW)));
					snprintf(tmp_str, TMP_STR_LEN, fmt
						, strcmp(rfctl->regd_name, regd_str(TXPWR_LMT_WW)) == 0 ? "*" : ""
						, regd_str(TXPWR_LMT_WW));
					_RTW_PRINT_SEL(sel, "%s", tmp_str);

					/* header for limit offset */
					for (path = 0; path < RF_PATH_MAX; path++) {
						if (path >= rfpath_num)
							break;
						_RTW_PRINT_SEL(sel, "|");
						head = &rfctl->txpwr_lmt_list;
						cur = get_next(head);
						while ((rtw_end_of_queue_search(head, cur)) == _FALSE) {
							ent = LIST_CONTAINOR(cur, struct txpwr_lmt_ent, list);
							cur = get_next(cur);
							_RTW_PRINT_SEL(sel, "%3c "
								, strcmp(ent->regd_name, rfctl->regd_name) == 0 ? rf_path_char(path) : ' ');
						}
						_RTW_PRINT_SEL(sel, "%3c "
								, strcmp(rfctl->regd_name, regd_str(TXPWR_LMT_WW)) == 0 ? rf_path_char(path) : ' ');
					}
					_RTW_PRINT_SEL(sel, "\n");

					for (n = 0; n < ch_num; n++) {
						s8 lmt;
						s8 lmt_offset;
						u8 base;

						if (band == BAND_ON_2_4G)
							ch = n + 1;
						else
							ch = center_chs_5g(bw, n);

						if (ch == 0) {
							rtw_warn_on(1);
							break;
						}

						/* dump limit in dBm */
						RTW_PRINT_SEL(sel, "%3u ", ch);
						head = &rfctl->txpwr_lmt_list;
						cur = get_next(head);
						while ((rtw_end_of_queue_search(head, cur)) == _FALSE) {
							ent = LIST_CONTAINOR(cur, struct txpwr_lmt_ent, list);
							cur = get_next(cur);
							lmt = phy_get_txpwr_lmt(adapter, ent->regd_name, band, bw, tlrs, ntx_idx, ch, 0);
							txpwr_idx_get_dbm_str(lmt, hal_spec->txgi_max, hal_spec->txgi_pdbm, strlen(ent->regd_name), tmp_str, TMP_STR_LEN);
							_RTW_PRINT_SEL(sel, "%s ", tmp_str);
						}
						lmt = phy_get_txpwr_lmt(adapter, regd_str(TXPWR_LMT_WW), band, bw, tlrs, ntx_idx, ch, 0);
						txpwr_idx_get_dbm_str(lmt, hal_spec->txgi_max, hal_spec->txgi_pdbm, strlen(regd_str(TXPWR_LMT_WW)), tmp_str, TMP_STR_LEN);
						_RTW_PRINT_SEL(sel, "%s ", tmp_str);

						/* dump limit offset of each path */
						for (path = RF_PATH_A; path < RF_PATH_MAX; path++) {
							if (path >= rfpath_num)
								break;

							base = phy_get_target_txpwr(adapter, band, path, rs);

							_RTW_PRINT_SEL(sel, "|");
							head = &rfctl->txpwr_lmt_list;
							cur = get_next(head);
							i = 0;
							while ((rtw_end_of_queue_search(head, cur)) == _FALSE) {
								ent = LIST_CONTAINOR(cur, struct txpwr_lmt_ent, list);
								cur = get_next(cur);
								lmt_offset = phy_get_txpwr_lmt_diff(adapter, ent->regd_name, band, bw, path, rs, tlrs, ntx_idx, ch, 0);
								if (lmt_offset == hal_spec->txgi_max) {
									*(lmt_idx + i * RF_PATH_MAX + path) = hal_spec->txgi_max;
									_RTW_PRINT_SEL(sel, "%3s ", "NA");
								} else {
									*(lmt_idx + i * RF_PATH_MAX + path) = lmt_offset + base;
									_RTW_PRINT_SEL(sel, "%3d ", lmt_offset);
								}
								i++;
							}
							lmt_offset = phy_get_txpwr_lmt_diff(adapter, regd_str(TXPWR_LMT_WW), band, bw, path, rs, tlrs, ntx_idx, ch, 0);
							if (lmt_offset == hal_spec->txgi_max)
								_RTW_PRINT_SEL(sel, "%3s ", "NA");
							else
								_RTW_PRINT_SEL(sel, "%3d ", lmt_offset);

						}

						/* compare limit_idx of each path, print 'x' when mismatch */
						if (rfpath_num > 1) {
							for (i = 0; i < rfctl->txpwr_regd_num; i++) {
								for (path = 0; path < RF_PATH_MAX; path++) {
									if (path >= rfpath_num)
										break;
									if (*(lmt_idx + i * RF_PATH_MAX + path) != *(lmt_idx + i * RF_PATH_MAX + ((path + 1) % rfpath_num)))
										break;
								}
								if (path >= rfpath_num)
									_RTW_PRINT_SEL(sel, " ");
								else
									_RTW_PRINT_SEL(sel, "x");
							}
						}
						_RTW_PRINT_SEL(sel, "\n");

					}
					RTW_PRINT_SEL(sel, "\n");
				}
			} /* loop for rate sections */
		} /* loop for bandwidths */
	} /* loop for bands */

	if (lmt_idx)
		rtw_mfree(lmt_idx, sizeof(s8) * RF_PATH_MAX * rfctl->txpwr_regd_num);

release_lock:
	_exit_critical_mutex(&rfctl->txpwr_lmt_mutex, &irqL);
}

#endif /* !CONFIG_RUST || HOST_RF_DUMP_TXPWR_LMT_TEST || !CONFIG_RUST_RF_DUMP_TXPWR_LMT */
#endif /* CONFIG_TXPWR_LIMIT */
