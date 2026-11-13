// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_hal_trxnss_init (W4-66). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_intf_trxnss_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	struct host_hal_trxnss_init_in in;
	unsigned expect_tx_nss;
	unsigned expect_rx_nss;
	unsigned expect_txpath_cap_num_nss[4];
};

static int parse_s8_in(const char *obj, size_t len, const char *key, int8_t *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (int8_t)tmp;
	return 0;
}

static int parse_u8_in(const char *obj, size_t len, const char *key, uint8_t *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (uint8_t)tmp;
	return 0;
}

static int parse_u8_array_in(const char *obj, size_t obj_len, const char *key,
			     uint8_t *out, size_t out_cap, size_t *count_out)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);
	size_t n = 0;

	if (!p || *p != '[')
		return -1;
	p++;
	for (;;) {
		int val;

		p = host_json_skip_ws(p);
		if (*p == ']') {
			p++;
			*count_out = n;
			return 0;
		}
		if (n >= out_cap)
			return -1;
		if (n > 0) {
			if (*p != ',')
				return -1;
			p++;
			p = host_json_skip_ws(p);
		}
		val = 0;
		if (*p == '-')
			return -1;
		while (*p >= '0' && *p <= '9') {
			val = val * 10 + (*p - '0');
			p++;
		}
		out[n++] = (uint8_t)val;
	}
}

static int parse_cap_array(const char *obj, size_t len, const char *key, unsigned *out,
			   size_t n)
{
	uint8_t tmp[4];
	size_t count;

	if (parse_u8_array_in(obj, len, key, tmp, n, &count) || count != n)
		return -1;
	for (size_t i = 0; i < n; i++)
		out[i] = tmp[i];
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_s8_in(obj, len, "hal_spec_tx_nss", &v->in.hal_spec_tx_nss))
		return -1;
	if (parse_s8_in(obj, len, "hal_spec_rx_nss", &v->in.hal_spec_rx_nss))
		return -1;
	if (parse_s8_in(obj, len, "reg_tx_nss", &v->in.reg_tx_nss))
		return -1;
	if (parse_s8_in(obj, len, "reg_rx_nss", &v->in.reg_rx_nss))
		return -1;
	if (parse_u8_in(obj, len, "max_tx_cnt", &v->in.max_tx_cnt))
		return -1;
	if (parse_u8_in(obj, len, "rf_path", &v->in.rf_path))
		return -1;
	if (parse_u8_in(obj, len, "hw_full_tx", &v->in.hw_full_tx))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_tx_nss", (int *)&v->expect_tx_nss))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_rx_nss", (int *)&v->expect_rx_nss))
		return -1;
	return parse_cap_array(obj, len, "expect_txpath_cap_num_nss",
			       v->expect_txpath_cap_num_nss, 4);
}

static int cmp_out(const struct vector *v, const struct host_hal_trxnss_init_out *got)
{
	size_t i;

	if (got->tx_nss != v->expect_tx_nss || got->rx_nss != v->expect_rx_nss)
		return -1;
	for (i = 0; i < 4; i++) {
		if (got->txpath_cap_num_nss[i] != v->expect_txpath_cap_num_nss[i])
			return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_intf_trxnss_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct host_hal_trxnss_init_out got;

		memset(&got, 0, sizeof(got));
		host_hal_trxnss_init_common(&vectors[i].in, &got);
		if (cmp_out(&vectors[i], &got)) {
			fprintf(stderr, "FAIL %s tx_nss=%u rx_nss=%u cap=%u,%u,%u,%u\n",
				vectors[i].name, got.tx_nss, got.rx_nss,
				got.txpath_cap_num_nss[0], got.txpath_cap_num_nss[1],
				got.txpath_cap_num_nss[2], got.txpath_cap_num_nss[3]);
			return 1;
		}
		printf("PASS trxnss %s\n", vectors[i].name);
	}

	printf("ok %zu vectors\n", count);
	return 0;
}
