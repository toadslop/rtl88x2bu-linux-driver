// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_hal_rfpath_init common path (W4-65). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_intf_rfpath_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	struct host_hal_rfpath_init_in in;
	unsigned expect_status;
	unsigned expect_fail_reason;
	unsigned expect_hal_trx_path_bmp;
	unsigned expect_hal_rf_type;
	unsigned expect_NumTotalRFPath;
	unsigned expect_hal_max_tx_cnt;
};

static int parse_u8_in(const char *obj, size_t len, const char *key, u8 *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u8)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_in(obj, len, "rf_reg_trx_path_bmp", &v->in.rf_reg_trx_path_bmp))
		return -1;
	if (parse_u8_in(obj, len, "reg_trx_path_bmp", &v->in.reg_trx_path_bmp))
		return -1;
	if (parse_u8_in(obj, len, "eeprom_trx_path_bmp", &v->in.eeprom_trx_path_bmp))
		return -1;
	if (parse_u8_in(obj, len, "eeprom_max_tx_cnt", &v->in.eeprom_max_tx_cnt))
		return -1;
	if (parse_u8_in(obj, len, "hal_spec_max_tx_cnt", &v->in.hal_spec_max_tx_cnt))
		return -1;
	if (parse_u8_in(obj, len, "tx_path_lmt", &v->in.tx_path_lmt))
		return -1;
	if (parse_u8_in(obj, len, "rx_path_lmt", &v->in.rx_path_lmt))
		return -1;
	{
		int tmp;

		if (host_json_parse_int_in(obj, len, "expect_status", &tmp))
			return -1;
		v->expect_status = (unsigned)tmp;
		if (host_json_parse_int_in(obj, len, "expect_fail_reason", &tmp))
			return -1;
		v->expect_fail_reason = (unsigned)tmp;
		if (host_json_parse_int_in(obj, len, "expect_hal_trx_path_bmp", &tmp))
			return -1;
		v->expect_hal_trx_path_bmp = (unsigned)tmp;
		if (host_json_parse_int_in(obj, len, "expect_hal_rf_type", &tmp))
			return -1;
		v->expect_hal_rf_type = (unsigned)tmp;
		if (host_json_parse_int_in(obj, len, "expect_NumTotalRFPath", &tmp))
			return -1;
		v->expect_NumTotalRFPath = (unsigned)tmp;
		if (host_json_parse_int_in(obj, len, "expect_hal_max_tx_cnt", &tmp))
			return -1;
		v->expect_hal_max_tx_cnt = (unsigned)tmp;
	}
	return 0;
}

static int cmp_out(const struct vector *v, const struct host_hal_rfpath_init_out *got)
{
	if (got->status != v->expect_status ||
	    got->fail_reason != v->expect_fail_reason) {
		fprintf(stderr,
			"FAIL %s: status=%u fail_reason=%u (expect status=%u reason=%u)\n",
			v->name, got->status, got->fail_reason, v->expect_status,
			v->expect_fail_reason);
		return -1;
	}
	if (v->expect_status != 1)
		return 0;
	if (got->hal_trx_path_bmp != v->expect_hal_trx_path_bmp ||
	    got->hal_rf_type != v->expect_hal_rf_type ||
	    got->NumTotalRFPath != v->expect_NumTotalRFPath ||
	    got->hal_max_tx_cnt != v->expect_hal_max_tx_cnt) {
		fprintf(stderr,
			"FAIL %s: out bmp=%u rf=%u paths=%u max_tx=%u "
			"(expect bmp=%u rf=%u paths=%u max_tx=%u)\n",
			v->name, got->hal_trx_path_bmp, got->hal_rf_type,
			got->NumTotalRFPath, got->hal_max_tx_cnt,
			v->expect_hal_trx_path_bmp, v->expect_hal_rf_type,
			v->expect_NumTotalRFPath, v->expect_hal_max_tx_cnt);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_intf_rfpath_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct host_hal_rfpath_init_out got;

		memset(&got, 0, sizeof(got));
		(void)host_hal_rfpath_init_common(&vectors[i].in, &got);
		if (cmp_out(&vectors[i], &got))
			return 1;
	}

	printf("ok %zu vectors\n", count);
	return 0;
}
