// SPDX-License-Identifier: GPL-2.0
/* W3-125 follow-up (#982) PR9: L2 C oracle for get_cur_max_rate legacy path. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define IEEE80211_BASIC_RATE_MASK 0x80
#define NumRates 13

typedef unsigned char u8;
typedef unsigned short u16;

#define WIFI_ASOC_STATE (1u << 0)
#define WIFI_ADHOC_MASTER_STATE (1u << 1)

struct vector {
	char name[64];
	unsigned fw_state;
	int has_sta;
	int sta_mode;
	int ap_rates[NumRates];
	int sta_rates[NumRates];
	int sta_rate_len;
	int expect_rate;
};

#ifdef HOST_IOCTL_MAX_RATE_RUST
extern u16 rtw_get_cur_max_rate_legacy_rust(unsigned fw_state, int has_sta, int sta_mode,
					  const u8 *ap_rates, const u8 *sta_rates,
					  int sta_rate_len);
#define MAX_RATE_FN rtw_get_cur_max_rate_legacy_rust
#else
static int chk_fw(unsigned fw_state, unsigned bit)
{
	return (fw_state & bit) != 0;
}

static u16 MAX_RATE_FN(unsigned fw_state, int has_sta, int sta_mode, const u8 *ap_rates,
		       const u8 *sta_rates, int sta_rate_len)
{
	int i = 0;
	int j;
	u16 rate = 0;
	u16 max_rate = 0;

	if (!chk_fw(fw_state, WIFI_ASOC_STATE) &&
	    !chk_fw(fw_state, WIFI_ADHOC_MASTER_STATE))
		return 0;

	if (!has_sta)
		return 0;

	while (ap_rates[i] != 0 && ap_rates[i] != 0xff) {
		rate = ap_rates[i] & 0x7f;
		if (sta_mode) {
			for (j = 0; j < sta_rate_len; j++) {
				if ((rate | IEEE80211_BASIC_RATE_MASK) ==
				    (sta_rates[j] | IEEE80211_BASIC_RATE_MASK)) {
					if (rate > max_rate)
						max_rate = rate;
					break;
				}
			}
		} else if (rate > max_rate) {
			max_rate = rate;
		}
		i++;
	}

	return (u16)(max_rate * 10 / 2);
}
#endif

static int parse_rate_csv(const char *obj, size_t len, const char *key, int *out, int max_n)
{
	char buf[128];
	const char *p;
	const char *end;
	int n = 0;

	for (int i = 0; i < max_n; i++)
		out[i] = 0;
	if (host_json_parse_string_in(obj, len, key, buf, sizeof(buf)))
		return 0;
	p = buf;
	end = buf + strlen(buf);
	while (p < end && n < max_n) {
		out[n++] = (int)strtol(p, (char **)&p, 10);
		while (p < end && (*p == ',' || *p == ' '))
			p++;
	}
	while (n < max_n)
		out[n++] = 255;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "fw_state", (int *)&v->fw_state);
	host_json_parse_int_in(obj, len, "has_sta", &v->has_sta);
	host_json_parse_int_in(obj, len, "sta_mode", &v->sta_mode);
	host_json_parse_int_in(obj, len, "sta_rate_len", &v->sta_rate_len);
	host_json_parse_int_in(obj, len, "expect_rate", &v->expect_rate);
	parse_rate_csv(obj, len, "ap_rates", v->ap_rates, NumRates);
	parse_rate_csv(obj, len, "sta_rates", v->sta_rates, NumRates);
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 ap[NumRates];
	u8 sta[NumRates];
	u16 got;
	int i;

	for (i = 0; i < NumRates; i++) {
		ap[i] = (u8)v->ap_rates[i];
		sta[i] = (u8)v->sta_rates[i];
	}

	got = MAX_RATE_FN(v->fw_state, v->has_sta, v->sta_mode, ap, sta, v->sta_rate_len);
	if (got != (u16)v->expect_rate) {
		fprintf(stderr, "FAIL %s: expected rate %d got %u\n", v->name, v->expect_rate, got);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_max_rate_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
