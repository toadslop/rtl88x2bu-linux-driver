// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_dump_fw_info formatter (W4-06). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_fw_info_types.h"
#include "host_vector_json.h"

#ifdef RUST_HAL_COM_FW_ORACLE
int rtw_dump_fw_info_format(const host_hal_com_fw_fixture *fx, char *buf,
			    size_t buflen);
#else
/* C oracle linked from hal_dump_fw_info_c_oracle.c */
#endif

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_EXPECT 128

struct vector {
	char name[MAX_NAME];
	host_hal_com_fw_fixture fx;
	char expect[MAX_EXPECT];
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "b_fw_ready", &tmp))
		return -1;
	v->fx.b_fw_ready = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "firmware_version", &tmp))
		return -1;
	v->fx.firmware_version = (uint16_t)tmp;
	if (host_json_parse_int_in(obj, len, "firmware_sub_version", &tmp))
		return -1;
	v->fx.firmware_sub_version = (uint16_t)tmp;
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
	const char *path = argc > 1 ? argv[1] : "hal_dump_fw_info_vectors.json";
	char got[MAX_EXPECT];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		int n;

		n = rtw_dump_fw_info_format(&vectors[i].fx, got, sizeof(got));
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
		printf("PASS %s\n", vectors[i].name);
	}
	return 0;
}
