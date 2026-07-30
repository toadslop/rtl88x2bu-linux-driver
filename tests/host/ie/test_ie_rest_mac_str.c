// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for string/MAC address helpers (W3-30).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

enum mac_str_fn {
	FN_STR_2CHAR2NUM = 0,
	FN_KEY_2CHAR2NUM,
	FN_MACSTR2NUM,
	FN_CONVERT_IP_ADDR,
	FN_CHECK_INVALID_MAC,
	FN_MACADDR_CFG,
};

struct vector {
	char name[MAX_NAME];
	enum mac_str_fn fn;
	u8 hch;
	u8 mch;
	u8 lch;
	char macstr[32];
	u8 mac[ETH_ALEN];
	u8 hw_mac[ETH_ALEN];
	u8 expect_mac[ETH_ALEN];
	int has_expect_mac;
	char initmac[32];
	u32 random32;
	int check_local_bit;
	u32 expect;
	int has_mac;
	int has_initmac;
	int has_hw_mac;
	int has_random32;
};

static int parse_fn(const char *obj, size_t obj_len, enum mac_str_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "str_2char2num") == 0)
		*out = FN_STR_2CHAR2NUM;
	else if (strcmp(fn, "key_2char2num") == 0)
		*out = FN_KEY_2CHAR2NUM;
	else if (strcmp(fn, "macstr2num") == 0)
		*out = FN_MACSTR2NUM;
	else if (strcmp(fn, "convert_ip_addr") == 0)
		*out = FN_CONVERT_IP_ADDR;
	else if (strcmp(fn, "rtw_check_invalid_mac_address") == 0)
		*out = FN_CHECK_INVALID_MAC;
	else if (strcmp(fn, "rtw_macaddr_cfg") == 0)
		*out = FN_MACADDR_CFG;
	else
		return -1;
	return 0;
}

static int parse_char_field(const char *obj, size_t obj_len, const char *key,
			    u8 *out)
{
	char buf[8];

	if (host_json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
		return -1;
	if (buf[0] == '\0')
		return -1;
	*out = (u8)buf[0];
	return 0;
}

static int parse_mac_hex(const char *obj, size_t obj_len, const char *key,
			 u8 *out, int *has)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t len = 0;

	*has = 0;
	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return 0;
	if (host_hex_decode(hex, out, ETH_ALEN, &len) || len != ETH_ALEN)
		return -1;
	*has = 1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "expect", (int *)&v->expect);
	host_json_parse_int_in(obj, obj_len, "check_local_bit", &v->check_local_bit);
	host_json_parse_int_in(obj, obj_len, "random32", (int *)&v->random32);
	parse_char_field(obj, obj_len, "hch", &v->hch);
	parse_char_field(obj, obj_len, "mch", &v->mch);
	parse_char_field(obj, obj_len, "lch", &v->lch);
	if (host_json_parse_string_in(obj, obj_len, "macstr", v->macstr,
				    sizeof(v->macstr)))
		memset(v->macstr, 0, sizeof(v->macstr));
	if (host_json_parse_string_in(obj, obj_len, "initmac", v->initmac,
				    sizeof(v->initmac)))
		memset(v->initmac, 0, sizeof(v->initmac));
	else if (v->initmac[0])
		v->has_initmac = 1;
	if (parse_mac_hex(obj, obj_len, "mac", v->mac, &v->has_mac))
		return -1;
	if (parse_mac_hex(obj, obj_len, "hw_mac", v->hw_mac, &v->has_hw_mac))
		return -1;
	if (parse_mac_hex(obj, obj_len, "expect_mac", v->expect_mac,
			  &v->has_expect_mac))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "random32", (int *)&v->random32) == 0)
		v->has_random32 = 1;
	return 0;
}

static int mac_equal(const u8 *a, const u8 *b)
{
	return memcmp(a, b, ETH_ALEN) == 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_STR_2CHAR2NUM: {
		u8 ret = str_2char2num(v->hch, v->lch);

		if (ret != (u8)v->expect) {
			fprintf(stderr, "%s: str_2char2num got %u expect %u\n",
				v->name, ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_KEY_2CHAR2NUM: {
		u8 ret = key_2char2num(v->hch, v->lch);

		if (ret != (u8)v->expect) {
			fprintf(stderr, "%s: key_2char2num got %u expect %u\n",
				v->name, ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_MACSTR2NUM: {
		u8 out[ETH_ALEN];

		macstr2num(out, (u8 *)v->macstr);
		if (!mac_equal(out, v->expect_mac)) {
			fprintf(stderr, "%s: macstr2num mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_CONVERT_IP_ADDR: {
		u8 ret = convert_ip_addr(v->hch, v->mch, v->lch);

		if (ret != (u8)v->expect) {
			fprintf(stderr, "%s: convert_ip_addr got %u expect %u\n",
				v->name, ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_CHECK_INVALID_MAC: {
		u8 ret = rtw_check_invalid_mac_address(v->mac,
						       (u8)v->check_local_bit);

		if (ret != (u8)v->expect) {
			fprintf(stderr, "%s: invalid_mac got %u expect %u\n",
				v->name, ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_MACADDR_CFG: {
		u8 out[ETH_ALEN];
		const u8 *hw = v->has_hw_mac ? v->hw_mac : NULL;

		host_mac_str_test_clear_initmac();
		if (v->has_initmac)
			host_mac_str_test_set_initmac(v->initmac);
		if (v->has_random32)
			host_mac_str_test_set_random32(v->random32);
		rtw_macaddr_cfg(out, hw);
		host_mac_str_test_clear_initmac();
		if (!mac_equal(out, v->expect_mac)) {
			fprintf(stderr, "%s: macaddr_cfg mismatch\n", v->name);
			return -1;
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = "ie_rest_mac_str_vectors.json";
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			failed++;
		}
	}

	if (failed)
		return 1;

	printf("PASS: %zu vectors\n", count);
	return 0;
}
