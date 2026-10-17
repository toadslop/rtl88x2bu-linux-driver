// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W4-32 dump_sta_traffic formatter. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_dump_sta_traffic_types.h"
#include "host_vector_json.h"

#ifdef RUST_HAL_DM_DUMP_STA_TRAFFIC_ORACLE
int dump_sta_traffic_format(const struct host_hal_dm_dump_sta_traffic_fixture *fx,
			    char *buf, size_t buflen);
#endif

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 2048

struct vector {
	char name[MAX_NAME];
	int null_sta;
	struct host_hal_dm_dump_sta_traffic_fixture fx;
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
	if (!host_json_parse_int_in(obj, len, "associated_to_ap", &tmp))
		v->fx.associated_to_ap = (uint8_t)tmp;
	if (!host_json_parse_int_in(obj, len, "bcn_cnt", &tmp))
		v->fx.bcn_cnt = (uint8_t)tmp;
	if (!host_json_parse_int_in(obj, len, "bcn_dtim", &tmp))
		v->fx.bcn_dtim = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "stub_tx_rate", &tmp))
		return -1;
	v->fx.stub_tx_rate = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "stub_tx_sgi", &tmp))
		return -1;
	v->fx.stub_tx_sgi = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "curr_rx_rate", &tmp))
		return -1;
	v->fx.curr_rx_rate = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "curr_rx_rate_bmc", &tmp))
		return -1;
	v->fx.curr_rx_rate_bmc = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rssi", &tmp))
		return -1;
	v->fx.rssi = (int8_t)tmp;
	if (!host_json_parse_int_in(obj, len, "tx_tp_kbits", &tmp))
		v->fx.tx_tp_kbits = (uint32_t)tmp;
	if (!host_json_parse_int_in(obj, len, "rx_tp_kbits", &tmp))
		v->fx.rx_tp_kbits = (uint32_t)tmp;
	if (!host_json_parse_int_in(obj, len, "smooth_tx_tp_kbits", &tmp))
		v->fx.smooth_tx_tp_kbits = (uint32_t)tmp;
	if (!host_json_parse_int_in(obj, len, "smooth_rx_tp_kbits", &tmp))
		v->fx.smooth_rx_tp_kbits = (uint32_t)tmp;
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
		argc > 1 ? argv[1] : "hal_dm_dump_sta_traffic_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		if (vectors[i].null_sta) {
			dump_sta_traffic((void *)1, NULL, NULL);
			if (vectors[i].expect[0] != '\0') {
				fprintf(stderr,
					"FAIL %s: null_sta expect must be empty\n",
					vectors[i].name);
				return 1;
			}
			continue;
		}

		char got[MAX_EXPECT];
		int n;

		n = dump_sta_traffic_format(&vectors[i].fx, got, sizeof(got));
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
