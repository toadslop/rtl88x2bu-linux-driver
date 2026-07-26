// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_security.c helpers (T5).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 64
#define MAX_NAME 128

enum security_fn {
	FN_TYPE_STR = 0,
	FN_BIP_TO_GMCS,
};

struct vector {
	char name[MAX_NAME];
	enum security_fn fn;
	int value;
	int expect_null;
	char expect_str[64];
	u32 expect_gmcs;
};

#ifdef RUST_SECURITY_ORACLE
extern const char *security_type_str(u8 value);
extern u32 security_type_bip_to_gmcs(enum security_type type);
#else
const char *security_type_str(u8 value);
u32 security_type_bip_to_gmcs(enum security_type type);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum security_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "security_type_str") == 0)
		*out = FN_TYPE_STR;
	else if (strcmp(fn, "security_type_bip_to_gmcs") == 0)
		*out = FN_BIP_TO_GMCS;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "value", &v->value))
		return -1;
	host_json_parse_bool_in(obj, obj_len, "expect_null", &v->expect_null);
	host_json_parse_string_in(obj, obj_len, "expect_str", v->expect_str,
				  sizeof(v->expect_str));
	{
		int gmcs = 0;

		if (!host_json_parse_int_in(obj, obj_len, "expect_gmcs", &gmcs))
			v->expect_gmcs = (u32)gmcs;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_TYPE_STR: {
		const char *got = security_type_str((u8)v->value);

		if (v->expect_null) {
			if (got) {
				fprintf(stderr, "%s: expected NULL got %s\n",
					v->name, got);
				return -1;
			}
			break;
		}
		if (!got || strcmp(got, v->expect_str) != 0) {
			fprintf(stderr, "%s: type_str mismatch got=%s expect=%s\n",
				v->name, got ? got : "(null)", v->expect_str);
			return -1;
		}
		break;
	}
	case FN_BIP_TO_GMCS: {
		u32 got = security_type_bip_to_gmcs((enum security_type)v->value);

		if (got != v->expect_gmcs) {
			fprintf(stderr, "%s: bip_to_gmcs mismatch got=%u expect=%u\n",
				v->name, got, v->expect_gmcs);
			return -1;
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "security_type_vectors.json";
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
	printf("all %zu security vectors passed (oracle: core/rtw_security.c)\n",
	       nvec);
	return 0;
}
