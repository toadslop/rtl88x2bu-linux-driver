// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-182 rtw_wow_lps_level_decide. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_wow_lps.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int lps_level;
	int lps_level_bk;
	int wowlan_lps_level;
	int lps_1t1r;
	int lps_1t1r_bk;
	int wowlan_lps_1t1r;
	int wow_en;
	int expect_lps_level;
	int expect_lps_level_bk;
	int expect_lps_1t1r;
	int expect_lps_1t1r_bk;
	int roundtrip_off;
	int expect_after_off_lps_level;
	int expect_after_off_lps_1t1r;
};

static struct dvobj_priv g_dvobj;
static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "lps_level", &v->lps_level);
	host_json_parse_int_in(obj, len, "lps_level_bk", &v->lps_level_bk);
	host_json_parse_int_in(obj, len, "wowlan_lps_level", &v->wowlan_lps_level);
	host_json_parse_int_in(obj, len, "lps_1t1r", &v->lps_1t1r);
	host_json_parse_int_in(obj, len, "lps_1t1r_bk", &v->lps_1t1r_bk);
	host_json_parse_int_in(obj, len, "wowlan_lps_1t1r", &v->wowlan_lps_1t1r);
	host_json_parse_int_in(obj, len, "wow_en", &v->wow_en);
	host_json_parse_int_in(obj, len, "expect_lps_level", &v->expect_lps_level);
	host_json_parse_int_in(obj, len, "expect_lps_level_bk", &v->expect_lps_level_bk);
	host_json_parse_int_in(obj, len, "expect_lps_1t1r", &v->expect_lps_1t1r);
	host_json_parse_int_in(obj, len, "expect_lps_1t1r_bk", &v->expect_lps_1t1r_bk);
	host_json_parse_int_in(obj, len, "roundtrip_off", &v->roundtrip_off);
	host_json_parse_int_in(obj, len, "expect_after_off_lps_level",
			       &v->expect_after_off_lps_level);
	host_json_parse_int_in(obj, len, "expect_after_off_lps_1t1r",
			       &v->expect_after_off_lps_1t1r);
	return 0;
}

static int check_state(struct vector *v, struct host_pwrctrl_wow_lps_state *st)
{
	if ((int)st->lps_level != v->expect_lps_level ||
	    (int)st->lps_level_bk != v->expect_lps_level_bk ||
	    (int)st->lps_1t1r != v->expect_lps_1t1r ||
	    (int)st->lps_1t1r_bk != v->expect_lps_1t1r_bk) {
		fprintf(stderr,
			"FAIL %s level=%u bk=%u 1t1r=%u 1t1r_bk=%u expect level=%d bk=%d 1t1r=%d 1t1r_bk=%d\n",
			v->name, st->lps_level, st->lps_level_bk, st->lps_1t1r,
			st->lps_1t1r_bk, v->expect_lps_level, v->expect_lps_level_bk,
			v->expect_lps_1t1r, v->expect_lps_1t1r_bk);
		return 1;
	}
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv *p;
	struct host_pwrctrl_wow_lps_state st;

	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.dvobj = &g_dvobj;

	p = dvobj_to_pwrctl(&g_dvobj);
	p->lps_level = (u8)v->lps_level;
	p->lps_level_bk = (u8)v->lps_level_bk;
	p->wowlan_lps_level = (u8)v->wowlan_lps_level;
	p->lps_1t1r = (u8)v->lps_1t1r;
	p->lps_1t1r_bk = (u8)v->lps_1t1r_bk;
	p->wowlan_lps_1t1r = (u8)v->wowlan_lps_1t1r;

	rtw_wow_lps_level_decide(&g_adapter, (u8)v->wow_en);
	host_pwrctrl_wow_lps_read_state(&g_adapter, &st);
	if (check_state(v, &st))
		return 1;

	if (v->roundtrip_off) {
		rtw_wow_lps_level_decide(&g_adapter, _FALSE);
		host_pwrctrl_wow_lps_read_state(&g_adapter, &st);
		if ((int)st.lps_level != v->expect_after_off_lps_level ||
		    (int)st.lps_1t1r != v->expect_after_off_lps_1t1r) {
			fprintf(stderr,
				"FAIL %s after wow_off level=%u 1t1r=%u expect level=%d 1t1r=%d\n",
				v->name, st.lps_level, st.lps_1t1r,
				v->expect_after_off_lps_level,
				v->expect_after_off_lps_1t1r);
			return 1;
		}
	}

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "wow_lps_level_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
