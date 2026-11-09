// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-227 ACK timeout proc handlers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_ack_types.h"
#include "host_vector_json.h"

#define MAX_V 12

struct vector {
	char name[48];
	u8 reg_ackto, reg_cck;
	char get[24], set[24], set_input[64], expect[256];
	int exp_w_ackto, has_exp_w_ackto, exp_w_cck, has_exp_w_cck;
	int exp_ret, has_exp_ret, cfg_8821c, has_cfg_8821c, non_8821c_only;
};

int proc_get_ack_timeout(struct seq_file *m, void *v);
ssize_t proc_set_ack_timeout(void *f, const void *b, size_t c, long *p, void *d);

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "reg_ackto", &t) == 0)
		v->reg_ackto = (u8)t;
	if (host_json_parse_int_in(obj, len, "reg_cck", &t) == 0)
		v->reg_cck = (u8)t;
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
	if (host_json_parse_int_in(obj, len, "expect_write_ackto", &t) == 0) {
		v->exp_w_ackto = t;
		v->has_exp_w_ackto = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_write_cck", &t) == 0) {
		v->exp_w_cck = t;
		v->has_exp_w_cck = 1;
	}
	if (host_json_parse_int_in(obj, len, "set_expect_ret", &t) == 0) {
		v->exp_ret = t;
		v->has_exp_ret = 1;
	}
	if (host_json_parse_int_in(obj, len, "cfg_8821c", &t) == 0) {
		v->cfg_8821c = t;
		v->has_cfg_8821c = 1;
	}
	if (host_json_parse_int_in(obj, len, "non_8821c_only", &t) == 0)
		v->non_8821c_only = t;
	return 0;
}

static int skip_vector(const struct vector *v)
{
	if (v->has_cfg_8821c && v->cfg_8821c) {
#ifndef CONFIG_RTL8821C
		return 1;
#else
		return 0;
#endif
	}
#ifdef CONFIG_RTL8821C
	if (v->non_8821c_only)
		return 1;
#endif
	return 0;
}

static int run_one(const struct vector *v)
{
	struct _adapter a;
	struct net_device d;
	struct seq_file s;
	char got[256];
	ssize_t ret = 0;
	FILE *cap;
	const struct host_ack_reg_log *log;

	if (skip_vector(v))
		return 0;

	memset(&a, 0, sizeof(a));
	host_debug_proc_ack_reset_log();
	host_debug_proc_ack_seed_regs(v->reg_ackto, v->reg_cck);
	d.priv = &a;
	s.private = &d;

	if (v->set[0]) {
		if (!strcmp(v->set, "ack_timeout"))
			ret = proc_set_ack_timeout(NULL, v->set_input,
						   strlen(v->set_input), NULL,
						   &d);
		else
			return -1;
		if (v->has_exp_ret && (int)ret != v->exp_ret)
			return -1;
		log = host_debug_proc_ack_get_log();
		if (v->has_exp_w_ackto &&
		    (int)log->last_write_ackto != v->exp_w_ackto)
			return -1;
		if (v->has_exp_w_cck && (int)log->last_write_cck != v->exp_w_cck)
			return -1;
	}
	if (!v->get[0])
		return 0;
	cap = tmpfile();
	if (!cap)
		return -1;
	s.out = cap;
	if (!strcmp(v->get, "ack_timeout"))
		proc_get_ack_timeout(&s, NULL);
	else {
		fclose(cap);
		return -1;
	}
	rewind(cap);
	if (fread(got, 1, sizeof(got) - 1, cap) == 0 && v->expect[0])
		got[0] = '\0';
	fclose(cap);
	got[sizeof(got) - 1] = '\0';
	return strcmp(got, v->expect) ? -1 : 0;
}

int main(int argc, char **argv)
{
	struct vector v[MAX_V];
	size_t n = 0, ran = 0;

	if (host_load_vectors(argc > 1 ? argv[1] : "debug_proc_ack_vectors.json",
			      v, sizeof(v[0]), MAX_V, parse_vector, &n))
		return 1;
	for (size_t i = 0; i < n; i++) {
		if (skip_vector(&v[i]))
			continue;
		ran++;
		if (run_one(&v[i])) {
			fprintf(stderr, "fail: %s\n", v[i].name);
			return 1;
		}
	}
	printf("ok: %zu proc ack_timeout vectors\n", ran);
	return 0;
}
