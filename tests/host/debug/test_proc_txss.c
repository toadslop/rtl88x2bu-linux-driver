// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-224 TXSS proc handlers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_txss_types.h"
#include "host_vector_json.h"

#define MAX_V 12

struct vector {
	char name[48];
	u8 txss_ctrl_en, txss_tp_chk_cnt, txss_1ss;
	u16 txss_tp_th;
	char get[24], set[24], set_input[48], expect[192];
	int exp_ctrl, has_exp_ctrl, exp_th, has_exp_th, exp_chk, has_exp_chk;
	int exp_dbg, has_exp_dbg, exp_1ss, has_exp_1ss, exp_ret, has_exp_ret;
};

int proc_get_txss_tp(struct seq_file *m, void *v);
ssize_t proc_set_txss_tp(void *f, const void *b, size_t c, long *p, void *d);
#ifdef DBG_CTRL_TXSS
int proc_get_txss_ctrl(struct seq_file *m, void *v);
ssize_t proc_set_txss_ctrl(void *f, const void *b, size_t c, long *p, void *d);
#endif

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "txss_ctrl_en", &t) == 0)
		v->txss_ctrl_en = (u8)t;
	if (host_json_parse_int_in(obj, len, "txss_tp_th", &t) == 0)
		v->txss_tp_th = (u16)t;
	if (host_json_parse_int_in(obj, len, "txss_tp_chk_cnt", &t) == 0)
		v->txss_tp_chk_cnt = (u8)t;
	if (host_json_parse_int_in(obj, len, "txss_1ss", &t) == 0)
		v->txss_1ss = (u8)t;
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
	if (host_json_parse_int_in(obj, len, "expect_txss_ctrl_en", &t) == 0) {
		v->exp_ctrl = t;
		v->has_exp_ctrl = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_txss_tp_th", &t) == 0) {
		v->exp_th = t;
		v->has_exp_th = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_txss_tp_chk_cnt", &t) == 0) {
		v->exp_chk = t;
		v->has_exp_chk = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_dbg_ctrl_calls", &t) == 0) {
		v->exp_dbg = t;
		v->has_exp_dbg = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_last_tx_1ss", &t) == 0) {
		v->exp_1ss = t;
		v->has_exp_1ss = 1;
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
	struct mlme_ext_priv *m;
	char got[192];
	ssize_t ret = 0;
	FILE *cap;

	memset(&a, 0, sizeof(a));
	host_debug_proc_txss_reset_log();
	m = &a.mlmeextpriv;
	m->txss_ctrl_en = v->txss_ctrl_en;
	m->txss_tp_th = v->txss_tp_th;
	m->txss_tp_chk_cnt = v->txss_tp_chk_cnt;
	m->txss_1ss = v->txss_1ss;
	d.priv = &a;
	s.private = &d;
	if (v->set[0]) {
		if (!strcmp(v->set, "txss_tp"))
			ret = proc_set_txss_tp(NULL, v->set_input, strlen(v->set_input),
					       NULL, &d);
#ifdef DBG_CTRL_TXSS
		else if (!strcmp(v->set, "txss_ctrl"))
			ret = proc_set_txss_ctrl(NULL, v->set_input,
						 strlen(v->set_input), NULL, &d);
#endif
		else
			return -1;
		if (v->has_exp_ret && (int)ret != v->exp_ret)
			return -1;
		if (v->has_exp_ctrl && (int)m->txss_ctrl_en != v->exp_ctrl)
			return -1;
		if (v->has_exp_th && (int)m->txss_tp_th != v->exp_th)
			return -1;
		if (v->has_exp_chk && (int)m->txss_tp_chk_cnt != v->exp_chk)
			return -1;
		if (v->has_exp_dbg &&
		    (int)host_debug_proc_txss_get_log()->dbg_ctrl_calls != v->exp_dbg)
			return -1;
		if (v->has_exp_1ss &&
		    (int)host_debug_proc_txss_get_log()->last_tx_1ss != v->exp_1ss)
			return -1;
	}
	if (!v->get[0])
		return 0;
	cap = tmpfile();
	if (!cap)
		return -1;
	s.out = cap;
	if (!strcmp(v->get, "txss_tp"))
		proc_get_txss_tp(&s, NULL);
#ifdef DBG_CTRL_TXSS
	else if (!strcmp(v->get, "txss_ctrl"))
		proc_get_txss_ctrl(&s, NULL);
#endif
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

	if (host_load_vectors(argc > 1 ? argv[1] : "debug_proc_txss_vectors.json",
			      v, sizeof(v[0]), MAX_V, parse_vector, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		if (run_one(&v[i]))
			return 1;
	printf("ok: %zu proc txss vectors\n", n);
	return 0;
}
