// SPDX-License-Identifier: GPL-2.0
/* W3-125 follow-up (#982) PR15: L2 C oracle for ACS sitesurvey parm builder. */
#include <stdio.h>
#include <string.h>

#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define SCAN_PASSIVE 0
#define CHANNEL_WIDTH_20 0
#define RTW_IEEE80211_CHAN_PASSIVE_SCAN (1 << 1)

#define HOST_ACS_MAX_CH 8

typedef unsigned char u8;

struct host_ieee80211_channel {
	u8 hw_value;
	unsigned int flags;
};

struct host_sitesurvey_parm {
	int scan_mode;
	u8 ch_num;
	u8 bw;
	int acs;
	struct host_ieee80211_channel ch[HOST_ACS_MAX_CH];
};

struct host_acs_adapter {
	u8 union_ok;
	u8 uch;
	u8 ch_sel_within_same_band;
	u8 scan_ret;
	int scan_calls;
	struct host_sitesurvey_parm last_parm;
};

#ifndef HOST_IOCTL_ACS_RUST
static const u8 host_acs_2g_chs[] = { 1, 6, 11 };
static const u8 host_acs_5g_chs[] = { 36, 40, 44 };

static u8 host_is_2g_ch(u8 ch)
{
	return ch >= 1 && ch <= 14;
}

#ifdef CONFIG_IEEE80211_BAND_5GHZ
static u8 host_is_5g_ch(u8 ch)
{
	return ch >= 36 && ch <= 177;
}
#endif

static u8 host_center_2g_num(u8 bw)
{
	(void)bw;
	return (u8)(sizeof(host_acs_2g_chs) / sizeof(host_acs_2g_chs[0]));
}

static u8 host_center_2g(u8 bw, u8 id)
{
	(void)bw;
	if (id >= host_center_2g_num(CHANNEL_WIDTH_20))
		return 0;
	return host_acs_2g_chs[id];
}

#ifdef CONFIG_IEEE80211_BAND_5GHZ
static u8 host_center_5g_num(u8 bw)
{
	(void)bw;
	return (u8)(sizeof(host_acs_5g_chs) / sizeof(host_acs_5g_chs[0]));
}

static u8 host_center_5g(u8 bw, u8 id)
{
	(void)bw;
	if (id >= host_center_5g_num(CHANNEL_WIDTH_20))
		return 0;
	return host_acs_5g_chs[id];
}
#endif

static void host_acs_add_band(struct host_acs_adapter *a, struct host_sitesurvey_parm *parm,
			      u8 band_is_2g, u8 (*center_chs_num)(u8), u8 (*center_chs)(u8, u8))
{
	int i;
	u8 ch_num;

	if (!center_chs_num || !center_chs)
		return;

	if (a->ch_sel_within_same_band) {
		if (host_is_2g_ch(a->uch) && !band_is_2g)
			return;
#ifdef CONFIG_IEEE80211_BAND_5GHZ
		if (host_is_5g_ch(a->uch) && band_is_2g)
			return;
#endif
	}

	ch_num = center_chs_num(CHANNEL_WIDTH_20);
	for (i = 0; i < ch_num && parm->ch_num < HOST_ACS_MAX_CH; i++) {
		parm->ch[parm->ch_num].hw_value = center_chs(CHANNEL_WIDTH_20, (u8)i);
		parm->ch[parm->ch_num].flags = RTW_IEEE80211_CHAN_PASSIVE_SCAN;
		parm->ch_num++;
	}
}

static void host_acs_fill_parm(struct host_acs_adapter *a, struct host_sitesurvey_parm *parm)
{
	memset(parm, 0, sizeof(*parm));
	parm->scan_mode = SCAN_PASSIVE;
	parm->bw = CHANNEL_WIDTH_20;
	parm->acs = 1;

	host_acs_add_band(a, parm, 1, host_center_2g_num, host_center_2g);
#ifdef CONFIG_IEEE80211_BAND_5GHZ
	host_acs_add_band(a, parm, 0, host_center_5g_num, host_center_5g);
#endif
}

static u8 host_bssid_list_scan(struct host_acs_adapter *a, struct host_sitesurvey_parm *parm)
{
	a->scan_calls++;
	a->last_parm = *parm;
	return a->scan_ret;
}

static u8 host_acs_sitesurvey_c(struct host_acs_adapter *a)
{
	struct host_sitesurvey_parm parm;
	u8 ret = _FAIL;

	if (!a->union_ok)
		goto exit;

	host_acs_fill_parm(a, &parm);
	ret = host_bssid_list_scan(a, &parm);

exit:
	return ret;
}

#define ACS_FN host_acs_sitesurvey_c
#else
extern u8 rtw_set_acs_sitesurvey_rust(struct host_acs_adapter *a);
#define ACS_FN rtw_set_acs_sitesurvey_rust
#endif

struct vector {
	char name[64];
	int union_ok;
	int uch;
	int ch_sel_same_band;
	int scan_ret;
	int expect_ret;
	int expect_scan_calls;
	int expect_ch_num;
	int expect_scan_mode;
	int expect_acs;
	int expect_first_ch;
	int expect_last_ch;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "union_ok", &v->union_ok);
	host_json_parse_int_in(obj, len, "uch", &v->uch);
	host_json_parse_int_in(obj, len, "ch_sel_same_band", &v->ch_sel_same_band);
	host_json_parse_int_in(obj, len, "scan_ret", &v->scan_ret);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_scan_calls", &v->expect_scan_calls);
	host_json_parse_int_in(obj, len, "expect_ch_num", &v->expect_ch_num);
	host_json_parse_int_in(obj, len, "expect_scan_mode", &v->expect_scan_mode);
	host_json_parse_int_in(obj, len, "expect_acs", &v->expect_acs);
	host_json_parse_int_in(obj, len, "expect_first_ch", &v->expect_first_ch);
	host_json_parse_int_in(obj, len, "expect_last_ch", &v->expect_last_ch);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct host_acs_adapter a;
	u8 ret;

	memset(&a, 0, sizeof(a));
	a.union_ok = (u8)v->union_ok;
	a.uch = (u8)v->uch;
	a.ch_sel_within_same_band = (u8)v->ch_sel_same_band;
	a.scan_ret = (u8)v->scan_ret;

	ret = ACS_FN(&a);
	if (ret != (u8)v->expect_ret) {
		fprintf(stderr, "FAIL %s: ret got %u expect %d\n", v->name, ret, v->expect_ret);
		return 1;
	}
	if (a.scan_calls != v->expect_scan_calls) {
		fprintf(stderr, "FAIL %s: scan_calls got %d expect %d\n", v->name, a.scan_calls,
			v->expect_scan_calls);
		return 1;
	}
	if (v->expect_scan_calls > 0) {
		if (a.last_parm.ch_num != (u8)v->expect_ch_num) {
			fprintf(stderr, "FAIL %s: ch_num got %u expect %d\n", v->name,
				a.last_parm.ch_num, v->expect_ch_num);
			return 1;
		}
		if (v->expect_acs > 0 && a.last_parm.scan_mode != SCAN_PASSIVE) {
			fprintf(stderr, "FAIL %s: scan_mode got %d expect passive\n", v->name,
				a.last_parm.scan_mode);
			return 1;
		}
		if (v->expect_acs > 0 && a.last_parm.acs != v->expect_acs) {
			fprintf(stderr, "FAIL %s: acs got %d expect %d\n", v->name, a.last_parm.acs,
				v->expect_acs);
			return 1;
		}
		if (v->expect_first_ch > 0 &&
		    a.last_parm.ch[0].hw_value != (u8)v->expect_first_ch) {
			fprintf(stderr, "FAIL %s: first ch got %u expect %d\n", v->name,
				a.last_parm.ch[0].hw_value, v->expect_first_ch);
			return 1;
		}
		if (v->expect_last_ch > 0 && v->expect_ch_num > 0 &&
		    a.last_parm.ch[v->expect_ch_num - 1].hw_value != (u8)v->expect_last_ch) {
			fprintf(stderr, "FAIL %s: last ch got %u expect %d\n", v->name,
				a.last_parm.ch[v->expect_ch_num - 1].hw_value, v->expect_last_ch);
			return 1;
		}
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_acs_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
