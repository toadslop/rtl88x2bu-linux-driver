// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-170 regsty RX AMPDU limit dump. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_regsty_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 512

struct vector {
	char name[MAX_NAME];
	u8 table[16];
	char expect[MAX_EXPECT];
};

static void unescape_expect(char *s)
{
	char *r = s;
	char *w = s;

	while (*r) {
		if (r[0] == '\\' && r[1] == 'n') {
			*w++ = '\n';
			r += 2;
		} else {
			*w++ = *r++;
		}
	}
	*w = '\0';
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[64];
	size_t hex_len;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "limits_hex", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->table, sizeof(v->table), &hex_len) ||
	    hex_len != sizeof(v->table))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	char got[MAX_EXPECT];
	FILE *f;
	size_t nread;

	memset(&adapter, 0, sizeof(adapter));
	memcpy(adapter.registrypriv.rx_ampdu_sz_limit_by_nss_bw, v->table,
	       sizeof(v->table));

	f = tmpfile();
	if (!f) {
		fprintf(stderr, "tmpfile failed\n");
		return -1;
	}
	dump_regsty_rx_ampdu_size_limit(f, &adapter);
	rewind(f);
	nread = fread(got, 1, sizeof(got) - 1, f);
	fclose(f);
	got[nread] = '\0';

	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n", v->name,
			got, v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "debug_regsty_rx_ampdu_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			return 1;
	}

	printf("ok: %zu regsty rx_ampdu vectors\n", count);
	return 0;
}
