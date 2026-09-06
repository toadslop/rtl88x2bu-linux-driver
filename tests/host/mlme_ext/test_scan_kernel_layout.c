// SPDX-License-Identifier: GPL-2.0
/*
 * L2 lock-in for kernel rtw_scan_backop_decision.
 *
 * Host L2 overlays a 6-byte packed mi_state
 * {sta_num, ld_sta_num, ap_num, ld_ap_num, mesh_num, ld_mesh_num}.
 * Kernel struct mi_state (CONFIG_AP_MODE=y, CONFIG_TDLS=n) is:
 *   sta_num@0, ld_sta_num@1, lg_sta_num@2, ap_num@3,
 *   starting_ap_num@4, ld_ap_num@5, ...
 * and rtw_mi_status memsets sizeof(struct mi_state) (~16+ bytes).
 *
 * Overlaying the stub on the kernel path:
 *   1) stack-smashes past the 6-byte Rust local (memset of full mi_state)
 *   2) reads AP counts from lg_sta_num / ap_num instead of ap_num / ld_ap_num
 *
 * This test compiles rust/rtw_mlme_ext_scan.rs with the kernel cfgs
 * (rust_mlme_ext_scan, no host_mlme_ext_scan_test) and poisons the
 * overlay slots so a leftover rtw_mi_status path cannot pass.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define SS_BACKOP_EN (1 << 0)
#define SS_BACKOP_EN_NL (1 << 1)

/* Kernel mi_state prefix for this driver's default config. */
struct kernel_mi_state {
	u8 sta_num;
	u8 ld_sta_num;
	u8 lg_sta_num;		/* overlay put ap_num here */
	u8 ap_num;		/* overlay put ld_ap_num here */
	u8 starting_ap_num;
	u8 ld_ap_num;
	u8 adhoc_num;
	u8 ld_adhoc_num;
	u8 scan_num;
	u8 scan_enter_num;
	u8 uwps_num;
	u8 roch_num;
	u8 mgmt_tx_num;
	u8 p2p_device_num;
	u8 p2p_gc;
	u8 p2p_go;
};

_Static_assert(__builtin_offsetof(struct kernel_mi_state, lg_sta_num) == 2,
	       "lg_sta_num is the host-overlay ap_num slot");
_Static_assert(__builtin_offsetof(struct kernel_mi_state, ap_num) == 3,
	       "ap_num is the host-overlay ld_ap_num slot");
_Static_assert(__builtin_offsetof(struct kernel_mi_state, ld_ap_num) == 5,
	       "ld_ap_num sits after starting_ap_num");

u8 rtw_scan_backop_decision(void *adapter);

static struct kernel_mi_state g_mi;
static u8 g_flags_sta;
static u8 g_flags_ap;
static int g_mi_status_called;

void rtw_rust_scan_mi_counts(void *adapter,
			     u8 *sta_num, u8 *ld_sta_num,
			     u8 *ap_num, u8 *ld_ap_num,
			     u8 *mesh_num, u8 *ld_mesh_num)
{
	(void)adapter;
	if (sta_num)
		*sta_num = g_mi.sta_num;
	if (ld_sta_num)
		*ld_sta_num = g_mi.ld_sta_num;
	if (ap_num)
		*ap_num = g_mi.ap_num;
	if (ld_ap_num)
		*ld_ap_num = g_mi.ld_ap_num;
	if (mesh_num)
		*mesh_num = 0;
	if (ld_mesh_num)
		*ld_mesh_num = 0;
}

/* Kernel path must not call this — doing so is the overlay/stack-smash bug. */
void rtw_mi_status(void *adapter, void *mstate)
{
	(void)adapter;
	(void)mstate;
	g_mi_status_called = 1;
	fprintf(stderr, "FAIL: kernel path called rtw_mi_status (MiState overlay)\n");
	exit(1);
}

u8 rtw_rust_scan_backop_flags_sta(void *adapter)
{
	(void)adapter;
	return g_flags_sta;
}

u8 rtw_rust_scan_backop_flags_ap(void *adapter)
{
	(void)adapter;
	return g_flags_ap;
}

/* Unused on the backop-only kernel-layout path; keep the link happy. */
int rtw_mi_busy_traffic_check(void *a) { (void)a; return 0; }
int rtw_mi_check_miracast_enabled(void *a) { (void)a; return 0; }
unsigned long rtw_rust_scan_last_scan_time(void *a) { (void)a; return 0; }
void rtw_rust_scan_set_last_scan_time(void *a, unsigned long t) { (void)a; (void)t; }
u32 rtw_rust_scan_wireless_mode(void *a) { (void)a; return 0; }
u16 rtw_rust_scan_ch_ms(void *a) { (void)a; return 0; }
u16 rtw_rust_scan_duration(void *a) { (void)a; return 0; }
u8 rtw_rust_scan_cnt_max(void *a) { (void)a; return 1; }
u16 rtw_rust_scan_backop_ms(void *a) { (void)a; return 0; }
void rtw_rust_scan_set_timeout_ms(void *a, u32 ms) { (void)a; (void)ms; }
u16 rtw_rust_scan_acs_adv_ms(void *a) { (void)a; return 0; }
unsigned long _rtw_get_current_time(void) { return 0; }
u32 _rtw_get_passing_time_ms(unsigned long s) { (void)s; return 0; }

static int fail(const char *msg)
{
	fprintf(stderr, "FAIL: %s\n", msg);
	return 1;
}

static void reset_fixture(void)
{
	memset(&g_mi, 0, sizeof(g_mi));
	g_flags_sta = 0;
	g_flags_ap = 0;
	g_mi_status_called = 0;
	/* Poison the host-overlay slots so a leftover 6-byte walk cannot
	 * accidentally match the real AP counts. */
	g_mi.lg_sta_num = 0xaa;
	g_mi.starting_ap_num = 0xbb;
}

int main(void)
{
	u8 dummy;
	u8 got;

	/* AP with clients: ld_ap_num@5 and ap_num@3. Overlay would read
	 * ld_ap_num from ap_num@3 (also 1) AND ap_num from lg_sta_num@2
	 * (poison 0xaa) — SS_BACKOP_EN_NL would still fire, so this case
	 * alone is not enough. Combined with the poison-only case below. */
	reset_fixture();
	g_mi.ap_num = 1;
	g_mi.ld_ap_num = 1;
	g_flags_ap = SS_BACKOP_EN | SS_BACKOP_EN_NL;
	got = rtw_scan_backop_decision(&dummy);
	if (g_mi_status_called)
		return fail("rtw_mi_status was called");
	if (got != g_flags_ap)
		return fail("AP-loaded backop missed (ld_ap_num@5 not read)");

	/* Idle AP + SS_BACKOP_EN only: C checks ld_ap_num (0) → no backop.
	 * Overlay reads ld_ap_num from ap_num@3 (1) → false SS_BACKOP_EN. */
	reset_fixture();
	g_mi.ap_num = 1;
	g_mi.ld_ap_num = 0;
	g_flags_ap = SS_BACKOP_EN;
	got = rtw_scan_backop_decision(&dummy);
	if (got != 0)
		return fail("idle AP + SS_BACKOP_EN used overlay ap_num as ld_ap_num");

	/* Idle AP + SS_BACKOP_EN_NL: C checks ap_num@3 (1) → backop.
	 * Overlay reads ap_num from lg_sta_num@2 (poison) → also backop,
	 * so this is the positive AP-NL case. */
	reset_fixture();
	g_mi.ap_num = 1;
	g_mi.ld_ap_num = 0;
	g_flags_ap = SS_BACKOP_EN_NL;
	got = rtw_scan_backop_decision(&dummy);
	if (got != SS_BACKOP_EN_NL)
		return fail("idle AP + SS_BACKOP_EN_NL missed ap_num@3");

	/* Poison-only: lg_sta_num@2 = 1, no AP. Overlay would treat that
	 * as ap_num and fire SS_BACKOP_EN_NL. */
	reset_fixture();
	g_mi.lg_sta_num = 1;
	g_mi.ap_num = 0;
	g_mi.ld_ap_num = 0;
	g_flags_ap = SS_BACKOP_EN_NL;
	got = rtw_scan_backop_decision(&dummy);
	if (got != 0)
		return fail("SS_BACKOP_EN_NL fired from poisoned lg_sta_num overlay");

	/* STA linked still works (offsets 0/1 match). */
	reset_fixture();
	g_mi.sta_num = 1;
	g_mi.ld_sta_num = 1;
	g_flags_sta = SS_BACKOP_EN;
	got = rtw_scan_backop_decision(&dummy);
	if (got != SS_BACKOP_EN)
		return fail("STA-linked backop missed");

	printf("PASS: kernel mi_state stride (AP counts via C shim, overlay poison ignored)\n");
	printf("  sizeof(kernel_mi_state_prefix)=%zu lg_sta@%zu ap@%zu ld_ap@%zu\n",
	       sizeof(g_mi),
	       (size_t)__builtin_offsetof(struct kernel_mi_state, lg_sta_num),
	       (size_t)__builtin_offsetof(struct kernel_mi_state, ap_num),
	       (size_t)__builtin_offsetof(struct kernel_mi_state, ld_ap_num));
	return 0;
}
