// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-212 proc TDLS enable + function info. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_tdls_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 2048

struct vector {
	char name[MAX_NAME];
	u8 en_tdls;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char get_field[MAX_FIELD];
	char expect[MAX_EXPECT];
	struct tdls_info tdlsinfo;
	int has_tdlsinfo;
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

static const char *json_object_end(const char *start)
{
	int depth = 0;
	const char *p = start;

	if (*p != '{')
		return NULL;
	for (; *p; p++) {
		if (*p == '{')
			depth++;
		else if (*p == '}') {
			depth--;
			if (depth == 0)
				return p + 1;
		}
	}
	return NULL;
}

static int parse_tdlsinfo_object(const char *obj, size_t len, struct tdls_info *info)
{
	const char *start = host_json_find_key_in(obj, len, "tdlsinfo");
	const char *end;
	size_t sub_len;

	if (!start || *start != '{')
		return -1;
	end = json_object_end(start);
	if (!end)
		return -1;
	sub_len = (size_t)(end - start);

	memset(info, 0, sizeof(*info));
	if (parse_u8_field(start, sub_len, "driver_setup", &info->driver_setup))
		return -1;
	if (parse_u8_field(start, sub_len, "ap_prohibited", &info->ap_prohibited))
		return -1;
	if (parse_u8_field(start, sub_len, "ch_switch_prohibited",
			    &info->ch_switch_prohibited))
		return -1;
	if (parse_u8_field(start, sub_len, "link_established", &info->link_established))
		return -1;
	if (parse_u8_field(start, sub_len, "sta_cnt", &info->sta_cnt))
		return -1;
	if (parse_u8_field(start, sub_len, "sta_maximum", &info->sta_maximum))
		return -1;
	if (parse_u8_field(start, sub_len, "dev_discovered", &info->dev_discovered))
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "en_tdls", &v->en_tdls))
		return -1;
	if (host_json_find_key_in(obj, len, "tdlsinfo")) {
		if (parse_tdlsinfo_object(obj, len, &v->tdlsinfo))
			return -1;
		v->has_tdlsinfo = 1;
	}
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
	adapter.registrypriv.en_tdls = v->en_tdls;
	if (v->has_tdlsinfo)
		adapter.tdlsinfo = v->tdlsinfo;
	dev.priv = &adapter;
	seq.private = &dev;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (v->set_field[0]) {
		if (!strcmp(v->set_field, "tdls_enable")) {
			if (proc_set_tdls_enable(NULL, v->set_input,
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

	if (!strcmp(v->get_field, "tdls_enable"))
		proc_get_tdls_enable(&seq, NULL);
	else if (!strcmp(v->get_field, "tdlsinfo"))
		proc_tdls_display_tdls_function_info(&seq);
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
	const char *path = argc > 1 ? argv[1] : "debug_proc_tdls_vectors.json";
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

	printf("ok: %zu proc TDLS vectors\n", count);
	return 0;
}
