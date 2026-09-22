// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_ft_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8

struct vector {
	char name[64];
	char op[16];
	int expect_flags;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "expect_flags", &v->expect_flags);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct ft_roam_info ft;

	memset(&ft, 0, sizeof(ft));
	if (strcmp(v->op, "info_init"))
		return 1;
	host_ft_info_init(&ft);
	if ((int)ft.ft_flags != v->expect_flags || ft.ft_updated_bcn)
		return 1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ft_ie_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
