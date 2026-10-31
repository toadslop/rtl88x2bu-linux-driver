// SPDX-License-Identifier: GPL-2.0
/* W3-193 PR1: host C oracle + L2 for _beamforming_leave.
 *
 * Scope: SU BFee lookup/remove + HAL leave trace + sounding teardown when
 * beamformee_su_cnt and beamformee_mu_cnt both hit zero (mirrors kernel slice
 * exercised before PR3). BFer slot removal in kernel _beamforming_leave is
 * out of scope here; PR3 kernel wiring must not assume BFer parity from this
 * oracle alone.
 */

#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_init_cmd_oracle.h"
#include "host_beamforming_bf_types.h"

#define BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT 0x2u
#define BF_HOST_BEAMFORMEE_CAP_VHT_SU 0x8u
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

#ifndef HOST_BF_SOUND_LEAVE_RUST
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
	u8 su_mask = BF_HOST_BEAMFORMEE_CAP_VHT_SU | BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT;

	if (entry->cap & su_mask) {
		if (info->beamformee_su_cnt)
			info->beamformee_su_cnt -= 1;
	}
	memset(entry, 0, sizeof(*entry));
}

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

static int expect_leave_traces(u32 hal, u32 cancel)
{
	if (bf_host_sound_leave_tr_hal != hal) {
		fprintf(stderr, "leave: tr_hal=%u expected %u\n", bf_host_sound_leave_tr_hal, hal);
		return 1;
	}
	if (bf_host_sound_leave_tr_cancel_timer != cancel) {
		fprintf(stderr, "leave: tr_cancel=%u expected %u\n",
			bf_host_sound_leave_tr_cancel_timer, cancel);
		return 1;
	}
	return 0;
}

static int run_leave_last_su_bfee(void)
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
		fprintf(stderr, "leave last: bfee still used\n");
		fail = 1;
	}
	if (g_adpt.sound.beamformee_su_cnt != 0) {
		fprintf(stderr, "leave last: su_cnt=%u\n", g_adpt.sound.beamformee_su_cnt);
		fail = 1;
	}
	if (g_adpt.sound.sounding_info.state != BF_HOST_SOUNDING_STATE_NONE) {
		fprintf(stderr, "leave last: sounding state=%u\n", g_adpt.sound.sounding_info.state);
		fail = 1;
	}
	fail |= expect_leave_traces(1, 1);
	return fail;
}

static int run_leave_unknown_ra(void)
{
	struct bf_host_sound_leave_adpt g_adpt;
	u8 ra[BF_HOST_ETH_ALEN] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	u8 other[BF_HOST_ETH_ALEN] = {0x02, 0xaa, 0xbb, 0xcc, 0xdd, 0xee};

	memset(&g_adpt, 0, sizeof(g_adpt));
	g_adpt.sound.bfee_entry[0].used = 1;
	memcpy(g_adpt.sound.bfee_entry[0].mac_addr, ra, BF_HOST_ETH_ALEN);
	g_adpt.sound.beamformee_su_cnt = 1;
	g_adpt.sound.sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;

	bf_host_sound_leave_tr_hal = 0;
	bf_host_sound_leave_tr_cancel_timer = 0;

	o_beamforming_leave(&g_adpt, other);

	if (g_adpt.sound.bfee_entry[0].used == 0) {
		fprintf(stderr, "leave unknown: bfee cleared unexpectedly\n");
		return 1;
	}
	if (g_adpt.sound.sounding_info.state != BF_HOST_SOUNDING_STATE_INIT) {
		fprintf(stderr, "leave unknown: sounding reset\n");
		return 1;
	}
	return expect_leave_traces(0, 0);
}

static int run_leave_one_of_two_bfee(void)
{
	struct bf_host_sound_leave_adpt g_adpt;
	u8 ra0[BF_HOST_ETH_ALEN] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	u8 ra1[BF_HOST_ETH_ALEN] = {0x02, 0x66, 0x77, 0x88, 0x99, 0xaa};

	memset(&g_adpt, 0, sizeof(g_adpt));
	g_adpt.sound.bfee_entry[0].used = 1;
	memcpy(g_adpt.sound.bfee_entry[0].mac_addr, ra0, BF_HOST_ETH_ALEN);
	g_adpt.sound.bfee_entry[0].cap = BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT | BF_HOST_BEAMFORMEE_CAP;
	g_adpt.sound.bfee_entry[1].used = 1;
	memcpy(g_adpt.sound.bfee_entry[1].mac_addr, ra1, BF_HOST_ETH_ALEN);
	g_adpt.sound.bfee_entry[1].cap = BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT | BF_HOST_BEAMFORMEE_CAP;
	g_adpt.sound.beamformee_su_cnt = 2;
	g_adpt.sound.sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;

	bf_host_sound_leave_tr_hal = 0;
	bf_host_sound_leave_tr_cancel_timer = 0;

	o_beamforming_leave(&g_adpt, ra0);

	if (g_adpt.sound.bfee_entry[0].used != 0) {
		fprintf(stderr, "leave partial: slot0 still used\n");
		return 1;
	}
	if (g_adpt.sound.bfee_entry[1].used == 0) {
		fprintf(stderr, "leave partial: slot1 cleared\n");
		return 1;
	}
	if (g_adpt.sound.beamformee_su_cnt != 1) {
		fprintf(stderr, "leave partial: su_cnt=%u\n", g_adpt.sound.beamformee_su_cnt);
		return 1;
	}
	if (g_adpt.sound.sounding_info.state != BF_HOST_SOUNDING_STATE_INIT) {
		fprintf(stderr, "leave partial: sounding reset\n");
		return 1;
	}
	return expect_leave_traces(1, 0);
}

static int run_leave_vht_su_cap_only(void)
{
	struct bf_host_sound_leave_adpt g_adpt;
	u8 ra[BF_HOST_ETH_ALEN] = {0x02, 0xde, 0xad, 0xbe, 0xef, 0x00};

	memset(&g_adpt, 0, sizeof(g_adpt));
	g_adpt.sound.bfee_entry[0].used = 1;
	memcpy(g_adpt.sound.bfee_entry[0].mac_addr, ra, BF_HOST_ETH_ALEN);
	g_adpt.sound.bfee_entry[0].cap = BF_HOST_BEAMFORMEE_CAP_VHT_SU | BF_HOST_BEAMFORMEE_CAP;
	g_adpt.sound.beamformee_su_cnt = 1;
	g_adpt.sound.sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;

	bf_host_sound_leave_tr_hal = 0;
	bf_host_sound_leave_tr_cancel_timer = 0;

	o_beamforming_leave(&g_adpt, ra);

	if (g_adpt.sound.beamformee_su_cnt != 0) {
		fprintf(stderr, "leave vht: su_cnt=%u\n", g_adpt.sound.beamformee_su_cnt);
		return 1;
	}
	return expect_leave_traces(1, 1);
}

static int run_leave_mu_cnt_blocks_timer(void)
{
	struct bf_host_sound_leave_adpt g_adpt;
	u8 ra[BF_HOST_ETH_ALEN] = {0x02, 0x12, 0x34, 0x56, 0x78, 0x9a};

	memset(&g_adpt, 0, sizeof(g_adpt));
	g_adpt.sound.bfee_entry[0].used = 1;
	memcpy(g_adpt.sound.bfee_entry[0].mac_addr, ra, BF_HOST_ETH_ALEN);
	g_adpt.sound.bfee_entry[0].cap = BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT | BF_HOST_BEAMFORMEE_CAP;
	g_adpt.sound.beamformee_su_cnt = 1;
	g_adpt.sound.beamformee_mu_cnt = 1;
	g_adpt.sound.sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;

	bf_host_sound_leave_tr_hal = 0;
	bf_host_sound_leave_tr_cancel_timer = 0;

	o_beamforming_leave(&g_adpt, ra);

	if (g_adpt.sound.beamformee_su_cnt != 0) {
		fprintf(stderr, "leave mu: su_cnt=%u\n", g_adpt.sound.beamformee_su_cnt);
		return 1;
	}
	if (g_adpt.sound.sounding_info.state != BF_HOST_SOUNDING_STATE_INIT) {
		fprintf(stderr, "leave mu: sounding reset with mu_cnt>0\n");
		return 1;
	}
	return expect_leave_traces(1, 0);
}

int main(void)
{
	if (run_leave_last_su_bfee())
		return 1;
	if (run_leave_unknown_ra())
		return 1;
	if (run_leave_one_of_two_bfee())
		return 1;
	if (run_leave_vht_su_cap_only())
		return 1;
	if (run_leave_mu_cnt_blocks_timer())
		return 1;
	printf("bf_sound_leave_l2: ok\n");
	return 0;
}
