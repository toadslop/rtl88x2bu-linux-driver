// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_ap_sta_ie_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	unsigned char cap[2];
	unsigned short expect_capability;
	int expect_flags;
};

static int parse_hex(const char *hex, unsigned char *out, size_t cap, size_t *len)
{
	size_t n = strlen(hex), i;

	if (n != 4 || cap < 2)
		return -1;
	*len = 2;
	for (i = 0; i < 2; i++) {
		unsigned v;
		if (sscanf(hex + i * 2, "%2x", &v) != 1)
			return -1;
		out[i] = (unsigned char)v;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	char name[64];
	size_t decoded = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", name, sizeof(name)))
		return -1;
	memcpy(v->name, name, sizeof(v->name));
	if (host_json_parse_string_in(obj, len, "cap", hex, sizeof(hex)) ||
	    parse_hex(hex, v->cap, 2, &decoded))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_capability", &tmp))
		return -1;
	v->expect_capability = (unsigned short)tmp;
	if (host_json_parse_int_in(obj, len, "expect_flags", &tmp))
		return -1;
	v->expect_flags = tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[4];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "ap_sta_ie_vectors.json";
	_adapter ad;
	struct sta_info sta;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 4,
			      parse_vector_object, &count))
		return 1;
	for (i = 0; i < count; i++) {
		memset(&ad, 0, sizeof(ad));
		memset(&sta, 0, sizeof(sta));
		rtw_ap_parse_sta_capability(&ad, &sta, vectors[i].cap);
		if (sta.capability != vectors[i].expect_capability ||
		    sta.flags != vectors[i].expect_flags) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			return 1;
		}
	}
	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
