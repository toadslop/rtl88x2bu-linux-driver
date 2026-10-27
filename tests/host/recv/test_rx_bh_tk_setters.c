// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-176 rx_bh_tk_set_* → dump_rx_bh_tk. */

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
	int has_stage;
	u32 stage;
	int has_buf;
	unsigned long buf;
	unsigned long data;
	u32 dlen;
	int has_buf2;
	unsigned long buf2;
	unsigned long data2;
	u32 dlen2;
	int has_pos;
	unsigned long pos;
	int has_frame;
	unsigned long frame;
	int clear_buf;
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

static int parse_optional_u32(const char *obj, size_t len, const char *key, u32 *out,
			      int *has)
{
	*has = 0;
	if (host_json_parse_int_in(obj, len, key, (int *)out) == 0) {
		*has = 1;
		return 0;
	}
	return 0;
}

static int parse_optional_ptr(const char *obj, size_t len, const char *key,
			      unsigned long *out, int *has)
{
	char hex[32];

	*has = 0;
	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return 0;
	*out = strtoul(hex, NULL, 16);
	*has = 1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int dummy;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	parse_optional_u32(obj, len, "stage", &v->stage, &v->has_stage);
	parse_optional_ptr(obj, len, "buf", &v->buf, &v->has_buf);
	if (v->has_buf) {
		if (parse_ptr_hex_field(obj, len, "data", &v->data))
			return -1;
		if (parse_u32_field(obj, len, "dlen", &v->dlen))
			return -1;
	}
	parse_optional_ptr(obj, len, "buf2", &v->buf2, &v->has_buf2);
	if (v->has_buf2) {
		if (parse_ptr_hex_field(obj, len, "data2", &v->data2))
			return -1;
		if (parse_u32_field(obj, len, "dlen2", &v->dlen2))
			return -1;
	}
	parse_optional_ptr(obj, len, "pos", &v->pos, &v->has_pos);
	parse_optional_ptr(obj, len, "frame", &v->frame, &v->has_frame);
	if (!host_json_parse_int_in(obj, len, "clear_buf", &dummy))
		v->clear_buf = dummy;
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
	if (v->has_stage)
		rx_bh_tk_set_stage(&recv, v->stage);
	if (v->has_buf)
		rx_bh_tk_set_buf(&recv, (void *)v->buf, (void *)v->data, v->dlen);
	if (v->has_buf2)
		rx_bh_tk_set_buf(&recv, (void *)v->buf2, (void *)v->data2, v->dlen2);
	if (v->clear_buf)
		rx_bh_tk_set_buf(&recv, NULL, NULL, 0);
	if (v->has_pos)
		rx_bh_tk_set_buf_pos(&recv, (void *)v->pos);
	if (v->has_frame)
		rx_bh_tk_set_frame(&recv, (void *)v->frame);

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
	const char *path = argc > 1 ? argv[1] : "rx_bh_tk_setters_vectors.json";
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
