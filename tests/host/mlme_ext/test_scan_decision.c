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

static int check_sparse_hw(const char *name, u8 n, const u16 *expect)
{
	u8 i;

	for (i = 0; i < n; i++) {
		if (ch[i].hw_value != expect[i]) {
			fprintf(stderr, "%s: ch[%u] hw_value %u != %u\n", name, i,
				ch[i].hw_value, expect[i]);
			return -1;
		}
	}
	if (ch[n].hw_value != 0) {
		fprintf(stderr, "%s: ch[%u] not zero-terminated\n", name, n);
		return -1;
	}
	return 0;
}

static int test_sparse(const char *name, u32 pass_ms, u8 busy, u8 mirc,
		       u8 n, u8 expect_cnt, const u16 *expect_hw)
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
	if (ret != expect_cnt) {
		fprintf(stderr, "%s: count %u != %u\n", name, ret, expect_cnt);
		return -1;
	}
	if (check_sparse_hw(name, expect_cnt, expect_hw))
		return -1;
	printf("PASS: %s\n", name);
	return 0;
}

static int test_backop_sta(const char *name, u8 ld, u8 num, u8 flags, u8 expect)
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

static int test_backop_ap(const char *name, u8 ld, u8 num, u8 flags, u8 expect)
{
	struct mi_state m = {0};
	reset();
	m.ld_ap_num = ld;
	m.ap_num = num;
	host_scan_set_mi_state(&m);
	ad.mlmeextpriv.sitesurvey_res.backop_flags_ap = flags;
	if (rtw_scan_backop_decision(&ad) != expect) {
		fprintf(stderr, "%s: backop mismatch\n", name);
		return -1;
	}
	printf("PASS: %s\n", name);
	return 0;
}

static int test_timeout(const char *name, u32 mode, u16 ms, u16 duration,
			u8 cnt_max, u16 bop_ms, u8 ld, u32 expect)
{
	struct mi_state m = {0};
	u32 t;
	reset();
	ad.registrypriv.wireless_mode = mode;
	ad.mlmeextpriv.sitesurvey_res.scan_ch_ms = ms;
	ad.mlmeextpriv.sitesurvey_res.duration = duration;
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
	static const u16 passthrough[] = {1, 2, 3, 4, 5, 6};
	static const u16 bg[] = {2, 5, 8, 11};
	static const u16 miracast[] = {3};

	if (test_sparse("sparse_passthrough", 1000, 0, 0, 6, 6, passthrough) ||
	    test_sparse("sparse_bg", 15000, 0, 0, 12, 4, bg) ||
	    test_sparse("sparse_miracast", 1000, 1, 1, 12, 1, miracast) ||
	    test_backop_sta("backop_sta_linked", 1, 1, 3, 3) ||
	    test_backop_sta("backop_sta_nl", 0, 1, 2, 2) ||
	    test_backop_sta("backop_sta_none", 0, 0, 3, 0) ||
	    test_backop_ap("backop_ap_linked", 1, 1, 3, 3) ||
	    test_backop_ap("backop_ap_nl", 0, 1, 2, 2) ||
	    test_backop_ap("backop_ap_none", 0, 0, 3, 0) ||
	    test_timeout("timeout_2g", 3, 100, 0, 2, 50, 0, 3400) ||
	    test_timeout("timeout_dual", 107, 100, 0, 2, 50, 0, 7900) ||
	    test_timeout("timeout_backop", 3, 100, 0, 2, 50, 1, 3750) ||
	    test_timeout("timeout_duration", 3, 100, 200, 2, 50, 0, 4800))
		return 1;
	printf("All scan decision vectors passed.\n");
	return 0;
}
