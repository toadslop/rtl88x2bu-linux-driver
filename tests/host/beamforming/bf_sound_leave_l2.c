// SPDX-License-Identifier: GPL-2.0
/* W3-193 PR1: host C oracle + L2 for _beamforming_leave (SU BFee teardown). */

#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_init_cmd_oracle.h"
#include "host_beamforming_bf_types.h"

#define BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT 0x2u
#define BF_HOST_BEAMFORMEE_CAP 0x10u
#define BF_HOST_SOUNDING_STATE_NONE 0u
#define BF_HOST_SOUNDING_STATE_INIT 1u
#define BF_HOST_MAX_BEAMFORMEE_ENTRY 8u

struct bf_host_sound_bfee_entry {
	u8 used;
	u16 mac_id;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	u8 cap;
	u8 NumofSoundingDim;
	u8 su_reg_index;
};

struct bf_host_sound_leave_info {
	struct bf_host_sound_bfee_entry bfee_entry[BF_HOST_MAX_BEAMFORMEE_ENTRY];
	u8 beamformee_su_cnt;
	u8 beamformee_mu_cnt;
	struct bf_host_sounding_info sounding_info;
};

struct bf_host_sound_leave_adpt {
	struct bf_host_adpt base;
	struct bf_host_sound_leave_info sound;
};

typedef struct bf_host_sound_leave_adpt *bf_host_sound_leave_padpt;

u32 bf_host_sound_leave_tr_hal;
u32 bf_host_sound_leave_tr_cancel_timer;

static struct bf_host_sound_bfee_entry *o_leave_find_bfee(bf_host_sound_leave_padpt a, u8 *ra)
{
	u8 i;

	if (!a || !ra)
		return NULL;
	for (i = 0; i < BF_HOST_MAX_BEAMFORMEE_ENTRY; i++) {
		if (!a->sound.bfee_entry[i].used)
			continue;
		if (!memcmp(a->sound.bfee_entry[i].mac_addr, ra, BF_HOST_ETH_ALEN))
			return &a->sound.bfee_entry[i];
	}
	return NULL;
}

static void o_leave_bfee_remove(bf_host_sound_leave_padpt a, struct bf_host_sound_bfee_entry *entry)
{
	struct bf_host_sound_leave_info *info = &a->sound;

	if (entry->cap & BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT) {
		if (info->beamformee_su_cnt)
			info->beamformee_su_cnt -= 1;
	}
	memset(entry, 0, sizeof(*entry));
}

#ifndef HOST_BF_SOUND_LEAVE_RUST
void o_beamforming_leave(bf_host_sound_leave_padpt adapter, u8 *ra)
{
	struct bf_host_sound_leave_info *info;
	struct bf_host_sound_bfee_entry *bfee;

	if (!adapter || !ra)
		return;
	info = &adapter->sound;
	bfee = o_leave_find_bfee(adapter, ra);
	if (!bfee)
		return;

	o_leave_bfee_remove(adapter, bfee);
	bf_host_sound_leave_tr_hal += 1;

	if (info->beamformee_su_cnt == 0 && info->beamformee_mu_cnt == 0) {
		bf_host_sound_leave_tr_cancel_timer += 1;
		bf_host_sounding_init(&info->sounding_info);
	}
}
#endif

void o_beamforming_leave(bf_host_sound_leave_padpt adapter, u8 *ra);

static int run_leave_vector(void)
{
	struct bf_host_sound_leave_adpt g_adpt;
	u8 ra[BF_HOST_ETH_ALEN] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	int fail = 0;

	memset(&g_adpt, 0, sizeof(g_adpt));
	g_adpt.sound.bfee_entry[0].used = 1;
	memcpy(g_adpt.sound.bfee_entry[0].mac_addr, ra, BF_HOST_ETH_ALEN);
	g_adpt.sound.bfee_entry[0].cap = BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT | BF_HOST_BEAMFORMEE_CAP;
	g_adpt.sound.beamformee_su_cnt = 1;
	g_adpt.sound.sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;

	bf_host_sound_leave_tr_hal = 0;
	bf_host_sound_leave_tr_cancel_timer = 0;

	o_beamforming_leave(&g_adpt, ra);

	if (g_adpt.sound.bfee_entry[0].used != 0) {
		fprintf(stderr, "leave: bfee still used\n");
		fail = 1;
	}
	if (g_adpt.sound.beamformee_su_cnt != 0) {
		fprintf(stderr, "leave: su_cnt=%u\n", g_adpt.sound.beamformee_su_cnt);
		fail = 1;
	}
	if (g_adpt.sound.sounding_info.state != BF_HOST_SOUNDING_STATE_NONE) {
		fprintf(stderr, "leave: sounding state=%u\n", g_adpt.sound.sounding_info.state);
		fail = 1;
	}
	if (bf_host_sound_leave_tr_hal != 1) {
		fprintf(stderr, "leave: tr_hal=%u\n", bf_host_sound_leave_tr_hal);
		fail = 1;
	}
	if (bf_host_sound_leave_tr_cancel_timer != 1) {
		fprintf(stderr, "leave: tr_cancel=%u\n", bf_host_sound_leave_tr_cancel_timer);
		fail = 1;
	}
	return fail;
}

int main(void)
{
	if (run_leave_vector())
		return 1;
	printf("bf_sound_leave_l2: ok\n");
	return 0;
}
