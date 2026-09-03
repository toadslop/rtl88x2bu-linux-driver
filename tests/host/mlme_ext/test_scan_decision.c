// SPDX-License-Identifier: GPL-2.0
/* W3-70 PR3: L2 oracle for scan sparse/backop/timeout helpers. */
#include <stdio.h>
#include <string.h>
#include "host_mlme_ext_scan_types.h"

static _adapter ad;
static struct rtw_ieee80211_channel ch[16];

static void reset(void)
{
	struct mi_state z = {0};
	memset(&ad, 0, sizeof(ad));
	host_scan_set_current_time(1000);
	host_scan_set_passing_time_ms(0);
	host_scan_set_busy_traffic(0);
	host_scan_set_miracast(0);
	host_scan_set_mi_state(&z);
}

static int test_sparse(const char *name, u32 pass_ms, u8 busy, u8 mirc,
		       u8 n, u8 expect)
{
	u8 i, ret;
	reset();
	host_scan_set_passing_time_ms(pass_ms);
	host_scan_set_busy_traffic(busy);
	host_scan_set_miracast(mirc);
	memset(ch, 0, sizeof(ch));
	for (i = 0; i < n; i++)
		ch[i].hw_value = i + 1;
	ret = rtw_scan_sparse(&ad, ch, n);
	if (ret != expect) {
		fprintf(stderr, "%s: %u != %u\n", name, ret, expect);
		return -1;
	}
	printf("PASS: %s\n", name);
	return 0;
}

static int test_backop(const char *name, u8 ld, u8 num, u8 flags, u8 expect)
{
	struct mi_state m = {0};
	reset();
	m.ld_sta_num = ld;
	m.sta_num = num;
	host_scan_set_mi_state(&m);
	ad.mlmeextpriv.sitesurvey_res.backop_flags_sta = flags;
	if (rtw_scan_backop_decision(&ad) != expect) {
		fprintf(stderr, "%s: backop mismatch\n", name);
		return -1;
	}
	printf("PASS: %s\n", name);
	return 0;
}

static int test_timeout(const char *name, u32 mode, u16 ms, u8 cnt_max,
			u16 bop_ms, u8 ld, u32 expect)
{
	struct mi_state m = {0};
	u32 t;
	reset();
	ad.registrypriv.wireless_mode = mode;
	ad.mlmeextpriv.sitesurvey_res.scan_ch_ms = ms;
	ad.mlmeextpriv.sitesurvey_res.scan_cnt_max = cnt_max;
	ad.mlmeextpriv.sitesurvey_res.backop_ms = bop_ms;
	if (ld) {
		m.ld_sta_num = 1;
		m.sta_num = 1;
		host_scan_set_mi_state(&m);
		ad.mlmeextpriv.sitesurvey_res.backop_flags_sta = SS_BACKOP_EN;
	}
	t = rtw_scan_timeout_decision(&ad);
	if (t != expect) {
		fprintf(stderr, "%s: %u != %u\n", name, t, expect);
		return -1;
	}
	printf("PASS: %s\n", name);
	return 0;
}

int main(void)
{
	if (test_sparse("sparse_passthrough", 1000, 0, 0, 6, 6) ||
	    test_sparse("sparse_bg", 15000, 0, 0, 12, 4) ||
	    test_sparse("sparse_miracast", 1000, 1, 1, 12, 1) ||
	    test_backop("backop_linked", 1, 1, 3, 3) ||
	    test_backop("backop_nl", 0, 1, 2, 2) ||
	    test_backop("backop_none", 0, 0, 3, 0) ||
	    test_timeout("timeout_2g", 3, 100, 2, 50, 0, 3400) ||
	    test_timeout("timeout_dual", 107, 100, 2, 50, 0, 7900) ||
	    test_timeout("timeout_backop", 3, 100, 2, 50, 1, 3750))
		return 1;
	printf("All scan decision vectors passed.\n");
	return 0;
}
