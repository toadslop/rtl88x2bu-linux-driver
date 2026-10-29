// SPDX-License-Identifier: GPL-2.0
/* W3-192 PR1: host C oracle + L2 for _beamforming_enter (SU BFee path). */

#include <stdio.h>
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
	u8 mac_addr[BF_HOST_ETH_ALEN];
	u16 aid;
	u16 mac_id;
	u8 g_id;
	u16 p_aid;
};

struct bf_host_sound_sta {
	u8 used;
	u8 wireless_mode;
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

struct bf_host_sound_enter_tr {
	u32 hal_sounding_enter;
	u32 sounding_timer_ms;
};

static struct bf_host_sound_enter_tr bf_host_sound_enter_tr;

static struct bf_host_sound_sta *bf_host_sound_get_stainfo(bf_host_sound_enter_padpt a, u8 *mac)
{
	unsigned i;

	if (!a || !mac)
		return NULL;
	for (i = 0; i < 4; i++) {
		if (!a->stapriv.stas[i].used)
			continue;
		if (!memcmp(a->stapriv.stas[i].cmn.mac_addr, mac, BF_HOST_ETH_ALEN))
			return &a->stapriv.stas[i];
	}
	return NULL;
}

#ifndef HOST_BF_SOUND_ENTER_RUST
static void o_sound_get_sta_beamform_cap(bf_host_sound_enter_padpt adapter, struct bf_host_sound_sta *sta,
					 u8 *sta_bf_cap, u8 *sounding_dim)
{
	struct bf_host_sound_enter_info *info = &adapter->sound;
	u16 ht_bf_cap;

	*sta_bf_cap = 0;
	*sounding_dim = 0;
	if ((sta->wireless_mode & BF_HOST_WIRELESS_11_24N) == 0)
		return;
	ht_bf_cap = adapter->ht_beamform_cap;
	if (ht_bf_cap & BF_HOST_BEAMFORMING_HT_BEAMFORMER_ENABLE) {
		info->beamforming_cap |= BF_HOST_BEAMFORMER_CAP_HT_EXPLICIT;
		*sta_bf_cap |= BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT;
		*sounding_dim = (ht_bf_cap & BF_HOST_BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP) >> 6;
	}
}

static struct bf_host_sound_bfee_entry *o_sound_bfee_add_entry(
	bf_host_sound_enter_padpt adapter, struct bf_host_sound_sta *sta, u8 bf_cap, u8 sounding_dim)
{
	struct bf_host_sound_enter_info *info = &adapter->sound;
	struct bf_host_sound_bfee_entry *bfee = NULL;
	u8 i;

	for (i = 0; i < BF_HOST_MAX_BEAMFORMEE_ENTRY; i++) {
		if (!info->bfee_entry[i].used) {
			bfee = &info->bfee_entry[i];
			break;
		}
	}
	if (!bfee)
		return NULL;
	bfee->used = 1;
	bfee->mac_id = sta->cmn.mac_id;
	memcpy(bfee->mac_addr, sta->cmn.mac_addr, BF_HOST_ETH_ALEN);
	bfee->cap = bf_cap;
	bfee->NumofSoundingDim = sounding_dim;
	if (bf_cap & BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT) {
		info->beamformee_su_cnt += 1;
		for (i = 0; i < BF_HOST_MAX_NUM_BEAMFORMEE_SU; i++) {
			if ((info->beamformee_su_reg_maping & (1u << i)) == 0) {
				info->beamformee_su_reg_maping |= (1u << i);
				bfee->su_reg_index = (u8)i;
				break;
			}
		}
	}
	return bfee;
}

void o_beamforming_enter(bf_host_sound_enter_padpt adapter, u8 *p)
{
	struct bf_host_sound_sta *sta_copy, *sta;
	struct bf_host_sound_enter_info *info;
	u8 sta_bf_cap, sounding_dim = 0;
	struct bf_host_sound_bfee_entry *bfee;

	if (!adapter || !p)
		return;
	sta_copy = (struct bf_host_sound_sta *)p;
	sta = bf_host_sound_get_stainfo(adapter, sta_copy->cmn.mac_addr);
	if (!sta || (sta->wireless_mode & BF_HOST_WIRELESS_11_24N) == 0)
		return;
	if (adapter->ht_beamform_cap == 0 && adapter->vht_beamform_cap == 0)
		return;
	info = &adapter->sound;
	o_sound_get_sta_beamform_cap(adapter, sta, &sta_bf_cap, &sounding_dim);
	if (sta_bf_cap == BF_HOST_CAP_NONE)
		return;
	if (sta_bf_cap & BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT)
		sta_bf_cap |= BF_HOST_BEAMFORMEE_CAP;
	bfee = o_sound_bfee_add_entry(adapter, sta, sta_bf_cap, sounding_dim);
	if (!bfee)
		return;
	bf_host_sound_enter_tr.hal_sounding_enter += 1;
	if (info->beamformee_su_cnt && info->sounding_info.state == BF_HOST_SOUNDING_STATE_NONE) {
		info->sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;
		bf_host_sound_enter_tr.sounding_timer_ms = 2000;
	}
}

static struct bf_host_sound_enter_adpt g_adpt;

static void reset_adpt(u16 ht_cap, u8 sta_used, u8 wm)
{
	u8 mac[6] = {0xaa, 0, 0, 0, 0, 1};

	memset(&g_adpt, 0, sizeof(g_adpt));
	memset(&bf_host_sound_enter_tr, 0, sizeof(bf_host_sound_enter_tr));
	o_bf_init(&g_adpt.base);
	bf_host_sounding_init(&g_adpt.sound.sounding_info);
	g_adpt.ht_beamform_cap = ht_cap;
	if (sta_used) {
		g_adpt.stapriv.stas[0].used = 1;
		g_adpt.stapriv.stas[0].wireless_mode = wm;
		memcpy(g_adpt.stapriv.stas[0].cmn.mac_addr, mac, 6);
		g_adpt.stapriv.stas[0].cmn.mac_id = 3;
	}
}

static int run_enter(u16 ht_cap, u8 sta_used, u8 wm, int x_hal, int x_bfee, int x_state, int x_ms,
		     const char *name)
{
	struct bf_host_sound_sta stub;
	u8 mac[6] = {0xaa, 0, 0, 0, 0, 1};

	reset_adpt(ht_cap, sta_used, wm);
	memcpy(stub.cmn.mac_addr, mac, 6);
	o_beamforming_enter(&g_adpt, (u8 *)&stub);
	if ((int)bf_host_sound_enter_tr.hal_sounding_enter != x_hal ||
	    (int)g_adpt.sound.beamformee_su_cnt != x_bfee ||
	    (x_state && (int)g_adpt.sound.sounding_info.state != x_state) ||
	    (x_ms && (int)bf_host_sound_enter_tr.sounding_timer_ms != x_ms)) {
		fprintf(stderr, "FAIL %s\n", name);
		return 1;
	}
	printf("PASS %s\n", name);
	return 0;
}

int main(void)
{
	int bad = 0;

	bad += run_enter(0, 0, 8, 0, 0, 0, 0, "enter_no_sta");
	bad += run_enter(0, 1, 72, 0, 0, 0, 0, "enter_no_bf_cap");
	bad += run_enter(1, 1, 8, 1, 1, 1, 2000, "enter_ht_bfee_su");
	if (!bad)
		printf("PASS 3 vectors (builtin)\n");
	return bad ? 1 : 0;
}
#endif
