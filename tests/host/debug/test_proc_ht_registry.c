// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-203 proc HT registry get/set. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_ht_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 32

struct vector {
	char name[MAX_NAME];
	u8 ht_enable;
	u8 bw_mode;
	u8 ampdu_enable;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char get_field[MAX_FIELD];
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

static int parse_u8_field(const char *obj, size_t len, const char *key, u8 *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u8)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "ht_enable", &v->ht_enable))
		return -1;
	if (parse_u8_field(obj, len, "bw_mode", &v->bw_mode))
		return -1;
	if (parse_u8_field(obj, len, "ampdu_enable", &v->ampdu_enable))
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
	unescape_expect(v->expect);
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;

	memset(&adapter, 0, sizeof(adapter));
	adapter.registrypriv.ht_enable = v->ht_enable;
	adapter.registrypriv.bw_mode = v->bw_mode;
	adapter.registrypriv.ampdu_enable = v->ampdu_enable;
	dev.priv = &adapter;
	seq.private = &dev;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (v->set_field[0]) {
		if (!strcmp(v->set_field, "ht_enable")) {
			if (proc_set_ht_enable(NULL, v->set_input, strlen(v->set_input),
					       NULL, &dev) < 0) {
				fclose(capture);
				return -1;
			}
		} else if (!strcmp(v->set_field, "bw_mode")) {
			if (proc_set_bw_mode(NULL, v->set_input, strlen(v->set_input),
					     NULL, &dev) < 0) {
				fclose(capture);
				return -1;
			}
		} else if (!strcmp(v->set_field, "ampdu_enable")) {
			if (proc_set_ampdu_enable(NULL, v->set_input,
						  strlen(v->set_input), NULL,
						  &dev) < 0) {
				fclose(capture);
				return -1;
			}
		} else {
			fclose(capture);
			return -1;
		}
	}

	if (!strcmp(v->get_field, "ht_enable"))
		proc_get_ht_enable(&seq, NULL);
	else if (!strcmp(v->get_field, "bw_mode"))
		proc_get_bw_mode(&seq, NULL);
	else if (!strcmp(v->get_field, "ampdu_enable"))
		proc_get_ampdu_enable(&seq, NULL);
	else {
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
	const char *path = argc > 1 ? argv[1] : "debug_proc_ht_vectors.json";
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

	printf("ok: %zu proc ht registry vectors\n", count);
	return 0;
}
