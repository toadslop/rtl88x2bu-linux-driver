// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_mlme_wmm_rsn_types.h"
#include "host_vector_json.h"

struct vector {
	char name[128]; u8 in[256], out[256]; size_t in_len, out_off, expect_len;
	char expect_hex[256];
};

int rtw_restruct_wmm_ie(_adapter *a, u8 *in, u8 *out, unsigned n, unsigned off);

static int parse_vector_object(const char *obj, size_t len, void *v)
{
	struct vector *vec = v; char hex[512]; size_t n = 0; int t = 0;
	memset(vec, 0, sizeof(*vec));
	if (host_json_parse_string_in(obj, len, "name", vec->name, sizeof(vec->name)) ||
	    host_json_parse_string_in(obj, len, "in_ie_hex", hex, sizeof(hex)) ||
	    host_hex_decode(hex, vec->in, sizeof(vec->in), &n))
		return -1;
	host_json_parse_int_in(obj, len, "in_len", &t); vec->in_len = t;
	host_json_parse_int_in(obj, len, "initial_out_len", &t); vec->out_off = t;
	host_json_parse_int_in(obj, len, "expect_len", &t); vec->expect_len = t;
	host_json_parse_string_in(obj, len, "expect_out_hex", vec->expect_hex,
				  sizeof(vec->expect_hex));
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8]; size_t n = 0; _adapter ad = {0}; u8 expect[256]; size_t en = 0;
	if (argc != 2 || host_load_vectors(argv[1], vecs, sizeof(vecs[0]), 8, parse_vector_object, &n))
		return 1;
	for (size_t i = 0; i < n; i++) {
		unsigned got = rtw_restruct_wmm_ie(&ad, vecs[i].in, vecs[i].out,
						   (unsigned)vecs[i].in_len,
						   (unsigned)vecs[i].out_off);
		if (got != vecs[i].expect_len || host_hex_decode(vecs[i].expect_hex,
								 expect, sizeof(expect), &en) ||
		    en != got || memcmp(vecs[i].out, expect, en))
			return fprintf(stderr, "%s: FAIL\n", vecs[i].name), 1;
	}
	printf("PASS %zu vectors (oracle: core/rtw_mlme_rest.c) (%s)\n", n, argv[1]);
	return 0;
}
