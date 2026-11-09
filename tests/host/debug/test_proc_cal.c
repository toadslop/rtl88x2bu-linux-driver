// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-230 proc IQK/LCK cal test handlers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_cal_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 12
#define MAX_NAME 48
#define MAX_FIELD 16
#define MAX_INPUT 64

struct vector {
	char name[MAX_NAME];
	char get[MAX_FIELD];
	char set[MAX_FIELD];
	char set_input[MAX_INPUT];
	size_t set_count;
	int has_set_count;
	int expect_ret;
	int has_expect_ret;
	int expect_iqk_calls;
	int has_expect_iqk_calls;
	int expect_iqk_recovery;
	int has_expect_iqk_recovery;
	int expect_iqk_clear;
	int has_expect_iqk_clear;
	int expect_iqk_segment;
	int has_expect_iqk_segment;
	int expect_lck_calls;
	int has_expect_lck_calls;
};

static int parse_int_opt(const char *obj, size_t len, const char *key, int *out,
			 int *has)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp) != 0)
		return 0;
	*out = tmp;
	*has = 1;
	return 0;
}

static int parse_vector(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "get", v->get, sizeof(v->get));
	if (host_json_find_key_in(obj, len, "set")) {
		if (host_json_parse_string_in(obj, len, "set", v->set,
					      sizeof(v->set)))
			return -1;
		if (host_json_parse_string_in(obj, len, "set_input",
					      v->set_input, sizeof(v->set_input)))
			return -1;
		v->set_count = strlen(v->set_input);
		if (host_json_parse_int_in(obj, len, "set_count", &tmp) == 0) {
			v->set_count = (size_t)tmp;
			v->has_set_count = 1;
		}
	}
	parse_int_opt(obj, len, "expect_ret", &v->expect_ret, &v->has_expect_ret);
	parse_int_opt(obj, len, "expect_iqk_calls", &v->expect_iqk_calls,
		      &v->has_expect_iqk_calls);
	parse_int_opt(obj, len, "expect_iqk_recovery", &v->expect_iqk_recovery,
		      &v->has_expect_iqk_recovery);
	parse_int_opt(obj, len, "expect_iqk_clear", &v->expect_iqk_clear,
		      &v->has_expect_iqk_clear);
	parse_int_opt(obj, len, "expect_iqk_segment", &v->expect_iqk_segment,
		      &v->has_expect_iqk_segment);
	parse_int_opt(obj, len, "expect_lck_calls", &v->expect_lck_calls,
		      &v->has_expect_lck_calls);
	return 0;
}

static int run_one(const struct vector *v)
{
	struct _adapter a;
	struct net_device d;
	struct seq_file s;
	ssize_t ret;
	const char *set_buf = v->set_input;

	memset(&a, 0, sizeof(a));
	host_debug_proc_cal_reset_log();
	d.priv = &a;
	s.private = &d;

	if (v->set[0]) {
		if (!strcmp(v->set, "iqk"))
			ret = proc_set_iqk(NULL, set_buf,
					   v->has_set_count ? v->set_count
							    : strlen(set_buf),
					   NULL, &d);
		else if (!strcmp(v->set, "lck"))
			ret = proc_set_lck(NULL, set_buf,
					   v->has_set_count ? v->set_count
							    : strlen(set_buf),
					   NULL, &d);
		else
			return -1;
		if (v->has_expect_ret && (int)ret != v->expect_ret)
			return -1;
		if (v->has_expect_iqk_calls &&
		    (int)host_debug_proc_cal_iqk_calls() != v->expect_iqk_calls)
			return -1;
		if (v->has_expect_iqk_recovery &&
		    (int)host_debug_proc_cal_iqk_last_recovery() !=
			    v->expect_iqk_recovery)
			return -1;
		if (v->has_expect_iqk_clear &&
		    (int)host_debug_proc_cal_iqk_last_clear() !=
			    v->expect_iqk_clear)
			return -1;
		if (v->has_expect_iqk_segment &&
		    (int)host_debug_proc_cal_iqk_last_segment() !=
			    v->expect_iqk_segment)
			return -1;
		if (v->has_expect_lck_calls &&
		    (int)host_debug_proc_cal_lck_calls() != v->expect_lck_calls)
			return -1;
	} else if (v->get[0]) {
		if (!strcmp(v->get, "iqk"))
			ret = proc_get_iqk_info(&s, NULL);
		else if (!strcmp(v->get, "lck"))
			ret = proc_get_lck_info(&s, NULL);
		else
			return -1;
		if (v->has_expect_ret && (int)ret != v->expect_ret)
			return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path =
		argc > 1 ? argv[1] : "debug_proc_cal_vectors.json";
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS,
			    parse_vector, &n)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}
	for (size_t i = 0; i < n; i++) {
		if (run_one(&vecs[i]) != 0) {
			fprintf(stderr, "FAIL %s\n", vecs[i].name);
			return 1;
		}
	}
	printf("ok: %zu proc cal test vectors\n", n);
	return 0;
}
