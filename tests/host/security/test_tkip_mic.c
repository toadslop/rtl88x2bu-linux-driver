// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for TKIP MIC helpers (T5 / W3-07a).
 */

#if defined(HOST_TKIP_ORACLE_BUILD) || defined(RUST_SECURITY_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_BUF 512

enum tkip_fn {
	FN_MIC_APPEND = 0,
	FN_CALC_TKIPMIC,
};

struct mic_data {
	u32 K0, K1;
	u32 L, R;
	u32 M;
	u32 nBytesInM;
};

struct vector {
	char name[MAX_NAME];
	enum tkip_fn fn;
	u8 key[16];
	size_t key_len;
	u8 header[MAX_BUF];
	size_t header_len;
	u8 data[MAX_BUF];
	size_t data_len;
	u8 expect_mic[8];
	int priority;
};

#ifdef RUST_SECURITY_ORACLE
extern void host_tkip_secmicsetkey(struct mic_data *pmicdata, u8 *key);
extern void host_tkip_secmicappendbyte(struct mic_data *pmicdata, u8 b);
extern void host_tkip_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nbytes);
extern void host_tkip_secgetmic(struct mic_data *pmicdata, u8 *dst);
extern void host_tkip_seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len,
				     u8 *mic_code, u8 pri);
#else
void host_tkip_secmicsetkey(struct mic_data *pmicdata, u8 *key);
void host_tkip_secmicappendbyte(struct mic_data *pmicdata, u8 b);
void host_tkip_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nbytes);
void host_tkip_secgetmic(struct mic_data *pmicdata, u8 *dst);
void host_tkip_seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len,
			      u8 *mic_code, u8 pri);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum tkip_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_tkip_secmicappendbyte") == 0)
		*out = FN_MIC_APPEND;
	else if (strcmp(fn, "host_tkip_seccalctkipmic") == 0)
		*out = FN_CALC_TKIPMIC;
	else
		return -1;
	return 0;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *out, size_t out_cap, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	return host_hex_decode(hex, out, out_cap, out_len);
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key), &v->key_len);
	parse_hex_field(obj, obj_len, "header", v->header, sizeof(v->header),
			&v->header_len);
	parse_hex_field(obj, obj_len, "data", v->data, sizeof(v->data), &v->data_len);
	host_json_parse_int_in(obj, obj_len, "priority", &v->priority);
	if (host_json_parse_string_in(obj, obj_len, "expect_mic", hex, sizeof(hex)))
		return -1;
	{
		size_t mic_len = 0;

		if (host_hex_decode(hex, v->expect_mic, sizeof(v->expect_mic),
				    &mic_len) || mic_len != 8)
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 mic[8];

	switch (v->fn) {
	case FN_MIC_APPEND: {
		struct mic_data md;
		size_t i;

		host_tkip_secmicsetkey(&md, v->key);
		for (i = 0; i < v->data_len; i++)
			host_tkip_secmicappendbyte(&md, v->data[i]);
		host_tkip_secgetmic(&md, mic);
		break;
	}
	case FN_CALC_TKIPMIC:
		host_tkip_seccalctkipmic(v->key, v->header, v->data,
					 (u32)v->data_len, mic, (u8)v->priority);
		break;
	default:
		return -1;
	}

	if (memcmp(mic, v->expect_mic, sizeof(mic)) != 0) {
		fprintf(stderr, "%s: mic mismatch\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "tkip_mic_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vectors[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	printf("all %zu tkip mic vectors passed (oracle: core/rtw_security.c)\n",
	       nvec);
	return 0;
}

#endif /* HOST_TKIP_ORACLE_BUILD || RUST_SECURITY_ORACLE */
