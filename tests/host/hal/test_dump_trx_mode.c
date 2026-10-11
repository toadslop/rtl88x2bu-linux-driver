// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for dump_hal_trx_mode formatter (W4-07). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_intf_trx_types.h"
#include "host_vector_json.h"

#ifdef RUST_HAL_INTF_TRX_ORACLE
int dump_hal_trx_mode_format(const host_hal_intf_trx_fixture *fx, char *buf,
			     size_t buflen);
#else
/* C oracle linked from hal_dump_trx_mode_c_oracle.c */
#endif

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 512

struct vector {
	char name[MAX_NAME];
	host_hal_intf_trx_fixture fx;
	char expect[MAX_EXPECT];
};

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

static int parse_u32_array_in(const char *obj, size_t obj_len, const char *key,
			      uint32_t *out, size_t out_cap, size_t *count_out)
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
		out[n++] = (uint32_t)val;
	}
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "trx_path_bmp", &tmp))
		return -1;
	v->fx.trx_path_bmp = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rf_type", &tmp))
		return -1;
	v->fx.rf_type = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "NumTotalRFPath", &tmp))
		return -1;
	v->fx.NumTotalRFPath = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "max_tx_cnt", &tmp))
		return -1;
	v->fx.max_tx_cnt = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "tx_nss", &tmp))
		return -1;
	v->fx.tx_nss = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rx_nss", &tmp))
		return -1;
	v->fx.rx_nss = (uint8_t)tmp;
	{
		size_t n = 0;
		if (parse_u8_array_in(obj, len, "txpath_cap_num_nss", v->fx.txpath_cap_num_nss,
				      4, &n))
			return -1;
	}
	if (host_json_parse_int_in(obj, len, "txpath", &tmp))
		return -1;
	v->fx.txpath = (uint32_t)tmp;
	if (host_json_parse_int_in(obj, len, "rxpath", &tmp))
		return -1;
	v->fx.rxpath = (uint32_t)tmp;
	{
		size_t n = 0;
		if (parse_u32_array_in(obj, len, "txpath_nss", v->fx.txpath_nss, 4, &n))
			return -1;
	}
	{
		size_t n = 0;
		if (parse_u8_array_in(obj, len, "txpath_num_nss", v->fx.txpath_num_nss, 4,
				      &n))
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	{
		char *w = v->expect;
		for (char *r = v->expect; *r; r++) {
			if (*r == '\\' && r[1] == 'n') {
				*w++ = '\n';
				r++;
			} else {
				*w++ = *r;
			}
		}
		*w = '\0';
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dump_trx_mode_vectors.json";
	char got[MAX_EXPECT];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		int n;

		n = dump_hal_trx_mode_format(&vectors[i].fx, got, sizeof(got));
		if (n < 0) {
			fprintf(stderr, "FAIL %s: format returned %d\n", vectors[i].name, n);
			return 1;
		}
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s: output mismatch\n", vectors[i].name);
			fprintf(stderr, "expected:\n%s\n", vectors[i].expect);
			fprintf(stderr, "got:\n%s\n", got);
			return 1;
		}
		printf("PASS %s\n", vectors[i].name);
	}
	return 0;
}
