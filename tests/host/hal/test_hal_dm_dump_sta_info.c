// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W4-31 dump_sta_info formatter. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_dump_sta_info_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 1024

struct vector {
	char name[MAX_NAME];
	int null_sta;
	struct host_hal_dm_dump_sta_fixture fx;
	char expect[MAX_EXPECT];
};

static int parse_mac_hex(const char *hex, uint8_t *out, size_t out_len)
{
	size_t i;

	if (strlen(hex) != out_len * 2)
		return -1;
	for (i = 0; i < out_len; i++) {
		unsigned int byte;

		if (sscanf(hex + i * 2, "%2x", &byte) != 1)
			return -1;
		out[i] = (uint8_t)byte;
	}
	return 0;
}

static int parse_u64_hex(const char *hex, uint64_t *out)
{
	unsigned long long v;

	if (sscanf(hex, "%llx", &v) != 1)
		return -1;
	*out = (uint64_t)v;
	return 0;
}

static void unescape_expect_newlines(char *s)
{
	char *r = s;
	char *w = s;

	while (*r) {
		if (r[0] == '\\' && r[1] == 'n') {
			*w++ = '\n';
			r += 2;
			continue;
		}
		*w++ = *r++;
	}
	*w = '\0';
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char mac_hex[20];
	char ramask_hex[24];
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "null_sta", &tmp))
		v->null_sta = tmp;
	if (v->null_sta)
		return host_json_parse_string_in(obj, len, "expect", v->expect,
						 sizeof(v->expect));
	if (host_json_parse_string_in(obj, len, "mac", mac_hex, sizeof(mac_hex)))
		return -1;
	if (parse_mac_hex(mac_hex, v->fx.mac_addr, sizeof(v->fx.mac_addr)))
		return -1;
	if (host_json_parse_int_in(obj, len, "mac_id", &tmp))
		return -1;
	v->fx.mac_id = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "wireless_mode", &tmp))
		return -1;
	v->fx.wireless_mode = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "mimo_type", &tmp))
		return -1;
	v->fx.mimo_type = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "sm_ps", &tmp))
		return -1;
	v->fx.sm_ps = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "bw_mode", &tmp))
		return -1;
	v->fx.bw_mode = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "ra_bw_mode", &tmp))
		return -1;
	v->fx.ra_bw_mode = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rate_id", &tmp))
		return -1;
	v->fx.rate_id = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rssi", &tmp))
		return -1;
	v->fx.rssi = (int8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rssi_level", &tmp))
		return -1;
	v->fx.rssi_level = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "is_support_sgi", &tmp))
		return -1;
	v->fx.is_support_sgi = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "is_vht_enable", &tmp))
		return -1;
	v->fx.is_vht_enable = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "disable_ra", &tmp))
		return -1;
	v->fx.disable_ra = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "disable_pt", &tmp))
		return -1;
	v->fx.disable_pt = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "is_noisy", &tmp))
		return -1;
	v->fx.is_noisy = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "txrx_state", &tmp))
		return -1;
	v->fx.txrx_state = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "stub_tx_rate", &tmp))
		return -1;
	v->fx.stub_tx_rate = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "stub_tx_sgi", &tmp))
		return -1;
	v->fx.stub_tx_sgi = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "curr_tx_bw", &tmp))
		return -1;
	v->fx.curr_tx_bw = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "curr_retry_ratio", &tmp))
		return -1;
	v->fx.curr_retry_ratio = (uint8_t)tmp;
	if (host_json_parse_string_in(obj, len, "ramask", ramask_hex,
				      sizeof(ramask_hex)))
		return -1;
	if (parse_u64_hex(ramask_hex, &v->fx.ramask))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	unescape_expect_newlines(v->expect);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_dm_dump_sta_info_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		if (vectors[i].null_sta) {
			dump_sta_info((void *)1, NULL);
			if (vectors[i].expect[0] != '\0') {
				fprintf(stderr, "FAIL %s: null_sta expect must be empty\n",
					vectors[i].name);
				return 1;
			}
			continue;
		}

		char got[MAX_EXPECT];
		int n;

		n = dump_sta_info_format(&vectors[i].fx, got, sizeof(got));
		if (n < 0) {
			fprintf(stderr, "FAIL %s: format returned %d\n",
				vectors[i].name, n);
			return 1;
		}
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n",
				vectors[i].name, got, vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
