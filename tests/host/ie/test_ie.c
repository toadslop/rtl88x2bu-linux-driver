// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_ieee80211.c IE helpers (W3-03).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

typedef int sint;

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_IES 256

enum ie_fn {
	FN_GET_IE = 0,
	FN_GET_IE_EX,
	FN_REMOVE_IE,
};

struct vector {
	char name[MAX_NAME];
	enum ie_fn fn;
	u8 ies[MAX_IES];
	size_t ies_len;
	int index;
	int limit;
	int eid;
	u8 oui[8];
	int oui_len;
	int offset;
	int expect_len;
	int expect_ielen;
	int expect_found;
	int expect_ret;
	int copy_out;
	u8 expect_copy[MAX_IES];
	size_t expect_copy_len;
	u8 ies_mut[MAX_IES];
};

static int parse_fn(const char *obj, size_t obj_len, enum ie_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_get_ie") == 0)
		*out = FN_GET_IE;
	else if (strcmp(fn, "rtw_get_ie_ex") == 0)
		*out = FN_GET_IE_EX;
	else if (strcmp(fn, "rtw_ies_remove_ie") == 0)
		*out = FN_REMOVE_IE;
	else
		return -1;
	return 0;
}

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
u8 *rtw_get_ie_ex(const u8 *in_ie, unsigned int in_len, u8 eid, const u8 *oui,
		  u8 oui_len, u8 *ie, unsigned int *ielen);
int rtw_ies_remove_ie(u8 *ies, unsigned int *ies_len, unsigned int offset,
		      u8 eid, u8 *oui, u8 oui_len);

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "index", &v->index);
	host_json_parse_int_in(obj, obj_len, "limit", &v->limit);
	host_json_parse_int_in(obj, obj_len, "eid", &v->eid);
	host_json_parse_int_in(obj, obj_len, "offset", &v->offset);
	host_json_parse_int_in(obj, obj_len, "expect_len", &v->expect_len);
	host_json_parse_int_in(obj, obj_len, "expect_ielen", &v->expect_ielen);
	host_json_parse_int_in(obj, obj_len, "expect_found", &v->expect_found);
	host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, obj_len, "copy_out", &v->copy_out);
	if (host_json_parse_string_in(obj, obj_len, "ies", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->ies, sizeof(v->ies), &v->ies_len))
		return -1;
	if (!host_json_parse_string_in(obj, obj_len, "oui", hex, sizeof(hex))) {
		size_t oui_len = 0;

		if (host_hex_decode(hex, v->oui, sizeof(v->oui), &oui_len))
			return -1;
		v->oui_len = (int)oui_len;
	}
	if (!host_json_parse_string_in(obj, obj_len, "expect_copy", hex, sizeof(hex))) {
		if (host_hex_decode(hex, v->expect_copy, sizeof(v->expect_copy),
				    &v->expect_copy_len))
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_GET_IE: {
		sint len = -1;
		u8 *ie = rtw_get_ie(v->ies, v->index, &len, v->limit);

		if (v->expect_found) {
			if (!ie || len != v->expect_len) {
				fprintf(stderr, "%s: get_ie mismatch\n", v->name);
				return -1;
			}
		} else if (ie) {
			fprintf(stderr, "%s: expected no IE\n", v->name);
			return -1;
		}
		break;
	}
	case FN_GET_IE_EX: {
		unsigned int ielen = 0;
		u8 copy_buf[MAX_IES];
		u8 *out_buf = NULL;

		if (v->copy_out)
			out_buf = copy_buf;
		u8 *ie = rtw_get_ie_ex(v->ies, (unsigned int)v->ies_len, (u8)v->eid,
				       v->oui_len ? v->oui : NULL, (u8)v->oui_len,
				       out_buf, &ielen);

		if (v->expect_found) {
			if (!ie || (int)ielen != v->expect_ielen) {
				fprintf(stderr, "%s: get_ie_ex mismatch\n", v->name);
				return -1;
			}
			if (v->copy_out && (ielen != v->expect_copy_len ||
					    memcmp(copy_buf, v->expect_copy,
						   v->expect_copy_len) != 0)) {
				fprintf(stderr, "%s: get_ie_ex copy-out mismatch\n",
					v->name);
				return -1;
			}
		} else if (ie) {
			fprintf(stderr, "%s: expected no IE ex\n", v->name);
			return -1;
		}
		break;
	}
	case FN_REMOVE_IE: {
		unsigned int ies_len = (unsigned int)v->ies_len;
		memcpy(v->ies_mut, v->ies, v->ies_len);
		int ret = rtw_ies_remove_ie(v->ies_mut, &ies_len, (unsigned int)v->offset,
					    (u8)v->eid, NULL, 0);

		if (ret != v->expect_ret || (int)ies_len != v->expect_len) {
			fprintf(stderr, "%s: remove_ie mismatch ret=%d len=%u\n",
				v->name, ret, ies_len);
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
	const char *path = "ie_vectors.json";
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
	printf("all %zu ie vectors passed (oracle: core/rtw_ieee80211.c)\n", nvec);
	return 0;
}
