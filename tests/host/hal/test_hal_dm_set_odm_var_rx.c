// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for W4-96 SetHalODMVar RX dump cases. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_set_odm_var_rx_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	char case_name[24];
	int is_linked;
	int rx_rate;
	int rssi_a;
	int rssi_b;
	int cur_igi;
	int phydm_min_rssi;
	int dm_min_rssi;
	int fa_cck;
	int fa_ofdm;
	int fa_total;
	char expect[1024];
};

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

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "case", v->case_name,
				      sizeof(v->case_name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "is_linked", &tmp))
		return -1;
	v->is_linked = tmp;
	if (host_json_parse_int_in(obj, len, "rx_rate", &tmp))
		return -1;
	v->rx_rate = tmp;
	if (host_json_parse_int_in(obj, len, "rssi_a", &tmp))
		return -1;
	v->rssi_a = tmp;
	if (host_json_parse_int_in(obj, len, "rssi_b", &tmp))
		return -1;
	v->rssi_b = tmp;
	if (host_json_parse_int_in(obj, len, "cur_igi", &tmp))
		return -1;
	v->cur_igi = tmp;
	if (host_json_parse_int_in(obj, len, "phydm_min_rssi", &tmp))
		return -1;
	v->phydm_min_rssi = tmp;
	if (host_json_parse_int_in(obj, len, "dm_min_rssi", &tmp))
		return -1;
	v->dm_min_rssi = tmp;
	if (host_json_parse_int_in(obj, len, "fa_cck", &tmp))
		return -1;
	v->fa_cck = tmp;
	if (host_json_parse_int_in(obj, len, "fa_ofdm", &tmp))
		return -1;
	v->fa_ofdm = tmp;
	if (host_json_parse_int_in(obj, len, "fa_total", &tmp))
		return -1;
	v->fa_total = tmp;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	unescape_expect_newlines(v->expect);
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = (argc > 1) ? argv[1]
				      : "hal_dm_set_odm_var_rx_vectors.json";
	struct host_hal_dm_print_capture cap;
	_adapter adapter;
	struct vector vectors[16];
	size_t nvec;
	unsigned int fail = 0;
	unsigned int i;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 16,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	if (nvec == 0) {
		fprintf(stderr, "no vectors in %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		const struct vector *v = &vectors[i];

		host_hal_dm_set_odm_var_rx_fixture(
			(u8)v->is_linked, (u8)v->rx_rate, (s8)v->rssi_a,
			(s8)v->rssi_b, (u8)v->cur_igi, (s8)v->phydm_min_rssi,
			(s8)v->dm_min_rssi, (u32)v->fa_cck, (u32)v->fa_ofdm,
			(u32)v->fa_total);
		host_hal_dm_print_capture_reset(&cap);
		memset(&adapter, 0, sizeof(adapter));

		if (!strcmp(v->case_name, "RX_INFO_DUMP")) {
			SetHalODMVar(&adapter, HAL_ODM_RX_INFO_DUMP, &cap, 0);
		} else if (!strcmp(v->case_name, "RX_DFRAME_INFO")) {
			SetHalODMVar(&adapter, HAL_ODM_RX_DFRAME_INFO, &cap, 0);
		} else {
			fprintf(stderr, "%s: unknown case %s\n", v->name,
				v->case_name);
			fail++;
			continue;
		}

		if (strcmp(host_hal_dm_print_capture_text(&cap), v->expect)) {
			fprintf(stderr, "FAIL %s\n--- got ---\n%s--- want ---\n%s",
				v->name, host_hal_dm_print_capture_text(&cap),
				v->expect);
			fail++;
		} else {
			printf("PASS %s\n", v->name);
		}
	}

	return fail ? 1 : 0;
}