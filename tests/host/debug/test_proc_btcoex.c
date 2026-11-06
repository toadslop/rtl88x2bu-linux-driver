// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-215 proc btcoex debug handlers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_btcoex_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 64
#define MAX_FIELD 32
#define MAX_EXPECT 128

struct vector {
	char name[MAX_NAME];
	u32 dbg0;
	u32 dbg1;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char get_field[MAX_FIELD];
	char expect[MAX_EXPECT];
	int set_expect_fail;
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
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u32)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u32_field(obj, len, "dbg0", &v->dbg0))
		return -1;
	if (parse_u32_field(obj, len, "dbg1", &v->dbg1))
		return -1;
	if (host_json_find_key_in(obj, len, "set")) {
		if (host_json_parse_string_in(obj, len, "set", v->set_field,
					      sizeof(v->set_field)))
			return -1;
		if (host_json_parse_string_in(obj, len, "set_input", v->set_input,
					      sizeof(v->set_input)))
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "get", v->get_field,
				      sizeof(v->get_field)))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	if (host_json_find_key_in(obj, len, "set_expect_fail")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "set_expect_fail", &tmp))
			return -1;
		v->set_expect_fail = tmp;
	}
	unescape_expect(v->expect);
	return 0;
}

static int dispatch_get(const struct vector *v, struct seq_file *seq)
{
	if (!strcmp(v->get_field, "btcoex_dbg"))
		return proc_get_btcoex_dbg(seq, NULL);
	if (!strcmp(v->get_field, "btcoex_info"))
		return proc_get_btcoex_info(seq, NULL);
	return 1;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;

	host_debug_proc_btcoex_reset_mocks();
	host_debug_proc_btcoex_set_dbg(v->dbg0, v->dbg1);

	memset(&adapter, 0, sizeof(adapter));
	dev.priv = &adapter;
	seq.private = &dev;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (v->set_field[0]) {
		ssize_t sr = proc_set_btcoex_dbg(NULL, v->set_input,
						 strlen(v->set_input), NULL,
						 &dev);
		if (v->set_expect_fail) {
			if (sr >= 0) {
				fclose(capture);
				return -1;
			}
		} else if (sr < 0) {
			fclose(capture);
			return -1;
		}
	}

	if (dispatch_get(v, &seq)) {
		fclose(capture);
		return -1;
	}

	rewind(capture);
	nread = fread(got, 1, sizeof(got) - 1, capture);
	fclose(capture);
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
	const char *path = argc > 1 ? argv[1] : "debug_proc_btcoex_vectors.json";
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

	printf("ok: %zu proc btcoex vectors\n", count);
	return 0;
}
