// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for TKIP phase1/phase2 key expansion (T5 / W3-07b).
 */

#if defined(HOST_TKIP_PHASE_ORACLE_BUILD) || defined(RUST_SECURITY_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

enum tkip_phase_fn {
	FN_PHASE1 = 0,
	FN_PHASE2,
	FN_PHASE_CHAIN,
};

struct vector {
	char name[MAX_NAME];
	enum tkip_phase_fn fn;
	u8 tk[16];
	u8 ta[6];
	u32 iv32;
	u16 iv16;
	u16 p1k[5];
	u8 expect_p1k[10];
	u8 expect_rc4key[16];
};

#ifdef RUST_SECURITY_ORACLE
extern void host_tkip_phase1(u16 *p1k, const u8 *tk, const u8 *ta, u32 iv32);
extern void host_tkip_phase2(u8 *rc4key, const u8 *tk, const u16 *p1k, u16 iv16);
#else
void host_tkip_phase1(u16 *p1k, const u8 *tk, const u8 *ta, u32 iv32);
void host_tkip_phase2(u8 *rc4key, const u8 *tk, const u16 *p1k, u16 iv16);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum tkip_phase_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_tkip_phase1") == 0)
		*out = FN_PHASE1;
	else if (strcmp(fn, "host_tkip_phase2") == 0)
		*out = FN_PHASE2;
	else if (strcmp(fn, "host_tkip_phase_chain") == 0)
		*out = FN_PHASE_CHAIN;
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

static int parse_iv32(const char *obj, size_t obj_len, u32 *out)
{
	u8 buf[4];
	size_t len = 0;

	if (parse_hex_field(obj, obj_len, "iv32", buf, sizeof(buf), &len) || len != 4)
		return -1;
	*out = (u32)buf[0] | ((u32)buf[1] << 8) | ((u32)buf[2] << 16) |
	       ((u32)buf[3] << 24);
	return 0;
}

static int parse_iv16(const char *obj, size_t obj_len, u16 *out)
{
	u8 buf[2];
	size_t len = 0;

	if (parse_hex_field(obj, obj_len, "iv16", buf, sizeof(buf), &len) || len != 2)
		return -1;
	*out = (u16)buf[0] | ((u16)buf[1] << 8);
	return 0;
}

static int parse_p1k_field(const char *obj, size_t obj_len, const char *key,
			   u16 *out)
{
	u8 buf[10];
	size_t len = 0;
	size_t i;

	if (parse_hex_field(obj, obj_len, key, buf, sizeof(buf), &len) || len != 10)
		return -1;
	for (i = 0; i < 5; i++)
		out[i] = (u16)buf[2 * i] | ((u16)buf[2 * i + 1] << 8);
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	size_t tk_len = 0;
	size_t ta_len = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (parse_hex_field(obj, obj_len, "tk", v->tk, sizeof(v->tk), &tk_len) ||
	    tk_len != 16)
		return -1;
	if (v->fn == FN_PHASE1 || v->fn == FN_PHASE_CHAIN) {
		if (parse_hex_field(obj, obj_len, "ta", v->ta, sizeof(v->ta), &ta_len) ||
		    ta_len != 6)
			return -1;
		if (parse_iv32(obj, obj_len, &v->iv32))
			return -1;
	}
	if (v->fn == FN_PHASE1) {
		if (parse_hex_field(obj, obj_len, "expect_p1k", v->expect_p1k,
				    sizeof(v->expect_p1k), &tk_len) || tk_len != 10)
			return -1;
	} else {
		if (v->fn == FN_PHASE2 &&
		    parse_p1k_field(obj, obj_len, "p1k", v->p1k))
			return -1;
		if (parse_iv16(obj, obj_len, &v->iv16))
			return -1;
		if (parse_hex_field(obj, obj_len, "expect_rc4key", v->expect_rc4key,
				    sizeof(v->expect_rc4key), &tk_len) || tk_len != 16)
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	u16 p1k[5];
	u8 rc4key[16];

	switch (v->fn) {
	case FN_PHASE1:
		host_tkip_phase1(p1k, v->tk, v->ta, v->iv32);
		if (memcmp(p1k, v->expect_p1k, sizeof(v->expect_p1k)) != 0) {
			fprintf(stderr, "%s: p1k mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_PHASE2:
		host_tkip_phase2(rc4key, v->tk, v->p1k, v->iv16);
		if (memcmp(rc4key, v->expect_rc4key, sizeof(rc4key)) != 0) {
			fprintf(stderr, "%s: rc4key mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_PHASE_CHAIN:
		host_tkip_phase1(p1k, v->tk, v->ta, v->iv32);
		host_tkip_phase2(rc4key, v->tk, p1k, v->iv16);
		if (memcmp(rc4key, v->expect_rc4key, sizeof(rc4key)) != 0) {
			fprintf(stderr, "%s: rc4key mismatch (chain)\n", v->name);
			return -1;
		}
		break;
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "tkip_phase_vectors.json";
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
	printf("all %zu tkip phase vectors passed\n", nvec);
	return 0;
}

#endif /* HOST_TKIP_PHASE_ORACLE_BUILD || RUST_SECURITY_ORACLE */
