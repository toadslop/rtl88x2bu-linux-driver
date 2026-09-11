// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_sta_mgt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128

enum lookup_fn { FN_INIT, FN_GET, FN_OFFSET };

struct vector {
	char name[MAX_NAME];
	enum lookup_fn fn;
	u8 mac[ETH_ALEN], insert_mac[ETH_ALEN], sta_index;
	int expect_found, expect_offset, expect_st_ctl_cleared;
	int expect_hash_list_init;
};

static int parse_fn(const char *obj, size_t len, enum lookup_fn *out)
{
	char fn[64];
	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "_rtw_init_stainfo"))
		*out = FN_INIT;
	else if (!strcmp(fn, "rtw_get_stainfo"))
		*out = FN_GET;
	else if (!strcmp(fn, "rtw_get_stainfo_by_offset"))
		*out = FN_OFFSET;
	else
		return -1;
	return 0;
}

static int parse_mac_opt(const char *obj, size_t len, const char *key, u8 *out)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t n = 0;
	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return 0;
	return host_hex_decode(hex, out, ETH_ALEN, &n) || n != ETH_ALEN;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	memset(v, 0, sizeof(*v));
	v->expect_offset = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, len, "sta_index", (int *)&v->sta_index);
	host_json_parse_int_in(obj, len, "expect_found", &v->expect_found);
	host_json_parse_int_in(obj, len, "expect_offset", &v->expect_offset);
	host_json_parse_int_in(obj, len, "expect_st_ctl_cleared", &v->expect_st_ctl_cleared);
	host_json_parse_int_in(obj, len, "expect_hash_list_init", &v->expect_hash_list_init);
	return parse_mac_opt(obj, len, "mac", v->mac) ||
	       parse_mac_opt(obj, len, "insert_mac", v->insert_mac);
}

static int mac_set(const u8 *mac)
{
	return mac[0] || mac[1] || mac[2] || mac[3] || mac[4] || mac[5];
}

static int run_vector(const struct vector *v)
{
	_adapter a;
	struct sta_info *sta, *got;
	int i, off, found;

	host_sta_mgt_lookup_reset(&a);
	if (host_sta_mgt_lookup_buf_setup(&a))
		return -1;
	switch (v->fn) {
	case FN_INIT:
		sta = (struct sta_info *)(a.stapriv.pstainfo_buf +
					  v->sta_index * sizeof(*sta));
		sta->padapter = &a;
		memset(sta, 0xff, sizeof(*sta));
		_rtw_init_stainfo(sta);
		if (v->expect_hash_list_init && sta->hash_list.next != &sta->hash_list)
			return -1;
		if (v->expect_st_ctl_cleared)
			for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++)
				if (sta->st_ctl.reg[i].s_proto || sta->st_ctl.reg[i].rule)
					return -1;
		return 0;
	case FN_GET:
		if (mac_set(v->insert_mac))
			host_sta_mgt_lookup_hash_insert(&a, v->sta_index, v->insert_mac);
		got = rtw_get_stainfo(&a.stapriv, mac_set(v->mac) ? v->mac : NULL);
		found = got ? 1 : 0;
		return found == v->expect_found ? 0 : -1;
	case FN_OFFSET:
		got = rtw_get_stainfo_by_offset(&a.stapriv, v->sta_index);
		if (!got)
			return -1;
		off = (int)(((u8 *)got - a.stapriv.pstainfo_buf) / sizeof(*got));
		return off == v->expect_offset ? 0 : -1;
	default:
		return -1;
	}
}

int main(int argc, char **argv)
{
	struct vector v[MAX_VECTORS];
	size_t n = 0, i, fail = 0;
	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], v, sizeof(v[0]), MAX_VECTORS, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i])) {
			fprintf(stderr, "FAIL: %s\n", v[i].name);
			fail++;
		}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
