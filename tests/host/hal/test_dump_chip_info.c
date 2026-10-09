// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for dump_chip_info formatter (W4-03). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_chip_info_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64
#define MAX_EXPECT 160

struct vector {
	char name[MAX_NAME];
	HAL_VERSION version;
	char expect[MAX_EXPECT];
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "ic_type", &tmp))
		return -1;
	v->version.ICType = (HAL_IC_TYPE_E)tmp;
	if (host_json_parse_int_in(obj, len, "chip_type", &tmp))
		return -1;
	v->version.ChipType = (HAL_CHIP_TYPE_E)tmp;
	if (host_json_parse_int_in(obj, len, "cut_version", &tmp))
		return -1;
	v->version.CUTVersion = (HAL_CUT_VERSION_E)tmp;
	if (host_json_parse_int_in(obj, len, "vendor_type", &tmp))
		return -1;
	v->version.VendorType = (HAL_VENDOR_E)tmp;
	if (host_json_parse_int_in(obj, len, "rf_type", &tmp))
		return -1;
	v->version.RFType = (HAL_RF_TYPE_E)tmp;
	if (host_json_parse_int_in(obj, len, "rom_ver", &tmp))
		return -1;
	v->version.ROMVer = (u8)tmp;
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	return 0;
}

#ifdef RUST_HAL_COM_CHIP_ORACLE
int dump_chip_info_format(HAL_VERSION chip_version, char *buf, size_t buflen);
#else
/* C oracle linked from hal_dump_chip_info_c_oracle.c */
#endif

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dump_chip_info_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		char got[128];
		int n;

		n = dump_chip_info_format(vectors[i].version, got, sizeof(got));
		if (n < 0) {
			fprintf(stderr, "FAIL %s: format returned %d\n",
				vectors[i].name, n);
			return 1;
		}
		if (n < 1 || got[n - 1] != '\n') {
			fprintf(stderr, "FAIL %s: missing trailing newline\n",
				vectors[i].name);
			return 1;
		}
		got[n - 1] = '\0';
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n",
				vectors[i].name, got, vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
