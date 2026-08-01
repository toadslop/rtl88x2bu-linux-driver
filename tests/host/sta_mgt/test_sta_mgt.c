// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for sta_mgt match rule + ACL helpers (W3-37). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_sta_mgt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

enum sta_mgt_fn { FN_MATCH, FN_ACL, FN_ACL_ALL };

struct vector {
	char name[MAX_NAME];
	enum sta_mgt_fn fn;
	u8 local_port[2];
	u8 remote_port[2];
	u8 mac[ETH_ALEN];
	u8 period;
	int period_mode[RTW_ACL_PERIOD_NUM];
	u8 period_macs[RTW_ACL_PERIOD_NUM][ETH_ALEN];
	bool period_mac_set[RTW_ACL_PERIOD_NUM];
	int expect;
};

bool test_st_match_rule(_adapter *a, u8 *ln, u8 *lp, u8 *rn, u8 *rp);
u8 _rtw_access_ctrl(_adapter *a, u8 period, const u8 *mac);
u8 rtw_access_ctrl(_adapter *a, const u8 *mac);

static int parse_fn(const char *obj, size_t len, enum sta_mgt_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "test_st_match_rule"))
		*out = FN_MATCH;
	else if (!strcmp(fn, "_rtw_access_ctrl"))
		*out = FN_ACL;
	else if (!strcmp(fn, "rtw_access_ctrl"))
		*out = FN_ACL_ALL;
	else
		return -1;
	return 0;
}

static int parse_hex_opt(const char *obj, size_t len, const char *key, u8 *out,
			 size_t out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;

	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return 0;
	if (host_hex_decode(hex, out, out_len, &decoded) || decoded != out_len)
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char key[16];
	int i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect", &v->expect))
		return -1;
	host_json_parse_int_in(obj, len, "period", (int *)&v->period);
	if (parse_hex_opt(obj, len, "local_port", v->local_port, 2))
		return -1;
	if (parse_hex_opt(obj, len, "remote_port", v->remote_port, 2))
		return -1;
	if (parse_hex_opt(obj, len, "mac", v->mac, ETH_ALEN))
		return -1;
	for (i = 0; i < RTW_ACL_PERIOD_NUM; i++) {
		snprintf(key, sizeof(key), "period%d_mode", i);
		host_json_parse_int_in(obj, len, key, &v->period_mode[i]);
		snprintf(key, sizeof(key), "period%d_mac", i);
		if (host_json_find_key_in(obj, len, key)) {
			v->period_mac_set[i] = true;
			if (parse_hex_opt(obj, len, key, v->period_macs[i], ETH_ALEN))
				return -1;
		}
	}
	return 0;
}

static void setup_acl(_adapter *a, struct vector *v)
{
	int p;

	host_sta_mgt_acl_reset(a);
	for (p = 0; p < RTW_ACL_PERIOD_NUM; p++) {
		host_sta_mgt_acl_set_mode(a, p, v->period_mode[p]);
		if (v->period_mac_set[p])
			host_sta_mgt_acl_add(a, p, v->period_macs[p]);
	}
}

static int run_vector(struct vector *v)
{
	_adapter a;
	u8 ln[ETH_ALEN] = {0}, rn[ETH_ALEN] = {0};
	int got;

	setup_acl(&a, v);
	switch (v->fn) {
	case FN_MATCH:
		got = test_st_match_rule(&a, ln, v->local_port, rn, v->remote_port) ?
		      1 : 0;
		break;
	case FN_ACL:
		got = _rtw_access_ctrl(&a, v->period, v->mac);
		break;
	default:
		got = rtw_access_ctrl(&a, v->mac);
		break;
	}
	if (got != v->expect) {
		fprintf(stderr, "FAIL %s: expect %d got %d\n", v->name, v->expect,
			got);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	int failures = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <vectors.json>\n", argv[0]);
		return 2;
	}
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", argv[1]);
		return 2;
	}
	for (i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			failures++;
	printf("%zu vectors, %d failures\n", count, failures);
	return failures ? 1 : 0;
}
