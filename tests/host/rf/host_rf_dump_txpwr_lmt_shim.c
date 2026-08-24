// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-58 dump_txpwr_lmt L2 oracles. */

#include <stdlib.h>
#include <string.h>

#include "host_rf_dump_txpwr_lmt_types.h"

struct host_sel_capture host_sel_out;

static struct hal_spec_t host_rf_hal_spec = {
	.txgi_max = 63,
	.txgi_pdbm = 2,
	.rfpath_num_2g = 2,
	.rfpath_num_5g = 2,
};

const char *const _regd_str[] = {
	"NONE", "FCC", "MKK", "ETSI", "IC", "KCC", "NCC",
	"ACMA", "CHILE", "UKRAINE", "MEXICO", "CN", "WW",
};

const char *const _txpwr_lmt_rs_str[] = {
	"CCK", "OFDM", "HT", "VHT", "N/A",
};

u8 center_ch_5g_all[CENTER_CH_5G_ALL_NUM] = {
	36, 38, 40, 42, 44, 46, 48, 52, 54, 56, 58, 60, 62, 64,
	100, 102, 104, 106, 108, 110, 112, 116, 118, 120, 122,
	124, 126, 128, 132, 134, 136, 138, 140, 142, 144, 149,
	151, 153, 155, 157, 159, 161, 165, 167, 169, 171, 173,
	175, 177,
};

static u8 host_target_txpwr = 20;

struct hal_spec_t *host_rf_hal_spec_ptr(void)
{
	return &host_rf_hal_spec;
}

void *rtw_malloc(u32 sz)
{
	return malloc(sz);
}

void rtw_mfree(void *p, u32 sz)
{
	(void)sz;
	free(p);
}

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

void *rtw_zvmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_vmfree(u8 *p, u32 sz)
{
	(void)sz;
	free(p);
}

void host_rf_dump_txpwr_lmt_set_target_txpwr(u8 target)
{
	host_target_txpwr = target;
}

void host_rf_dump_txpwr_lmt_reset(struct _adapter *adapter)
{
	memset(adapter, 0, sizeof(*adapter));
	_rtw_init_listhead(&adapter->rf_ctl.reg_exc_list);
	_rtw_init_listhead(&adapter->rf_ctl.txpwr_lmt_list);
	adapter->rf_ctl.regd_name = regd_str(TXPWR_LMT_FCC);
	adapter->hal_data.max_tx_cnt = 1;
	adapter->band_cap = BAND_CAP_2G | BAND_CAP_5G;
	adapter->jaguar = 1;
	host_rf_hal_spec.rfpath_num_2g = 1;
	host_rf_hal_spec.rfpath_num_5g = 1;
	host_target_txpwr = 20;
}

bool hal_is_band_support(struct _adapter *adapter, u8 band)
{
	if (band == BAND_ON_2_4G)
		return !!(adapter->band_cap & BAND_CAP_2G);
	if (band == BAND_ON_5G)
		return !!(adapter->band_cap & BAND_CAP_5G);
	return false;
}

static s8 host_ent_lmt(struct txpwr_lmt_ent *ent, u8 band, u8 bw, u8 tlrs,
		       u8 ntx_idx, u8 ch)
{
	u8 ch_idx;

	if (!ent)
		return host_rf_hal_spec.txgi_max;

	if (band == BAND_ON_2_4G) {
		if (ch < 1 || ch > CENTER_CH_2G_NUM)
			return host_rf_hal_spec.txgi_max;
		ch_idx = ch - 1;
		return ent->lmt_2g[bw][tlrs][ch_idx][ntx_idx];
	}
#if CONFIG_IEEE80211_BAND_5GHZ
	if (band == BAND_ON_5G) {
		for (ch_idx = 0; ch_idx < center_chs_5g_num(bw); ch_idx++) {
			if (center_chs_5g(bw, ch_idx) == ch)
				return ent->lmt_5g[bw][tlrs - 1][ch_idx][ntx_idx];
		}
	}
#endif
	return host_rf_hal_spec.txgi_max;
}

s8 phy_get_txpwr_lmt(struct _adapter *adapter, const char *regd_name, u8 band,
		     u8 bw, u8 tlrs, u8 ntx_idx, u8 ch, u8 lock)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	struct txpwr_lmt_ent *ent;

	(void)lock;
	ent = _rtw_txpwr_lmt_get_by_name(rfctl, regd_name);
	return host_ent_lmt(ent, band, bw, tlrs, ntx_idx, ch);
}

s8 phy_get_txpwr_lmt_diff(struct _adapter *adapter, const char *regd_name,
			  u8 band, u8 bw, u8 rfpath, u8 rs, u8 tlrs,
			  u8 ntx_idx, u8 ch, u8 lock)
{
	s8 lmt;

	(void)adapter;
	(void)rfpath;
	(void)rs;
	(void)lock;

	lmt = phy_get_txpwr_lmt(adapter, regd_name, band, bw, tlrs, ntx_idx, ch, 0);
	if (lmt == host_rf_hal_spec.txgi_max)
		return host_rf_hal_spec.txgi_max;
	return lmt - (s8)host_target_txpwr;
}

u8 phy_get_target_txpwr(struct _adapter *adapter, u8 band, u8 rfpath, u8 rs)
{
	(void)adapter;
	(void)band;
	(void)rfpath;
	(void)rs;
	return host_target_txpwr;
}
