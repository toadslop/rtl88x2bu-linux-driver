// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for dump_hal_spec (W4-41). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_hal_spec_types.h"
#include "host_vector_json.h"

#ifdef RUST_HAL_COM_HAL_SPEC_ORACLE
int dump_hal_spec_format(const host_hal_com_hal_spec_fixture *fx, char *buf,
			 size_t buflen);
#endif

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_EXPECT 1024

struct vector {
	char name[MAX_NAME];
	host_hal_com_hal_spec_fixture fx;
	char expect[MAX_EXPECT];
};

static int parse_u8_field(const char *obj, size_t len, const char *key,
			  uint8_t *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (uint8_t)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "macid_num", &v->fx.macid_num))
		return -1;
	if (parse_u8_field(obj, len, "macid_cap", &v->fx.macid_cap))
		return -1;
	if (parse_u8_field(obj, len, "sec_cap", &v->fx.sec_cap))
		return -1;
	if (parse_u8_field(obj, len, "sec_cam_ent_num", &v->fx.sec_cam_ent_num))
		return -1;
	if (parse_u8_field(obj, len, "rfpath_num_2g", &v->fx.rfpath_num_2g))
		return -1;
	if (parse_u8_field(obj, len, "rfpath_num_5g", &v->fx.rfpath_num_5g))
		return -1;
	if (parse_u8_field(obj, len, "rf_reg_path_num", &v->fx.rf_reg_path_num))
		return -1;
	if (parse_u8_field(obj, len, "rf_reg_path_avail_num",
			   &v->fx.rf_reg_path_avail_num))
		return -1;
	if (parse_u8_field(obj, len, "rf_reg_trx_path_bmp",
			   &v->fx.rf_reg_trx_path_bmp))
		return -1;
	if (parse_u8_field(obj, len, "max_tx_cnt", &v->fx.max_tx_cnt))
		return -1;
	if (parse_u8_field(obj, len, "tx_nss_num", &v->fx.tx_nss_num))
		return -1;
	if (parse_u8_field(obj, len, "rx_nss_num", &v->fx.rx_nss_num))
		return -1;
	if (parse_u8_field(obj, len, "band_cap", &v->fx.band_cap))
		return -1;
	if (parse_u8_field(obj, len, "bw_cap", &v->fx.bw_cap))
		return -1;
	if (parse_u8_field(obj, len, "proto_cap", &v->fx.proto_cap))
		return -1;
	if (parse_u8_field(obj, len, "txgi_max", &v->fx.txgi_max))
		return -1;
	if (parse_u8_field(obj, len, "txgi_pdbm", &v->fx.txgi_pdbm))
		return -1;
	if (parse_u8_field(obj, len, "wl_func", &v->fx.wl_func))
		return -1;
	if (parse_u8_field(obj, len, "tx_aclt_unit_factor",
			   &v->fx.tx_aclt_unit_factor))
		return -1;
	if (parse_u8_field(obj, len, "rx_tsf_filter", &v->fx.rx_tsf_filter))
		return -1;
	if (parse_u8_field(obj, len, "pg_txpwr_saddr", &v->fx.pg_txpwr_saddr))
		return -1;
	if (parse_u8_field(obj, len, "pg_txgi_diff_factor",
			   &v->fx.pg_txgi_diff_factor))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
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
	const char *path = argc > 1 ? argv[1] : "hal_dump_hal_spec_vectors.json";
	char got[MAX_EXPECT];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		int n = dump_hal_spec_format(&vectors[i].fx, got, sizeof(got));

		if (n < 0) {
			fprintf(stderr, "FAIL %s: format returned %d\n",
				vectors[i].name, n);
			return 1;
		}
		got[n] = '\0';
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s: output mismatch\n",
				vectors[i].name);
			fprintf(stderr, "expected:\n%s\n", vectors[i].expect);
			fprintf(stderr, "got:\n%s\n", got);
			return 1;
		}
		printf("PASS %s\n", vectors[i].name);
	}
	return 0;
}
