// SPDX-License-Identifier: GPL-2.0
/* L1 C reference: single export rtw_rust_beamforming_enter (host oracle, static helpers). */

#include <string.h>

#include "host_beamforming_bf_init_cmd_oracle.h"
#include "host_beamforming_bf_types.h"

#define BF_HOST_WIRELESS_11_24N 8u
#define BF_HOST_BEAMFORMING_HT_BEAMFORMER_ENABLE (1u << 0)
#define BF_HOST_BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP (0xC0u)
#define BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT 0x2u
#define BF_HOST_BEAMFORMER_CAP_HT_EXPLICIT 0x1u
#define BF_HOST_BEAMFORMEE_CAP 0x10u
#define BF_HOST_SOUNDING_STATE_NONE 0u
#define BF_HOST_SOUNDING_STATE_INIT 1u
#define BF_HOST_MAX_BEAMFORMEE_ENTRY 8u
#define BF_HOST_MAX_NUM_BEAMFORMEE_SU 2u

struct bf_host_sound_sta_cmn {
	u16 aid;
	u16 mac_id;
	u8 g_id;
	u16 p_aid;
};

struct bf_host_sound_sta {
	u8 used;
	u8 wireless_mode;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	struct bf_host_sound_sta_cmn cmn;
};

struct bf_host_sound_stapriv {
	struct bf_host_sound_sta stas[4];
};

struct bf_host_sound_bfee_entry {
	u8 used;
	u16 mac_id;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	u8 cap;
	u8 NumofSoundingDim;
	u8 su_reg_index;
};

struct bf_host_sound_enter_info {
	struct bf_host_sound_bfee_entry bfee_entry[BF_HOST_MAX_BEAMFORMEE_ENTRY];
	u8 beamforming_cap;
	u8 beamformee_su_cnt;
	u32 beamformee_su_reg_maping;
	struct bf_host_sounding_info sounding_info;
};

struct bf_host_sound_enter_adpt {
	struct bf_host_adpt base;
	u16 ht_beamform_cap;
	u16 vht_beamform_cap;
	struct bf_host_sound_stapriv stapriv;
	struct bf_host_sound_enter_info sound;
};

typedef struct bf_host_sound_enter_adpt *bf_host_sound_enter_padpt;

static u32 tr_hal;
static u32 tr_timer_ms;

static struct bf_host_sound_sta *get_stainfo(bf_host_sound_enter_padpt a, u8 *mac)
{
	unsigned i;

	if (!a || !mac)
		return NULL;
	for (i = 0; i < 4; i++) {
		if (!a->stapriv.stas[i].used)
			continue;
		if (!memcmp(a->stapriv.stas[i].mac_addr, mac, BF_HOST_ETH_ALEN))
			return &a->stapriv.stas[i];
	}
	return NULL;
}

static void beamforming_enter(bf_host_sound_enter_padpt adapter, u8 *p)
{
	struct bf_host_sound_sta *sta_copy, *sta;
	struct bf_host_sound_enter_info *info;
	u8 sta_bf_cap, sounding_dim = 0;
	struct bf_host_sound_bfee_entry *bfee;
	u16 ht_bf_cap;
	u8 i;

	if (!adapter || !p)
		return;
	sta_copy = (struct bf_host_sound_sta *)p;
	sta = get_stainfo(adapter, sta_copy->mac_addr);
	if (!sta || (sta->wireless_mode & BF_HOST_WIRELESS_11_24N) == 0)
		return;
	if (adapter->ht_beamform_cap == 0 && adapter->vht_beamform_cap == 0)
		return;
	info = &adapter->sound;
	sta_bf_cap = 0;
	sounding_dim = 0;
	ht_bf_cap = adapter->ht_beamform_cap;
	if (ht_bf_cap & BF_HOST_BEAMFORMING_HT_BEAMFORMER_ENABLE) {
		info->beamforming_cap |= BF_HOST_BEAMFORMER_CAP_HT_EXPLICIT;
		sta_bf_cap |= BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT;
		sounding_dim = (ht_bf_cap & BF_HOST_BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP) >> 6;
	}
	if (sta_bf_cap == BF_HOST_CAP_NONE)
		return;
	if (sta_bf_cap & BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT)
		sta_bf_cap |= BF_HOST_BEAMFORMEE_CAP;
	bfee = NULL;
	for (i = 0; i < BF_HOST_MAX_BEAMFORMEE_ENTRY; i++) {
		if (!info->bfee_entry[i].used) {
			bfee = &info->bfee_entry[i];
			break;
		}
	}
	if (!bfee)
		return;
	bfee->used = 1;
	bfee->mac_id = sta->cmn.mac_id;
	memcpy(bfee->mac_addr, sta->mac_addr, BF_HOST_ETH_ALEN);
	bfee->cap = sta_bf_cap;
	bfee->NumofSoundingDim = sounding_dim;
	if (sta_bf_cap & BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT) {
		info->beamformee_su_cnt += 1;
		for (i = 0; i < BF_HOST_MAX_NUM_BEAMFORMEE_SU; i++) {
			if ((info->beamformee_su_reg_maping & (1u << i)) == 0) {
				info->beamformee_su_reg_maping |= (1u << i);
				bfee->su_reg_index = (u8)i;
				break;
			}
		}
	}
	tr_hal += 1;
	if (info->beamformee_su_cnt && info->sounding_info.state == BF_HOST_SOUNDING_STATE_NONE) {
		info->sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;
		tr_timer_ms = 2000;
	}
}

void rtw_rust_beamforming_enter(bf_host_sound_enter_padpt adapter, u8 *p)
{
	beamforming_enter(adapter, p);
}
