// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-38 AID + pre-link sta helpers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_sta_mgt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_PRE_LINK 4

enum sta_mgt_fn {
	FN_AID_ALLOC,
	FN_IS_PRE_LINK,
	FN_PRE_LINK_DEL,
	FN_PRE_LINK_CTL_RESET,
};

struct vector {
	char name[MAX_NAME];
	enum sta_mgt_fn fn;
	u8 mac[ETH_ALEN];
	u16 max_aid;
	u16 max_num_sta;
	u8 rr_aid;
	int occupied_aids;
	u8 pre_link_macs[MAX_PRE_LINK][ETH_ALEN];
	int pre_link_count;
	uint sta_state;
	int expect;
};

u16 rtw_aid_alloc(_adapter *adapter, struct sta_info *sta);
bool rtw_is_pre_link_sta(struct sta_priv *stapriv, u8 *addr);
void rtw_pre_link_sta_del(struct sta_priv *stapriv, u8 *hwaddr);
void rtw_pre_link_sta_ctl_reset(struct sta_priv *stapriv);

static int parse_fn(const char *obj, size_t len, enum sta_mgt_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_aid_alloc"))
		*out = FN_AID_ALLOC;
	else if (!strcmp(fn, "rtw_is_pre_link_sta"))
		*out = FN_IS_PRE_LINK;
	else if (!strcmp(fn, "rtw_pre_link_sta_del"))
		*out = FN_PRE_LINK_DEL;
	else if (!strcmp(fn, "rtw_pre_link_sta_ctl_reset"))
		*out = FN_PRE_LINK_CTL_RESET;
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

static int parse_pre_link_macs(const char *obj, size_t len, struct vector *v)
{
	char key[24];
	int i;

	for (i = 0; i < MAX_PRE_LINK; i++) {
		snprintf(key, sizeof(key), "pre_link_mac%d", i);
		if (!host_json_find_key_in(obj, len, key))
			break;
		if (parse_hex_opt(obj, len, key, v->pre_link_macs[i], ETH_ALEN))
			return -1;
		v->pre_link_count++;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect", &v->expect))
		return -1;
	host_json_parse_int_in(obj, len, "max_aid", (int *)&v->max_aid);
	host_json_parse_int_in(obj, len, "max_num_sta", (int *)&v->max_num_sta);
	host_json_parse_int_in(obj, len, "rr_aid", (int *)&v->rr_aid);
	host_json_parse_int_in(obj, len, "occupied_aids", &v->occupied_aids);
	host_json_parse_int_in(obj, len, "sta_state", (int *)&v->sta_state);
	if (parse_hex_opt(obj, len, "mac", v->mac, ETH_ALEN))
		return -1;
	return parse_pre_link_macs(obj, len, v);
}

static void setup_aid(_adapter *a, struct vector *v)
{
	int i;
	u8 mac[ETH_ALEN] = {0x02, 0, 0, 0, 0, 0};
	struct sta_info *sta;

	host_sta_mgt_reset(a);
	if (host_sta_mgt_aid_setup(a, v->max_aid ? v->max_aid : 4,
				   v->max_num_sta ? v->max_num_sta : 4,
				   v->rr_aid))
		return;
	for (i = 0; i < v->occupied_aids; i++) {
		mac[5] = (u8)(i + 1);
		sta = host_sta_mgt_sta_add(a, mac, 0);
		if (!sta)
			return;
		rtw_aid_alloc(a, sta);
	}
}

static void setup_pre_link(_adapter *a, struct vector *v)
{
	int i;

	host_sta_mgt_reset(a);
	host_sta_mgt_pre_link_init(a);
	a->stapriv.padapter = a;
	for (i = 0; i < v->pre_link_count; i++) {
		host_sta_mgt_pre_link_add(a, v->pre_link_macs[i]);
		if (v->sta_state)
			host_sta_mgt_sta_add(a, v->pre_link_macs[i], v->sta_state);
	}
}

static int run_vector(struct vector *v)
{
	_adapter a;
	struct sta_info *sta;
	int got;

	memset(&a, 0, sizeof(a));
	switch (v->fn) {
	case FN_AID_ALLOC: {
		u8 mac[ETH_ALEN] = {0x02, 0, 0, 0, 0, 0x10};

		setup_aid(&a, v);
		sta = host_sta_mgt_sta_add(&a, mac, 0);
		if (!sta)
			return -1;
		got = rtw_aid_alloc(&a, sta);
		break;
	}
	case FN_IS_PRE_LINK:
		setup_pre_link(&a, v);
		got = rtw_is_pre_link_sta(&a.stapriv, v->mac) ? 1 : 0;
		break;
	case FN_PRE_LINK_DEL:
		setup_pre_link(&a, v);
		rtw_pre_link_sta_del(&a.stapriv, v->mac);
		got = host_sta_mgt_pre_link_count(&a);
		if (got == v->expect && v->expect == 0)
			got = rtw_get_stainfo(&a.stapriv, v->mac) ? 1 : 0;
		break;
	case FN_PRE_LINK_CTL_RESET:
		setup_pre_link(&a, v);
		rtw_pre_link_sta_ctl_reset(&a.stapriv);
		got = host_sta_mgt_pre_link_count(&a);
		break;
	default:
		return -1;
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
