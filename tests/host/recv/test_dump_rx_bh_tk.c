// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-171 dump_rx_bh_tk. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_recv_bh_dbg_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 512

struct vector {
	char name[MAX_NAME];
	u32 rx_bh_stage;
	u32 rx_bh_buf_dq_cnt;
	unsigned long lbuf;
	unsigned long cbuf;
	u32 rx_bh_cbuf_dlen;
	u32 rx_bh_cbuf_pos;
	unsigned long cframe;
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

static int parse_u32_field(const char *obj, size_t len, const char *key, u32 *out)
{
	int v;

	if (host_json_parse_int_in(obj, len, key, &v))
		return -1;
	*out = (u32)v;
	return 0;
}

static int parse_ptr_hex_field(const char *obj, size_t len, const char *key,
			       unsigned long *out)
{
	char hex[32];

	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return -1;
	*out = strtoul(hex, NULL, 16);
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u32_field(obj, len, "stage", &v->rx_bh_stage))
		return -1;
	if (parse_u32_field(obj, len, "buf_dq_cnt", &v->rx_bh_buf_dq_cnt))
		return -1;
	if (parse_ptr_hex_field(obj, len, "lbuf", &v->lbuf))
		return -1;
	if (parse_ptr_hex_field(obj, len, "cbuf", &v->cbuf))
		return -1;
	if (parse_u32_field(obj, len, "dlen", &v->rx_bh_cbuf_dlen))
		return -1;
	if (parse_u32_field(obj, len, "pos", &v->rx_bh_cbuf_pos))
		return -1;
	if (parse_ptr_hex_field(obj, len, "cframe", &v->cframe))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct recv_priv recv;
	char got[MAX_EXPECT];
	FILE *f;
	size_t nread;

	memset(&recv, 0, sizeof(recv));
	recv.rx_bh_stage = v->rx_bh_stage;
	recv.rx_bh_buf_dq_cnt = v->rx_bh_buf_dq_cnt;
	recv.rx_bh_lbuf = (void *)v->lbuf;
	recv.rx_bh_cbuf = (void *)v->cbuf;
	recv.rx_bh_cbuf_dlen = v->rx_bh_cbuf_dlen;
	recv.rx_bh_cbuf_pos = v->rx_bh_cbuf_pos;
	recv.rx_bh_cframe = (void *)v->cframe;

	f = tmpfile();
	if (!f) {
		fprintf(stderr, "tmpfile failed\n");
		return -1;
	}
	dump_rx_bh_tk(f, &recv);
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
	const char *path = argc > 1 ? argv[1] : "debug_rx_bh_tk_vectors.json";
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
	return 0;
}
