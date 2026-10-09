// SPDX-License-Identifier: GPL-2.0
/*
 * L2 lock-in for the kernel rtw_process_beacon_hint() path.
 *
 * Kernel RT_CHANNEL_INFO is 32 bytes for this driver's config (see
 * test_chplan_kernel_layout.c). A 2-byte {ChannelNum, flags} overlay finds
 * only channel_set[0], so a beacon on a passive channel past index 0 never
 * clears RTW_CHF_NO_IR. This test drives the real 32-byte stride.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;

#define MAX_CHANNEL_NUM 42
#define RTW_CHF_NO_IR (1 << 0)
#define RTW_CHF_DFS (1 << 1)

struct rt_channel_info {
	u8 ChannelNum;
	u8 flags;
	u8 pad0[2];
	u32 rx_count;
	unsigned long non_ocp_end_time;
	u8 hidden_bss_cnt;
	u8 pad1[7];
	void *os_chan;
};

_Static_assert(sizeof(struct rt_channel_info) == 32,
	       "kernel RT_CHANNEL_INFO stride");

/* Rust entry point under test. */
extern u8 rtw_process_beacon_hint(void *adapter, void *bss);

static struct rt_channel_info chset[MAX_CHANNEL_NUM];
static u8 bss_ch;

/* --- chset accessors: the C side owns the stride --- */
u8 rtw_rust_chset_ch_num(struct rt_channel_info *cs, u8 index)
{
	if (index >= MAX_CHANNEL_NUM)
		return 0;
	return cs[index].ChannelNum;
}

u8 rtw_rust_chset_ch_flags(struct rt_channel_info *cs, u8 index)
{
	if (index >= MAX_CHANNEL_NUM)
		return 0;
	return cs[index].flags;
}

void rtw_rust_chset_clear_flags(struct rt_channel_info *cs, u8 index, u8 mask)
{
	if (index >= MAX_CHANNEL_NUM)
		return;
	cs[index].flags &= ~mask;
}

void *rtw_rust_rfctl_channel_set(void *adapter) { (void)adapter; return chset; }
/* NULL country_ent counts as world-wide. */
const void *rtw_rust_rfctl_country_ent(void *adapter) { (void)adapter; return NULL; }
u8 rtw_rust_bss_ds_config(void *bss) { (void)bss; return bss_ch; }
void rtw_rust_chplan_beacon_hint_info(u8 ch) { (void)ch; }

/* --- remaining externs of rust/rtw_chplan_rest.rs; unused here --- */
struct chplan_ent {
	u8 regd_2g;
	u8 chd_2g;
	u8 regd_5g;
	u8 chd_5g;
};

const struct chplan_ent RTW_ChannelPlanMap[1] = { { 0, 0, 0, 0 } };
const int RTW_ChannelPlanMap_size = 1;

_Bool rtw_is_channel_plan_valid(u8 id) { (void)id; return 0; }
int rtw_ch2freq(int ch) { (void)ch; return 0; }
u8 rtw_chdef_2g_len(u8 chd) { (void)chd; return 0; }
u8 rtw_chdef_2g_ch(u8 chd, u8 i) { (void)chd; (void)i; return 0; }
u8 rtw_chdef_2g_attrib(u8 chd) { (void)chd; return 0; }
u8 rtw_chdef_5g_len(u8 chd) { (void)chd; return 0; }
u8 rtw_chdef_5g_ch(u8 chd, u8 i) { (void)chd; (void)i; return 0; }
u8 rtw_chdef_5g_attrib(u8 chd) { (void)chd; return 0; }
void rtw_rust_chplan_print_str(void *sel, const u8 *s) { (void)sel; (void)s; }

/* --- fixture --- */
struct ch_seed {
	u8 ch;
	u8 flags;
};

/* World-wide style plan: 12/13 and the 5 GHz channels are passive. */
static const struct ch_seed ww_plan[] = {
	{ 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 },
	{ 7, 0 }, { 8, 0 }, { 9, 0 }, { 10, 0 }, { 11, 0 },
	{ 12, RTW_CHF_NO_IR }, { 13, RTW_CHF_NO_IR },
	{ 36, RTW_CHF_NO_IR }, { 40, RTW_CHF_NO_IR },
	{ 52, RTW_CHF_DFS | RTW_CHF_NO_IR },
	{ 149, RTW_CHF_NO_IR }, { 165, RTW_CHF_NO_IR },
};

#define NSEED (sizeof(ww_plan) / sizeof(ww_plan[0]))

static struct rt_channel_info golden[MAX_CHANNEL_NUM];

static void build_chset(void)
{
	size_t i;

	memset(chset, 0, sizeof(chset));
	for (i = 0; i < MAX_CHANNEL_NUM; i++) {
		/* Poison every field a 2-byte overlay could mistake for data. */
		memset(chset[i].pad0, 0xAB, sizeof(chset[i].pad0));
		memset(chset[i].pad1, 0xCD, sizeof(chset[i].pad1));
		chset[i].rx_count = 0xDEADBEEFu;
		chset[i].non_ocp_end_time = ~0UL;
		chset[i].hidden_bss_cnt = 0xEF;
		chset[i].os_chan = (void *)(uintptr_t)(0x1000u + (unsigned)i);
	}
	for (i = 0; i < NSEED; i++) {
		chset[i].ChannelNum = ww_plan[i].ch;
		chset[i].flags = ww_plan[i].flags;
	}
	memcpy(golden, chset, sizeof(chset));
}

static int run(const char *name, u8 ch, u8 expect_ret, int changed_idx)
{
	u8 ret;

	build_chset();
	bss_ch = ch;
	ret = rtw_process_beacon_hint((void *)1, (void *)1);
	if (changed_idx >= 0)
		golden[changed_idx].flags &= ~RTW_CHF_NO_IR;
	if (ret != expect_ret) {
		fprintf(stderr, "FAIL %s: ret %u != %u\n", name, ret, expect_ret);
		return 1;
	}
	if (memcmp(chset, golden, sizeof(chset)) != 0) {
		fprintf(stderr, "FAIL %s: channel_set mismatch\n", name);
		return 1;
	}
	printf("PASS %s\n", name);
	return 0;
}

int main(void)
{
	int bad = 0;

	bad += run("hint_ch12_passive", 12, 1, 11);
	bad += run("hint_ch13_passive", 13, 1, 12);
	bad += run("hint_ch36_passive", 36, 1, 13);
	bad += run("hint_ch165_last", 165, 1, 17);
	bad += run("no_hint_ch1_11", 6, 0, -1);
	bad += run("no_hint_dfs", 52, 0, -1);
	bad += run("no_hint_absent", 100, 0, -1);
	bad += run("no_hint_ch0", 0, 0, -1);
	if (bad)
		return 1;
	printf("PASS: beacon hint walks the 32-byte kernel chset\n");
	return 0;
}
