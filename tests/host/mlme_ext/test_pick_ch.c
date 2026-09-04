// SPDX-License-Identifier: GPL-2.0
/* W3-71 PR1: L2 oracle for sitesurvey_pick_ch_behavior. */
#include <stdio.h>
#include <string.h>
#include "host_mlme_ext_scan_types.h"

static _adapter ad;

static void reset(void)
{
	struct mi_state z = {0};

	memset(&ad, 0, sizeof(ad));
	host_scan_set_current_time(1000);
	host_scan_set_p2p_social(0);
	host_scan_set_p2p_needed(0);
	host_scan_set_dfs_domain_unknown(1);
	host_scan_set_mi_state(&z);
}

static int run_case(const char *name, u8 expect_state, u8 expect_ch,
		    RT_SCAN_TYPE expect_type, u8 expect_force)
{
	u8 ch = 0;
	RT_SCAN_TYPE type = SCAN_PASSIVE;
	u8 state;

	state = sitesurvey_pick_ch_behavior(&ad, &ch, &type);
	if (state != expect_state || ch != expect_ch || type != expect_type ||
	    ad.mlmeextpriv.sitesurvey_res.force_ssid_scan != expect_force) {
		fprintf(stderr,
			"%s: state=%u ch=%u type=%d force=%u (expect state=%u ch=%u type=%d force=%u)\n",
			name, state, ch, type,
			ad.mlmeextpriv.sitesurvey_res.force_ssid_scan,
			expect_state, expect_ch, expect_type, expect_force);
		return -1;
	}
	printf("PASS: %s\n", name);
	return 0;
}

static void setup_channels(u8 n, u8 ch1, u32 flags1, u8 ch2, u32 flags2)
{
	struct ss_res *ss = &ad.mlmeextpriv.sitesurvey_res;

	memset(ss->ch, 0, sizeof(ss->ch));
	ss->ch_num = n;
	if (n >= 1) {
		ss->ch[0].hw_value = ch1;
		ss->ch[0].flags = flags1;
	}
	if (n >= 2) {
		ss->ch[1].hw_value = ch2;
		ss->ch[1].flags = flags2;
	}
}

int main(void)
{
	struct ss_res *ss = &ad.mlmeextpriv.sitesurvey_res;
	struct rf_ctl_t *rf = &ad.rfctl;

	reset();
	ss->channel_idx = 0;
	setup_channels(3, 1, 0, 6, 0);
	if (run_case("pick_first_active", SCAN_PROCESS, 1, SCAN_ACTIVE, 0))
		return 1;

	reset();
	ss->channel_idx = 0;
	setup_channels(2, 11, RTW_IEEE80211_CHAN_PASSIVE_SCAN, 0, 0);
	if (run_case("pick_passive", SCAN_PROCESS, 11, SCAN_PASSIVE, 0))
		return 1;

	reset();
	ss->channel_idx = 2;
	setup_channels(2, 1, 0, 6, 0);
	if (run_case("pick_complete", SCAN_COMPLETE, 0, SCAN_PASSIVE, 0))
		return 1;

	reset();
	ss->channel_idx = 0;
	ss->scan_cnt = 1;
	ss->scan_cnt_max = 2;
	ss->backop_flags_sta = SS_BACKOP_EN;
	setup_channels(2, 1, 0, 6, 0);
	{
		struct mi_state m = { .ld_sta_num = 1, .sta_num = 1 };

		host_scan_set_mi_state(&m);
	}
	if (run_case("backop_increment", SCAN_PROCESS, 1, SCAN_ACTIVE, 0) ||
	    ss->scan_cnt != 2)
		return 1;

	reset();
	ss->channel_idx = 0;
	ss->scan_cnt = 2;
	ss->scan_cnt_max = 2;
	ss->backop_flags_sta = SS_BACKOP_EN;
	setup_channels(2, 1, 0, 6, 0);
	{
		struct mi_state m = { .ld_sta_num = 1, .sta_num = 1 };

		host_scan_set_mi_state(&m);
	}
	if (run_case("backop_trigger", SCAN_BACKING_OP, 1, SCAN_ACTIVE, 0) ||
	    ss->backop_flags != SS_BACKOP_EN)
		return 1;

	reset();
	ss->channel_idx = 1;
	ss->ssid_num = 1;
	ss->force_ssid_scan = 0;
	setup_channels(2, 11, RTW_IEEE80211_CHAN_PASSIVE_SCAN, 6, 0);
	rf->channel_set[0].ChannelNum = 11;
	rf->channel_set[0].hidden_bss_cnt = 1;
	rf->channel_set[0].non_ocp_end_time = 0;
	if (run_case("hidden_bss_retry", SCAN_PROCESS, 11, SCAN_PASSIVE, 1) ||
	    ss->channel_idx != 0)
		return 1;

#ifdef CONFIG_P2P
	reset();
	ss->channel_idx = 0;
	ad.wdinfo.rx_invitereq_info.scan_op_ch_only = 1;
	ad.wdinfo.rx_invitereq_info.operation_ch[0] = 36;
	setup_channels(3, 1, 0, 6, 0);
	if (run_case("p2p_op_ch", SCAN_PROCESS, 36, SCAN_ACTIVE, 0))
		return 1;

	reset();
	host_scan_set_p2p_needed(1);
	ss->channel_idx = 2;
	setup_channels(2, 1, 0, 6, 0);
	if (run_case("p2p_listen", SCAN_TO_P2P_LISTEN, 0, SCAN_PASSIVE, 0))
		return 1;
#endif

	printf("All pick_ch vectors passed.\n");
	return 0;
}
