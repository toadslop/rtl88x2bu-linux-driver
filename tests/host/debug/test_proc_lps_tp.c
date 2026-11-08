// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-225 LPS chk-by-TP proc handlers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_lps_tp_types.h"
#include "host_vector_json.h"

#define MAX_V 12

struct vector {
	char name[48];
	u8 lps_chk_by_tp;
	u16 lps_tx_tp_th, lps_rx_tp_th, lps_bi_tp_th;
	int lps_chk_cnt_th;
	u32 lps_tx_pkts, lps_rx_pkts;
	char get[24], set[24], set_input[64], expect[384];
	int exp_en, has_exp_en;
	int exp_tx, has_exp_tx, exp_rx, has_exp_rx, exp_bi, has_exp_bi;
	int exp_cnt, has_exp_cnt, exp_tx_pkts, has_exp_tx_pkts;
	int exp_rx_pkts, has_exp_rx_pkts, exp_ret, has_exp_ret;
};

int proc_get_lps_chk_tp(struct seq_file *m, void *v);
ssize_t proc_set_lps_chk_tp(void *f, const void *b, size_t c, long *p, void *d);

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "lps_chk_by_tp", &t) == 0)
		v->lps_chk_by_tp = (u8)t;
	if (host_json_parse_int_in(obj, len, "lps_tx_tp_th", &t) == 0)
		v->lps_tx_tp_th = (u16)t;
	if (host_json_parse_int_in(obj, len, "lps_rx_tp_th", &t) == 0)
		v->lps_rx_tp_th = (u16)t;
	if (host_json_parse_int_in(obj, len, "lps_bi_tp_th", &t) == 0)
		v->lps_bi_tp_th = (u16)t;
	if (host_json_parse_int_in(obj, len, "lps_chk_cnt_th", &t) == 0)
		v->lps_chk_cnt_th = t;
	if (host_json_parse_int_in(obj, len, "lps_tx_pkts", &t) == 0)
		v->lps_tx_pkts = (u32)t;
	if (host_json_parse_int_in(obj, len, "lps_rx_pkts", &t) == 0)
		v->lps_rx_pkts = (u32)t;
	host_json_parse_string_in(obj, len, "get", v->get, sizeof(v->get));
	if (host_json_find_key_in(obj, len, "set")) {
		host_json_parse_string_in(obj, len, "set", v->set, sizeof(v->set));
		host_json_parse_string_in(obj, len, "set_input", v->set_input,
					  sizeof(v->set_input));
	}
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)) == 0) {
		char *r = v->expect, *w = v->expect;

		for (; *r; r++, w++) {
			if (r[0] == '\\' && r[1] == 'n') {
				*w = '\n';
				r++;
			} else {
				*w = *r;
			}
		}
		*w = '\0';
	}
	if (host_json_parse_int_in(obj, len, "expect_lps_chk_by_tp", &t) == 0) {
		v->exp_en = t;
		v->has_exp_en = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_lps_tx_tp_th", &t) == 0) {
		v->exp_tx = t;
		v->has_exp_tx = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_lps_rx_tp_th", &t) == 0) {
		v->exp_rx = t;
		v->has_exp_rx = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_lps_bi_tp_th", &t) == 0) {
		v->exp_bi = t;
		v->has_exp_bi = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_lps_chk_cnt_th", &t) == 0) {
		v->exp_cnt = t;
		v->has_exp_cnt = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_lps_tx_pkts", &t) == 0) {
		v->exp_tx_pkts = t;
		v->has_exp_tx_pkts = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_lps_rx_pkts", &t) == 0) {
		v->exp_rx_pkts = t;
		v->has_exp_rx_pkts = 1;
	}
	if (host_json_parse_int_in(obj, len, "set_expect_ret", &t) == 0) {
		v->exp_ret = t;
		v->has_exp_ret = 1;
	}
	return 0;
}

static int run_one(const struct vector *v)
{
	struct _adapter a;
	struct net_device d;
	struct seq_file s;
	struct pwrctrl_priv *p;
	char got[384];
	ssize_t ret = 0;
	FILE *cap;

	memset(&a, 0, sizeof(a));
	p = &a.pwrctrlpriv;
	p->lps_chk_by_tp = v->lps_chk_by_tp;
	p->lps_tx_tp_th = v->lps_tx_tp_th;
	p->lps_rx_tp_th = v->lps_rx_tp_th;
	p->lps_bi_tp_th = v->lps_bi_tp_th;
	p->lps_chk_cnt_th = v->lps_chk_cnt_th;
	p->lps_tx_pkts = v->lps_tx_pkts;
	p->lps_rx_pkts = v->lps_rx_pkts;
	d.priv = &a;
	s.private = &d;
	if (v->set[0]) {
		if (!strcmp(v->set, "lps_chk_tp"))
			ret = proc_set_lps_chk_tp(NULL, v->set_input,
						strlen(v->set_input), NULL, &d);
		else
			return -1;
		if (v->has_exp_ret && (int)ret != v->exp_ret)
			return -1;
		if (v->has_exp_en && (int)p->lps_chk_by_tp != v->exp_en)
			return -1;
		if (v->has_exp_tx && (int)p->lps_tx_tp_th != v->exp_tx)
			return -1;
		if (v->has_exp_rx && (int)p->lps_rx_tp_th != v->exp_rx)
			return -1;
		if (v->has_exp_bi && (int)p->lps_bi_tp_th != v->exp_bi)
			return -1;
		if (v->has_exp_cnt && p->lps_chk_cnt_th != v->exp_cnt)
			return -1;
		if (v->has_exp_tx_pkts && (int)p->lps_tx_pkts != v->exp_tx_pkts)
			return -1;
		if (v->has_exp_rx_pkts && (int)p->lps_rx_pkts != v->exp_rx_pkts)
			return -1;
	}
	if (!v->get[0])
		return 0;
	cap = tmpfile();
	if (!cap)
		return -1;
	s.out = cap;
	if (!strcmp(v->get, "lps_chk_tp"))
		proc_get_lps_chk_tp(&s, NULL);
	else
		return -1;

	size_t nread;

	rewind(cap);
	nread = fread(got, 1, sizeof(got) - 1, cap);
	got[nread] = '\0';
	fclose(cap);
	return strcmp(got, v->expect) ? -1 : 0;
}

int main(int argc, char **argv)
{
	struct vector v[MAX_V];
	size_t n = 0;

	if (host_load_vectors(argc > 1 ? argv[1] : "debug_proc_lps_tp_vectors.json",
			      v, sizeof(v[0]), MAX_V, parse_vector, &n))
		return 1;
	for (size_t i = 0; i < n; i++) {
		if (run_one(&v[i])) {
			fprintf(stderr, "fail: %s\n", v[i].name);
			return 1;
		}
	}
	printf("ok: %zu proc lps_chk_tp vectors\n", n);
	return 0;
}
