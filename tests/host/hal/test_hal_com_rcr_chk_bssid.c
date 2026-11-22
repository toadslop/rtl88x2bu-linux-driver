// SPDX-License-Identifier: GPL-2.0
/* L2 table tests for W4-128 rtw_hal_rcr_set_chk_bssid policy. */

#include "host_hal_com_rcr_chk_bssid_types.h"
#include <stdio.h>

static int failures;

static void fail(const char *tag)
{
	fprintf(stderr, "FAIL %s\n", tag);
	failures++;
}

static void expect_u32(const char *tag, u32 a, u32 b)
{
	if (a != b)
		fail(tag);
}

static void expect_rcr_bssid_bits(const char *tag, u32 rcr, u32 want_data,
				  u32 want_bcn)
{
	u32 data = (rcr & RCR_CBSSID_DATA) ? 1u : 0u;
	u32 bcn = (rcr & RCR_CBSSID_BCN) ? 1u : 0u;

	if (data != want_data || bcn != want_bcn)
		fail(tag);
}

static void expect_rcr_aap(const char *tag, u32 rcr, u32 want)
{
	u32 aap = (rcr & RCR_AAP) ? 1u : 0u;

	if (aap != want)
		fail(tag);
}

static void run_case(const char *tag, u32 initial_rcr,
		   const struct mi_state *no_self,
		   const struct mi_state *no_others, u8 self_action,
		   u32 want_data, u32 want_bcn, unsigned int want_sets)
{
	struct _adapter *a = host_hal_rcr_chk_adapter();
	u32 got;

	host_hal_rcr_chk_reset();
	host_hal_rcr_chk_set_mi_no_self(no_self);
	host_hal_rcr_chk_set_mi_no_others(no_others);
	host_hal_rcr_chk_set_initial_rcr(initial_rcr);

	rtw_hal_rcr_set_chk_bssid(a, self_action);
	got = host_hal_rcr_chk_last_set_rcr();
	if (want_sets == 0) {
		if (host_hal_rcr_trace.set_count != 0)
			fail(tag);
		expect_rcr_bssid_bits(tag, initial_rcr, want_data, want_bcn);
		return;
	}

	expect_u32(tag, host_hal_rcr_trace.set_count, want_sets);
	expect_rcr_bssid_bits(tag, got, want_data, want_bcn);
}

int main(void)
{
	struct mi_state zero = {0};
	struct mi_state ap_self = {0};

	ap_self.ap_num = 1;

	/* Idle STA: both BSSID filter bits on. */
	run_case("idle", RCR_CBSSID_BCN | RCR_CBSSID_DATA, &zero, &zero,
		 MLME_ACTION_NONE, 1, 1, 0);

	/* Scan enter clears BCN; DATA clears when CONFIG_FIND_BEST_CHANNEL is on. */
	run_case("scan_enter", RCR_CBSSID_BCN | RCR_CBSSID_DATA, &zero, &zero,
		 MLME_SCAN_ENTER, 0, 0, 1);

	/* CTA clears both BSSID filter bits. */
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_in_cta_test(1);
		host_hal_rcr_chk_set_initial_rcr(RCR_CBSSID_BCN | RCR_CBSSID_DATA);
		rtw_hal_rcr_set_chk_bssid(a, MLME_ACTION_NONE);
		expect_rcr_bssid_bits("cta", host_hal_rcr_chk_last_set_rcr(), 0, 0);
	}

	/* AP + wifi_spec clears beacon bit unless connecting sets lg_sta. */
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_mi_no_self(&ap_self);
		host_hal_rcr_chk_set_mi_no_others(&zero);
		host_hal_rcr_chk_set_wifi_spec(1);
		host_hal_rcr_chk_set_initial_rcr(RCR_CBSSID_DATA);
		rtw_hal_rcr_set_chk_bssid(a, MLME_STA_CONNECTING);
		expect_rcr_bssid_bits("ap_wifi_connecting", host_hal_rcr_chk_last_set_rcr(),
				      0, 1);
	}

	/* Periodic TSF etime keeps beacon filter on. */
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_mi_no_self(&zero);
		host_hal_rcr_chk_set_mi_no_others(&zero);
		host_hal_rcr_chk_set_periodic_tsf(1UL << 32);
		host_hal_rcr_chk_set_initial_rcr(0);
		rtw_hal_rcr_set_chk_bssid(a, MLME_ACTION_NONE);
		expect_rcr_bssid_bits("periodic_tsf_hi", host_hal_rcr_chk_last_set_rcr(),
				      1, 1);
	}

	/* TSF pause when beacon bit falls and rx_tsf_filter is clear. */
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_rx_tsf_filter(0);
		host_hal_rcr_chk_set_initial_rcr(RCR_CBSSID_BCN | RCR_CBSSID_DATA);
		rtw_hal_rcr_set_chk_bssid(a, MLME_SCAN_ENTER);
		expect_u32("tsf_pause_on_scan", host_hal_rcr_trace.tsf_pause_count, 1u);
		expect_u32("tsf_restore_skip_scan", host_hal_rcr_trace.tsf_restore_count,
			   0u);
	}

	/* TSF restore when beacon bit rises (not on MLME_STA_CONNECTING). */
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_rx_tsf_filter(0);
		host_hal_rcr_chk_set_initial_rcr(RCR_CBSSID_DATA);
		rtw_hal_rcr_set_chk_bssid(a, MLME_STA_CONNECTED);
		expect_u32("tsf_restore_connected", host_hal_rcr_trace.tsf_restore_count,
			   1u);
	}

	/* MULTI_AP sets RCR_AAP when AP is up and unassoc NMY UC is enabled. */
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_mi_no_self(&ap_self);
		host_hal_rcr_chk_set_mi_no_others(&zero);
		host_hal_rcr_chk_set_unassoc_nmy_uc(1);
		host_hal_rcr_chk_set_initial_rcr(0);
		rtw_hal_rcr_set_chk_bssid(a, MLME_ACTION_NONE);
		expect_rcr_aap("multi_ap_on", host_hal_rcr_chk_last_set_rcr(), 1);
	}

	/* STA connecting: BCN on (lg_sta), DATA on. */
	run_case("sta_connecting", RCR_CBSSID_DATA, &zero, &zero,
		 MLME_STA_CONNECTING, 1, 1, 1);

	/* STA connected: ld_sta, BCN+DATA on. */
	run_case("sta_connected", 0, &zero, &zero, MLME_STA_CONNECTED, 1, 1, 1);

	/* STA disconnected: both on from cleared STA tallies. */
	run_case("sta_disconnected", 0, &zero, &zero, MLME_STA_DISCONNECTED, 1, 1,
		 1);

	/* act_non wrapper matches MLME_ACTION_NONE. */
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_initial_rcr(0);
		rtw_hal_rcr_set_chk_bssid_act_non(a);
		expect_rcr_bssid_bits("act_non", host_hal_rcr_chk_last_set_rcr(), 1,
				      1);
	}

#ifdef RUST_HAL_COM_RCR_CHK_BSSID_ORACLE
	{
		struct _adapter *a = host_hal_rcr_chk_adapter();
		u32 got;

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_initial_rcr(RCR_CBSSID_BCN | RCR_CBSSID_DATA);
		rtw_hal_rcr_set_chk_bssid(a, MLME_SCAN_ENTER);
		got = host_hal_rcr_chk_last_set_rcr();

		host_hal_rcr_chk_reset();
		host_hal_rcr_chk_set_initial_rcr(RCR_CBSSID_BCN | RCR_CBSSID_DATA);
		rtw_hal_rcr_set_chk_bssid_c_oracle(a, MLME_SCAN_ENTER);
		expect_u32("rust_scan_enter", got, host_hal_rcr_chk_last_set_rcr());
	}
#endif

	if (failures)
		return 1;
	puts("OK hal_com_rcr chk_bssid L2");
	return 0;
}