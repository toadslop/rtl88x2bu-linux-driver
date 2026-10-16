// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for rtw_halmac_get_version (W4-27). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_halmac_version_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define BUF_SIZE 48

struct vector {
	char name[48];
	int major_ver, prototype_ver, minor_ver, halmac_status, buf_len;
	char expect[24];
	int expect_unchanged;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_int_in(obj, len, "major_ver", &v->major_ver) ||
	    host_json_parse_int_in(obj, len, "prototype_ver", &v->prototype_ver) ||
	    host_json_parse_int_in(obj, len, "minor_ver", &v->minor_ver) ||
	    host_json_parse_int_in(obj, len, "halmac_status", &v->halmac_status) ||
	    host_json_parse_int_in(obj, len, "buf_len", &v->buf_len))
		return -1;
	if (!host_json_parse_int_in(obj, len, "expect_unchanged", &tmp))
		v->expect_unchanged = tmp;
	if (!v->expect_unchanged &&
	    host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_halmac_get_version_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned char before[BUF_SIZE], got[BUF_SIZE];
		size_t blen = (size_t)vectors[i].buf_len;

		if (blen > BUF_SIZE)
			return 1;

		host_halmac_version_stub_reset();
		host_halmac_version_stub.status =
			(enum halmac_ret_status)vectors[i].halmac_status;
		host_halmac_version_stub.ver.major_ver = (uint8_t)vectors[i].major_ver;
		host_halmac_version_stub.ver.prototype_ver =
			(uint8_t)vectors[i].prototype_ver;
		host_halmac_version_stub.ver.minor_ver = (uint8_t)vectors[i].minor_ver;

		memset(before, 0xA5, blen);
		memcpy(got, before, blen);
		rtw_halmac_get_version((char *)got, (uint32_t)blen);

		if (vectors[i].expect_unchanged) {
			if (memcmp(got, before, blen)) {
				fprintf(stderr, "FAIL %s: buffer changed\n", vectors[i].name);
				return 1;
			}
			continue;
		}
		if (strcmp((char *)got, vectors[i].expect)) {
			fprintf(stderr, "FAIL %s: got %s expect %s\n", vectors[i].name,
				(char *)got, vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
