// SPDX-License-Identifier: GPL-2.0
/*
 * L2 lock-in for the kernel rtw_chplan DFS helpers.
 *
 * Kernel RT_CHANNEL_INFO is 32 bytes for this driver's config
 * (ChannelNum@0, flags@1, rx_count@4 from CONFIG_FIND_BEST_CHANNEL,
 * non_ocp_end_time@8 from CONFIG_DFS_MASTER, hidden_bss_cnt@16,
 * os_chan@24 from CONFIG_IOCTL_CFG80211) -- not the packed 2-byte
 * {ChannelNum, flags} pair the host oracle uses.
 *
 * A 2-byte-stride overlay reads entry 1 out of entry 0's padding, sees
 * ChannelNum == 0 and stops after the first channel, so every DFS lookup
 * beyond channel_set[0] answers "not DFS". This test drives the real
 * 32-byte stride and fails if the helpers only see the first entry.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t s32;

#define MAX_CHANNEL_NUM 59
#define RTW_CHF_NO_IR (1 << 0)
#define RTW_CHF_DFS (1 << 1)
#define CHANNEL_WIDTH_20 0

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
_Static_assert(__builtin_offsetof(struct rt_channel_info, non_ocp_end_time) == 8,
	       "non_ocp_end_time offset");
_Static_assert(__builtin_offsetof(struct rt_channel_info, hidden_bss_cnt) == 16,
	       "hidden_bss_cnt offset");
_Static_assert(__builtin_offsetof(struct rt_channel_info, os_chan) == 24,
	       "os_chan offset");

/* Rust entry points under test. */
extern _Bool rtw_chset_is_dfs_ch(struct rt_channel_info *chset, u8 ch);
extern _Bool rtw_chset_is_dfs_range(struct rt_channel_info *chset, u32 hi, u32 lo);
extern _Bool rtw_chset_is_dfs_chbw(struct rt_channel_info *chset, u8 ch, u8 bw,
				   u8 offset);

/* --- chset accessors: the C side owns the stride --- */
u8 rtw_rust_chset_ch_num(struct rt_channel_info *chset, u8 index)
{
	if (index >= MAX_CHANNEL_NUM)
		return 0;
	return chset[index].ChannelNum;
}

u8 rtw_rust_chset_ch_flags(struct rt_channel_info *chset, u8 index)
{
	if (index >= MAX_CHANNEL_NUM)
		return 0;
	return chset[index].flags;
}

/* --- RF helpers (real 2.4/5 GHz centre-frequency maths) --- */
static int ch2freq(int ch)
{
	if (ch >= 1 && ch <= 13)
		return 2407 + ch * 5;
	if (ch == 14)
		return 2484;
	return 5000 + ch * 5;
}

int rtw_freq2ch(int freq)
{
	if (freq == 2484)
		return 14;
	if (freq >= 2412 && freq <= 2472)
		return (freq - 2407) / 5;
	if (freq >= 5000)
		return (freq - 5000) / 5;
	return 0;
}

_Bool rtw_chbw_to_freq_range(u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo)
{
	int cfreq;

	(void)offset;
	if (bw != CHANNEL_WIDTH_20 || ch == 0)
		return 0;
	cfreq = ch2freq(ch);
	*hi = (u32)cfreq + 10;
	*lo = (u32)cfreq - 10;
	return 1;
}

/* --- remaining externs of rust/rtw_chplan.rs; unused by these helpers --- */
struct chplan_ent {
	u8 regd_2g;
	u8 chd_2g;
	u8 regd_5g;
	u8 chd_5g;
};

struct country_chplan_ent {
	u8 alpha2[2];
	u8 chplan;
	u8 en_11ac;
};

const struct chplan_ent RTW_ChannelPlanMap[1] = { { 0, 0, 0, 0 } };
const int RTW_ChannelPlanMap_size = 1;
const struct country_chplan_ent country_chplan_map[1] = { { { '0', '0' }, 0, 0 } };
const unsigned int rtw_country_chplan_map_size = 1;

void rtw_chplan_warn_regd_mismatch(u8 id, u8 regd_2g, u8 regd_5g)
{
	(void)id;
	(void)regd_2g;
	(void)regd_5g;
}

u8 rtw_chdef_2g_len(u8 chd) { (void)chd; return 0; }
u8 rtw_chdef_2g_ch(u8 chd, u8 i) { (void)chd; (void)i; return 0; }
u8 rtw_chdef_2g_attrib(u8 chd) { (void)chd; return 0; }
u8 rtw_chdef_5g_len(u8 chd) { (void)chd; return 0; }
u8 rtw_chdef_5g_ch(u8 chd, u8 i) { (void)chd; (void)i; return 0; }
u8 rtw_chdef_5g_attrib(u8 chd) { (void)chd; return 0; }

u8 rtw_rust_rfctl_channel_plan(void *adapter) { (void)adapter; return 0; }
u8 rtw_rust_rfctl_regd_src(void *adapter) { (void)adapter; return 0; }
void *rtw_rust_rfctl_channel_set(void *adapter) { (void)adapter; return NULL; }
u8 rtw_rust_regsty_wireless_mode(void *adapter) { (void)adapter; return 0; }
void *rtw_rust_adapter_regsty(void *adapter) { (void)adapter; return NULL; }
_Bool hal_chk_band_cap(void *adapter, u8 cap) { (void)adapter; (void)cap; return 0; }

void rtw_rust_chset_zero(struct rt_channel_info *chset)
{
	memset(chset, 0, sizeof(*chset) * MAX_CHANNEL_NUM);
}

void rtw_rust_chset_write(struct rt_channel_info *chset, u8 index, u8 ch, u8 flags)
{
	chset[index].ChannelNum = ch;
	chset[index].flags = flags;
}

void rtw_rust_chset_set_non_ocp(struct rt_channel_info *chset, u8 count)
{
	(void)chset;
	(void)count;
}

void rtw_rust_warn_on(int condition) { (void)condition; }

/* --- fixture --- */
struct ch_seed {
	u8 ch;
	u8 flags;
};

/* US-style 2.4 GHz + 5 GHz plan: 52/56/60/64 and 100/104 are DFS. */
static const struct ch_seed us_plan[] = {
	{ 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 },
	{ 7, 0 }, { 8, 0 }, { 9, 0 }, { 10, 0 }, { 11, 0 },
	{ 36, 0 }, { 40, 0 }, { 44, 0 }, { 48, 0 },
	{ 52, RTW_CHF_DFS | RTW_CHF_NO_IR },
	{ 56, RTW_CHF_DFS | RTW_CHF_NO_IR },
	{ 60, RTW_CHF_DFS | RTW_CHF_NO_IR },
	{ 64, RTW_CHF_DFS | RTW_CHF_NO_IR },
	{ 100, RTW_CHF_DFS | RTW_CHF_NO_IR },
	{ 104, RTW_CHF_DFS | RTW_CHF_NO_IR },
	{ 149, 0 }, { 153, 0 }, { 157, 0 }, { 161, 0 }, { 165, 0 },
};

static void build_chset(struct rt_channel_info *chset,
			const struct ch_seed *seed, size_t n)
{
	size_t i;

	memset(chset, 0, sizeof(*chset) * MAX_CHANNEL_NUM);
	for (i = 0; i < MAX_CHANNEL_NUM; i++) {
		/* Poison every field a 2-byte overlay could mistake for data. */
		memset(chset[i].pad0, 0xAB, sizeof(chset[i].pad0));
		memset(chset[i].pad1, 0xCD, sizeof(chset[i].pad1));
		chset[i].rx_count = 0xDEADBEEFu;
		chset[i].non_ocp_end_time = ~0UL;
		chset[i].hidden_bss_cnt = 0xEF;
		chset[i].os_chan = (void *)(uintptr_t)(0x1000u + (unsigned)i);
	}
	for (i = 0; i < n; i++) {
		chset[i].ChannelNum = seed[i].ch;
		chset[i].flags = seed[i].flags;
	}
	if (n < MAX_CHANNEL_NUM)
		chset[n].ChannelNum = 0;
}

static int check(const char *what, int got, int want)
{
	if (got == want)
		return 0;
	fprintf(stderr, "FAIL %s: got %d want %d\n", what, got, want);
	return 1;
}

int main(void)
{
	struct rt_channel_info chset[MAX_CHANNEL_NUM];
	int bad = 0;

	build_chset(chset, us_plan, sizeof(us_plan) / sizeof(us_plan[0]));

	/* DFS entries live well past index 0; a 2-byte stride never reaches them. */
	bad |= check("is_dfs_ch(52)", rtw_chset_is_dfs_ch(chset, 52), 1);
	bad |= check("is_dfs_ch(56)", rtw_chset_is_dfs_ch(chset, 56), 1);
	bad |= check("is_dfs_ch(100)", rtw_chset_is_dfs_ch(chset, 100), 1);
	bad |= check("is_dfs_ch(104)", rtw_chset_is_dfs_ch(chset, 104), 1);

	/* Present but non-DFS. */
	bad |= check("is_dfs_ch(1)", rtw_chset_is_dfs_ch(chset, 1), 0);
	bad |= check("is_dfs_ch(11)", rtw_chset_is_dfs_ch(chset, 11), 0);
	bad |= check("is_dfs_ch(36)", rtw_chset_is_dfs_ch(chset, 36), 0);
	bad |= check("is_dfs_ch(149)", rtw_chset_is_dfs_ch(chset, 149), 0);

	/* Absent from the plan. */
	bad |= check("is_dfs_ch(140)", rtw_chset_is_dfs_ch(chset, 140), 0);

	/* BW20 coverage of a DFS channel vs a non-DFS one. */
	bad |= check("is_dfs_chbw(52,20)",
		     rtw_chset_is_dfs_chbw(chset, 52, CHANNEL_WIDTH_20, 0), 1);
	bad |= check("is_dfs_chbw(100,20)",
		     rtw_chset_is_dfs_chbw(chset, 100, CHANNEL_WIDTH_20, 0), 1);
	bad |= check("is_dfs_chbw(36,20)",
		     rtw_chset_is_dfs_chbw(chset, 36, CHANNEL_WIDTH_20, 0), 0);
	bad |= check("is_dfs_chbw(149,20)",
		     rtw_chset_is_dfs_chbw(chset, 149, CHANNEL_WIDTH_20, 0), 0);

	/* 5250-5350 MHz spans DFS channels 52..64. */
	bad |= check("is_dfs_range(5350,5250)",
		     rtw_chset_is_dfs_range(chset, 5350, 5250), 1);
	/* 5170-5250 MHz spans only non-DFS 36..48. */
	bad |= check("is_dfs_range(5250,5170)",
		     rtw_chset_is_dfs_range(chset, 5250, 5170), 0);

	/* A 2.4 GHz-only plan must still report no DFS. */
	{
		static const struct ch_seed g_only[] = {
			{ 1, 0 }, { 6, 0 }, { 11, 0 },
		};

		build_chset(chset, g_only, sizeof(g_only) / sizeof(g_only[0]));
		bad |= check("2g_only is_dfs_ch(6)", rtw_chset_is_dfs_ch(chset, 6), 0);
		bad |= check("2g_only is_dfs_range(5350,5250)",
			     rtw_chset_is_dfs_range(chset, 5350, 5250), 0);
	}

	if (!bad)
		printf("PASS kernel-layout chplan DFS (32-byte RT_CHANNEL_INFO stride)\n");
	return bad;
}
