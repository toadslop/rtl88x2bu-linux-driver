// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for CCMP MIC/CTR construct helpers (W3-11).
 */

#if defined(HOST_CCMP_CONSTRUCT_ORACLE_BUILD) || defined(RUST_SECURITY_REST_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_MPDU 64

enum construct_fn {
	FN_MIC_IV = 0,
	FN_MIC_HEADER1,
	FN_MIC_HEADER2,
	FN_CTR_PRELOAD,
};

struct vector {
	char name[MAX_NAME];
	enum construct_fn fn;
	u8 mpdu[MAX_MPDU];
	size_t mpdu_len;
	u8 pn[6];
	int qc_exists;
	int a4_exists;
	unsigned int header_length;
	unsigned int payload_length;
	unsigned int frtype;
	int ctr;
	u8 expect[16];
};

#ifdef RUST_SECURITY_REST_ORACLE
extern void host_ccmp_construct_mic_iv(u8 *mic_iv, int qc_exists, int a4_exists,
				       u8 *mpdu, unsigned int payload_length,
				       u8 *pn_vector, unsigned int frtype);
extern void host_ccmp_construct_mic_header1(u8 *mic_header1, int header_length,
					    u8 *mpdu, unsigned int frtype);
extern void host_ccmp_construct_mic_header2(u8 *mic_header2, u8 *mpdu,
					    int a4_exists, int qc_exists);
extern void host_ccmp_construct_ctr_preload(u8 *ctr_preload, int a4_exists,
					    int qc_exists, u8 *mpdu,
					    u8 *pn_vector, int c,
					    unsigned int frtype);
#else
void host_ccmp_construct_mic_iv(u8 *mic_iv, int qc_exists, int a4_exists,
				u8 *mpdu, unsigned int payload_length,
				u8 *pn_vector, unsigned int frtype);
void host_ccmp_construct_mic_header1(u8 *mic_header1, int header_length,
				    u8 *mpdu, unsigned int frtype);
void host_ccmp_construct_mic_header2(u8 *mic_header2, u8 *mpdu,
				     int a4_exists, int qc_exists);
void host_ccmp_construct_ctr_preload(u8 *ctr_preload, int a4_exists,
				     int qc_exists, u8 *mpdu,
				     u8 *pn_vector, int c, unsigned int frtype);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum construct_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_ccmp_construct_mic_iv") == 0)
		*out = FN_MIC_IV;
	else if (strcmp(fn, "host_ccmp_construct_mic_header1") == 0)
		*out = FN_MIC_HEADER1;
	else if (strcmp(fn, "host_ccmp_construct_mic_header2") == 0)
		*out = FN_MIC_HEADER2;
	else if (strcmp(fn, "host_ccmp_construct_ctr_preload") == 0)
		*out = FN_CTR_PRELOAD;
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

static int parse_uint_field(const char *obj, size_t obj_len, const char *key,
			    unsigned int *out)
{
	int val;

	if (host_json_parse_int_in(obj, obj_len, key, &val))
		return -1;
	*out = (unsigned int)val;
	return 0;
}

static int parse_int_field(const char *obj, size_t obj_len, const char *key,
			   int *out)
{
	return host_json_parse_int_in(obj, obj_len, key, out);
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (parse_hex_field(obj, obj_len, "mpdu", v->mpdu, sizeof(v->mpdu), &v->mpdu_len))
		return -1;

	switch (v->fn) {
	case FN_MIC_IV:
		if (parse_int_field(obj, obj_len, "qc_exists", &v->qc_exists) ||
		    parse_int_field(obj, obj_len, "a4_exists", &v->a4_exists) ||
		    parse_uint_field(obj, obj_len, "payload_length", &v->payload_length) ||
		    parse_uint_field(obj, obj_len, "frtype", &v->frtype))
			return -1;
		{
			size_t pn_len;

			if (parse_hex_field(obj, obj_len, "pn", v->pn, sizeof(v->pn), &pn_len) ||
			    pn_len != 6)
				return -1;
		}
		break;
	case FN_MIC_HEADER1:
		if (parse_uint_field(obj, obj_len, "header_length", &v->header_length) ||
		    parse_uint_field(obj, obj_len, "frtype", &v->frtype))
			return -1;
		break;
	case FN_MIC_HEADER2:
		if (parse_int_field(obj, obj_len, "qc_exists", &v->qc_exists) ||
		    parse_int_field(obj, obj_len, "a4_exists", &v->a4_exists))
			return -1;
		break;
	case FN_CTR_PRELOAD:
		if (parse_int_field(obj, obj_len, "qc_exists", &v->qc_exists) ||
		    parse_int_field(obj, obj_len, "a4_exists", &v->a4_exists) ||
		    parse_uint_field(obj, obj_len, "frtype", &v->frtype) ||
		    parse_int_field(obj, obj_len, "ctr", &v->ctr))
			return -1;
		{
			size_t pn_len;

			if (parse_hex_field(obj, obj_len, "pn", v->pn, sizeof(v->pn), &pn_len) ||
			    pn_len != 6)
				return -1;
		}
		break;
	default:
		return -1;
	}
	{
		size_t expect_len;

		if (parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
				    &expect_len) || expect_len != 16)
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 out[16];

	switch (v->fn) {
	case FN_MIC_IV:
		host_ccmp_construct_mic_iv(out, v->qc_exists, v->a4_exists, v->mpdu,
					   v->payload_length, v->pn, v->frtype);
		break;
	case FN_MIC_HEADER1:
		host_ccmp_construct_mic_header1(out, (int)v->header_length, v->mpdu,
						v->frtype);
		break;
	case FN_MIC_HEADER2:
		host_ccmp_construct_mic_header2(out, v->mpdu, v->a4_exists, v->qc_exists);
		break;
	case FN_CTR_PRELOAD:
		host_ccmp_construct_ctr_preload(out, v->a4_exists, v->qc_exists, v->mpdu,
						v->pn, v->ctr, v->frtype);
		break;
	default:
		return -1;
	}

	if (memcmp(out, v->expect, 16) != 0) {
		fprintf(stderr, "%s: mismatch got=", v->name);
		for (int i = 0; i < 16; i++)
			fprintf(stderr, "%02x", out[i]);
		fprintf(stderr, " expect=");
		for (int i = 0; i < 16; i++)
			fprintf(stderr, "%02x", v->expect[i]);
		fprintf(stderr, "\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "ccmp_construct_vectors.json";
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
	printf("all %zu ccmp construct vectors passed (oracle: core/rtw_security_rest.c)\n",
	       nvec);
	return 0;
}

#endif /* HOST_CCMP_CONSTRUCT_ORACLE_BUILD || RUST_SECURITY_REST_ORACLE */
