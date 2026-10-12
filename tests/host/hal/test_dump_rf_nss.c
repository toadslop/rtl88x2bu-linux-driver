// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for _dump_rf_path / _dump_trx_nss (W4-08). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_intf_rf_nss_types.h"
#include "host_vector_json.h"

#ifdef RUST_HAL_INTF_RF_NSS_ORACLE
int dump_rf_path_format(const host_hal_intf_rf_path_fixture *fx, char *buf,
			size_t buflen);
int dump_trx_nss_format(const host_hal_intf_trx_nss_fixture *fx, char *buf,
			size_t buflen);
#endif

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 512

struct rf_vector {
	char name[MAX_NAME];
	host_hal_intf_rf_path_fixture fx;
	char expect[MAX_EXPECT];
};

struct nss_vector {
	char name[MAX_NAME];
	host_hal_intf_trx_nss_fixture fx;
	char expect[MAX_EXPECT];
};

static void unescape_expect(char *expect)
{
	char *w = expect;

	for (char *r = expect; *r; r++) {
		if (*r == '\\' && r[1] == 'n') {
			*w++ = '\n';
			r++;
		} else {
			*w++ = *r;
		}
	}
	*w = '\0';
}

static int parse_rf_vector(const char *obj, size_t len, void *vec_void)
{
	struct rf_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "ver_id_rf_type", &tmp))
		return -1;
	v->fx.ver_id_rf_type = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rf_reg_trx_path_bmp", &tmp))
		return -1;
	v->fx.rf_reg_trx_path_bmp = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rf_reg_path_avail_num", &tmp))
		return -1;
	v->fx.rf_reg_path_avail_num = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "hal_spec_max_tx_cnt", &tmp))
		return -1;
	v->fx.hal_spec_max_tx_cnt = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "eeprom_trx_path_bmp", &tmp))
		return -1;
	v->fx.eeprom_trx_path_bmp = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "eeprom_max_tx_cnt", &tmp))
		return -1;
	v->fx.eeprom_max_tx_cnt = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "reg_trx_path_bmp", &tmp))
		return -1;
	v->fx.reg_trx_path_bmp = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "tx_path_lmt", &tmp))
		return -1;
	v->fx.tx_path_lmt = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "rx_path_lmt", &tmp))
		return -1;
	v->fx.rx_path_lmt = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "hal_trx_path_bmp", &tmp))
		return -1;
	v->fx.hal_trx_path_bmp = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "hal_max_tx_cnt", &tmp))
		return -1;
	v->fx.hal_max_tx_cnt = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "hal_rf_type", &tmp))
		return -1;
	v->fx.hal_rf_type = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "NumTotalRFPath", &tmp))
		return -1;
	v->fx.NumTotalRFPath = (int8_t)tmp;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static int parse_nss_vector(const char *obj, size_t len, void *vec_void)
{
	struct nss_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "hal_spec_tx_nss", &tmp))
		return -1;
	v->fx.hal_spec_tx_nss = (int8_t)tmp;
	if (host_json_parse_int_in(obj, len, "hal_spec_rx_nss", &tmp))
		return -1;
	v->fx.hal_spec_rx_nss = (int8_t)tmp;
	if (host_json_parse_int_in(obj, len, "reg_tx_nss", &tmp))
		return -1;
	v->fx.reg_tx_nss = (int8_t)tmp;
	if (host_json_parse_int_in(obj, len, "reg_rx_nss", &tmp))
		return -1;
	v->fx.reg_rx_nss = (int8_t)tmp;
	if (host_json_parse_int_in(obj, len, "hal_tx_nss", &tmp))
		return -1;
	v->fx.hal_tx_nss = (int8_t)tmp;
	if (host_json_parse_int_in(obj, len, "hal_rx_nss", &tmp))
		return -1;
	v->fx.hal_rx_nss = (int8_t)tmp;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static int run_rf_path_suite(const char *path)
{
	struct rf_vector vectors[MAX_VECTORS];
	size_t count = 0;
	char got[MAX_EXPECT];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_rf_vector, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (size_t i = 0; i < count; i++) {
		int n = dump_rf_path_format(&vectors[i].fx, got, sizeof(got));

		if (n < 0) {
			fprintf(stderr, "FAIL %s: format returned %d\n",
				vectors[i].name, n);
			return 1;
		}
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s: output mismatch\n",
				vectors[i].name);
			fprintf(stderr, "expected:\n%s\n", vectors[i].expect);
			fprintf(stderr, "got:\n%s\n", got);
			return 1;
		}
		printf("PASS rf_path %s\n", vectors[i].name);
	}
	return 0;
}

static int run_trx_nss_suite(const char *path)
{
	struct nss_vector vectors[MAX_VECTORS];
	size_t count = 0;
	char got[MAX_EXPECT];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_nss_vector, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (size_t i = 0; i < count; i++) {
		int n = dump_trx_nss_format(&vectors[i].fx, got, sizeof(got));

		if (n < 0) {
			fprintf(stderr, "FAIL %s: format returned %d\n",
				vectors[i].name, n);
			return 1;
		}
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s: output mismatch\n",
				vectors[i].name);
			fprintf(stderr, "expected:\n%s\n", vectors[i].expect);
			fprintf(stderr, "got:\n%s\n", got);
			return 1;
		}
		printf("PASS trx_nss %s\n", vectors[i].name);
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *rf_path =
		argc > 1 ? argv[1] : "hal_dump_rf_path_vectors.json";
	const char *trx_nss =
		argc > 2 ? argv[2] : "hal_dump_trx_nss_vectors.json";

	if (run_rf_path_suite(rf_path))
		return 1;
	if (run_trx_nss_suite(trx_nss))
		return 1;
	return 0;
}
