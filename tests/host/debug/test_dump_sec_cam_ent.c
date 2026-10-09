// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-133 sec_cam_ent dump formatters. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_sec_cam_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64
#define MAX_EXPECT 200

enum vector_kind {
	VEC_TITLE = 0,
	VEC_ENT = 1,
};

struct vector {
	char name[MAX_NAME];
	int kind;
	int id;
	int has_id;
	u16 ctrl;
	u8 mac[ETH_ALEN];
	u8 key[16];
	char expect[MAX_EXPECT];
};

static int parse_hex_bytes(const char *hex, u8 *out, size_t out_len)
{
	size_t i;

	if (strlen(hex) != out_len * 2)
		return -1;
	for (i = 0; i < out_len; i++) {
		unsigned int byte;
		if (sscanf(hex + i * 2, "%2x", &byte) != 1)
			return -1;
		out[i] = (u8)byte;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;
	char hex[40];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "type", hex, sizeof(hex)))
		return -1;
	if (!strcmp(hex, "title")) {
		v->kind = VEC_TITLE;
		if (host_json_parse_int_in(obj, len, "has_id", &tmp))
			return -1;
		v->has_id = tmp;
	} else if (!strcmp(hex, "ent")) {
		v->kind = VEC_ENT;
		if (host_json_parse_int_in(obj, len, "id", &tmp))
			return -1;
		v->id = tmp;
		if (host_json_parse_int_in(obj, len, "ctrl", &tmp))
			return -1;
		v->ctrl = (u16)tmp;
		if (host_json_parse_string_in(obj, len, "mac", hex, sizeof(hex)))
			return -1;
		if (parse_hex_bytes(hex, v->mac, ETH_ALEN))
			return -1;
		if (host_json_parse_string_in(obj, len, "key", hex, sizeof(hex)))
			return -1;
		if (parse_hex_bytes(hex, v->key, 16))
			return -1;
	} else {
		return -1;
	}
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "debug_sec_cam_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	size_t titles = 0;
	size_t ents = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		char got[MAX_EXPECT];

		if (vectors[i].kind == VEC_TITLE) {
			if (dump_sec_cam_ent_title_format((u8)vectors[i].has_id, got,
							  sizeof(got)) < 0) {
				fprintf(stderr, "FAIL %s: title format error\n",
					vectors[i].name);
				return 1;
			}
			titles++;
		} else {
			struct sec_cam_ent ent;

			ent.ctrl = vectors[i].ctrl;
			memcpy(ent.mac, vectors[i].mac, ETH_ALEN);
			memcpy(ent.key, vectors[i].key, 16);
			if (dump_sec_cam_ent_format(&ent, vectors[i].id, got,
						    sizeof(got)) < 0) {
				fprintf(stderr, "FAIL %s: ent format error\n",
					vectors[i].name);
				return 1;
			}
			ents++;
		}
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n",
				vectors[i].name, got, vectors[i].expect);
			return 1;
		}
	}

	printf("ok: %zu title + %zu ent vectors\n", titles, ents);
	return 0;
}
