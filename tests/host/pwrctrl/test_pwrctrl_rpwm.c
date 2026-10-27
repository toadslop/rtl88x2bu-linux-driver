// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_vector_json.h"

#define _SUCCESS 1
#define _FAIL 0
#define MAX_VECTORS 8
#define MAX_NAME 64
typedef unsigned char u8;
void host_pwrctrl_rpwm_reset_hal(void);
void host_pwrctrl_rpwm_set_time(unsigned int t);
void host_pwrctrl_rpwm_set_cpwm_behavior(u8, unsigned int);
u8 host_pwrctrl_rpwm_get_cpwm(void *);
u8 host_pwrctrl_rpwm_get_cpwm_tog(void *);
u8 rtw_cpwm_polling(void *, u8, u8);

struct vector {
	char name[MAX_NAME];
	int cpwm_orig;
	int poll_reads_before_toggle;
	int rpwm_in;
	int cpwm_orig_in;
	int expect_success;
	int has_expect_success;
	int expect_cpwm;
	int has_expect_cpwm;
	int expect_cpwm_tog;
	int has_expect_cpwm_tog;
};

static char g_adapter[64];

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "cpwm_orig", &v->cpwm_orig);
	host_json_parse_int_in(obj, len, "poll_reads_before_toggle",
			       &v->poll_reads_before_toggle);
	host_json_parse_int_in(obj, len, "rpwm_in", &v->rpwm_in);
	host_json_parse_int_in(obj, len, "cpwm_orig_in", &v->cpwm_orig_in);
	if (host_json_parse_int_in(obj, len, "expect_success", &v->expect_success) == 0)
		v->has_expect_success = 1;
	if (host_json_parse_int_in(obj, len, "expect_cpwm", &v->expect_cpwm) == 0)
		v->has_expect_cpwm = 1;
	if (host_json_parse_int_in(obj, len, "expect_cpwm_tog", &v->expect_cpwm_tog) == 0)
		v->has_expect_cpwm_tog = 1;
	return 0;
}

static int run_vec(struct vector *v)
{
	u8 ret;

	host_pwrctrl_rpwm_reset_hal();
	host_pwrctrl_rpwm_set_time(0);
	host_pwrctrl_rpwm_set_cpwm_behavior((u8)v->cpwm_orig,
					    (unsigned int)v->poll_reads_before_toggle);
	memset(g_adapter, 0, sizeof(g_adapter));

	ret = rtw_cpwm_polling(g_adapter, (u8)v->rpwm_in, (u8)v->cpwm_orig_in);
	if (v->has_expect_success) {
		u8 expect = v->expect_success ? _SUCCESS : _FAIL;

		if (ret != expect) {
			fprintf(stderr, "%s: ret %u expect %u\n", v->name, ret, expect);
			return -1;
		}
	}
	if (v->has_expect_cpwm &&
	    host_pwrctrl_rpwm_get_cpwm(g_adapter) != (u8)v->expect_cpwm) {
		fprintf(stderr, "%s: cpwm mismatch\n", v->name);
		return -1;
	}
	if (v->has_expect_cpwm_tog &&
	    host_pwrctrl_rpwm_get_cpwm_tog(g_adapter) != (u8)v->expect_cpwm_tog) {
		fprintf(stderr, "%s: cpwm_tog mismatch\n", v->name);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "rpwm_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
