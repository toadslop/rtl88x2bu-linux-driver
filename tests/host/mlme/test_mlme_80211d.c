// SPDX-License-Identifier: GPL-2.0
/* W3-66 PR1: host L2 oracle for 802.11d guard + country IE parse. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define MAX_CHANNEL_NUM 59
#define _FIXED_IE_LENGTH_ 12
#define _COUNTRY_IE_ 7

typedef struct { u8 Channel[MAX_CHANNEL_NUM]; u8 Len; } RT_CHANNEL_PLAN;
typedef struct { u8 enable80211d; } registry_priv;
typedef struct { u8 update_channel_plan_by_ap_done; } mlme_ext_priv;
typedef struct { u32 IELength; u8 IEs[768]; } WLAN_BSSID_EX;
typedef struct { registry_priv registrypriv; mlme_ext_priv mlmeextpriv; } _adapter;

static u8 *rtw_get_ie(const u8 *pbuf, int index, int *len, int limit)
{
	int i = 0;
	const u8 *p = pbuf;

	*len = 0;
	while (i < limit) {
		int tmp;

		if (*p == (u8)index) {
			*len = *(p + 1);
			return (u8 *)p;
		}
		tmp = *(p + 1);
		p += tmp + 2;
		i += tmp + 2;
	}
	return NULL;
}

static int parse_country_ie(const u8 *p, const u8 *end, RT_CHANNEL_PLAN *out)
{
	u8 i = 0;

	while ((end - p) >= 3) {
		u8 fcn = *(p++);
		u8 noc = *(p++);

		p++;
		for (u8 j = 0; j < noc; j++) {
			u8 ch = (fcn <= 14) ? (u8)(fcn + j) : (u8)(fcn + j * 4);

			if (i >= MAX_CHANNEL_NUM)
				return -1;
			out->Channel[i++] = ch;
		}
	}
	out->Len = i;
	return 0;
}

static int process_80211d_probe(_adapter *a, WLAN_BSSID_EX *b, RT_CHANNEL_PLAN *out)
{
	int len;
	u8 *ie;

	if (!a->registrypriv.enable80211d || a->mlmeextpriv.update_channel_plan_by_ap_done)
		return 0;
	ie = rtw_get_ie(b->IEs + _FIXED_IE_LENGTH_, _COUNTRY_IE_, &len,
			(int)(b->IELength - _FIXED_IE_LENGTH_));
	if (!ie || len < 6)
		return 0;
	memset(out, 0, sizeof(*out));
	return parse_country_ie(ie + 5, ie + 2 + len, out) ? -1 : 1;
}

struct case_vec {
	const char *name;
	u8 enable, done;
	const u8 *ie;
	size_t ie_len;
	int expect;
	const u8 *expect_ch;
	int expect_n;
};

static void fill(WLAN_BSSID_EX *b, const u8 *ie, size_t n)
{
	memset(b, 0, sizeof(*b));
	b->IELength = _FIXED_IE_LENGTH_;
	if (ie && n) {
		memcpy(b->IEs + _FIXED_IE_LENGTH_, ie, n);
		b->IELength += (u32)n;
	}
}

static int ch_match(const RT_CHANNEL_PLAN *ap, const u8 *exp, int n)
{
	if ((int)ap->Len != n)
		return -1;
	for (int i = 0; i < n; i++)
		if (ap->Channel[i] != exp[i])
			return -1;
	return 0;
}

static int run_case(const struct case_vec *v)
{
	_adapter a;
	WLAN_BSSID_EX b;
	RT_CHANNEL_PLAN ap;
	int r;

	memset(&a, 0, sizeof(a));
	a.registrypriv.enable80211d = v->enable;
	a.mlmeextpriv.update_channel_plan_by_ap_done = v->done;
	fill(&b, v->ie, v->ie_len);
	r = process_80211d_probe(&a, &b, &ap);
	if (r != v->expect)
		return -1;
	if (r == 1 && ch_match(&ap, v->expect_ch, v->expect_n))
		return -1;
	return 0;
}

int main(void)
{
	static const u8 us2g[] = {0x07, 0x09, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e};
	static const u8 us24g[] = {0x07, 0x0c, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e, 0x24, 0x04, 0x1e};
	static const u8 ch11[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
	static const u8 ch11_5g[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 36, 40, 44, 48};
	static const u8 short_ie[] = {0x07, 0x03, 0x55, 0x53, 0x20};
	static const struct case_vec cases[] = {
		{"disabled", 0, 0, us2g, sizeof(us2g), 0, NULL, 0},
		{"already_done", 1, 1, us2g, sizeof(us2g), 0, NULL, 0},
		{"no_country_ie", 1, 0, NULL, 0, 0, NULL, 0},
		{"short_country_ie", 1, 0, short_ie, sizeof(short_ie), 0, NULL, 0},
		{"parse_us_2g", 1, 0, us2g, sizeof(us2g), 1, ch11, 11},
		{"parse_5g_triplet", 1, 0, us24g, sizeof(us24g), 1, ch11_5g, 15},
	};
	int bad = 0;

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
		bad += run_case(&cases[i]) ?
			(fprintf(stderr, "FAIL %s\n", cases[i].name), 1) :
			(printf("PASS %s\n", cases[i].name), 0);
	if (!bad)
		printf("PASS %zu vectors (W3-66 PR1 host oracle)\n", sizeof(cases) / sizeof(cases[0]));
	return bad ? 1 : 0;
}
