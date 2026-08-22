// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-56 op_class_pref L2 oracles. */

#include <stdlib.h>
#include <string.h>

#include "host_rf_op_class_pref_types.h"

static struct hal_spec_t host_hal_spec = {
	.bw_cap = BW_CAP_20M | BW_CAP_40M | BW_CAP_80M | BW_CAP_160M,
};

static u8 host_band_cap = BAND_CAP_2G | BAND_CAP_5G;
static s16 host_reg_max_txpwr_mbm = 2000;
static u8 host_dfs_domain_unknown = _TRUE;

struct hal_spec_t *host_rf_op_class_pref_hal_spec(void)
{
	return &host_hal_spec;
}

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_mfree(void *p, u32 sz)
{
	(void)sz;
	free(p);
}

bool hal_chk_band_cap(_adapter *adapter, u8 cap)
{
	(void)adapter;
	return (host_band_cap & cap) != 0;
}

s16 rtw_rfctl_get_reg_max_txpwr_mbm(struct rf_ctl_t *rfctl, u8 ch, u8 bw,
				    u8 offset, bool eirp)
{
	(void)rfctl;
	(void)ch;
	(void)bw;
	(void)offset;
	(void)eirp;
	return host_reg_max_txpwr_mbm;
}

u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl)
{
	(void)rfctl;
	return host_dfs_domain_unknown;
}

int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, const u32 ch)
{
	int i;

	if (ch == 0)
		return -1;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (ch == ch_set[i].ChannelNum)
			return i;
	}

	return -1;
}

void host_rf_op_class_pref_set_hal(u8 band_cap, u8 bw_cap)
{
	host_band_cap = band_cap;
	host_hal_spec.bw_cap = bw_cap;
}

void host_rf_op_class_pref_set_txpwr(s16 mbm)
{
	host_reg_max_txpwr_mbm = mbm;
}

void host_rf_op_class_pref_set_dfs_unknown(u8 unknown)
{
	host_dfs_domain_unknown = unknown;
}

void host_rf_op_class_pref_reset(_adapter *adapter)
{
	memset(adapter, 0, sizeof(*adapter));
	adapter->registrypriv.wireless_mode = WIRELESS_MODE_24G | WIRELESS_MODE_5G;
	adapter->registrypriv.bw_mode = (CHANNEL_WIDTH_160 << 4) | CHANNEL_WIDTH_40;
	adapter->registrypriv.vht_enable = 1;
	host_band_cap = BAND_CAP_2G | BAND_CAP_5G;
	host_hal_spec.bw_cap = BW_CAP_20M | BW_CAP_40M | BW_CAP_80M | BW_CAP_160M;
	host_reg_max_txpwr_mbm = 2000;
	host_dfs_domain_unknown = _TRUE;
}

struct op_class_pref_t *host_rf_op_class_pref_by_class_id(_adapter *adapter,
							   u8 class_id)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	int i;

	if (!rfctl->spt_op_class_ch)
		return NULL;

	for (i = 0; i < global_op_class_num; i++) {
		if (!rfctl->spt_op_class_ch[i])
			continue;
		if (rfctl->spt_op_class_ch[i]->class_id == class_id)
			return rfctl->spt_op_class_ch[i];
	}

	return NULL;
}
