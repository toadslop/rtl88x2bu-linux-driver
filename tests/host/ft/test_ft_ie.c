// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_ft_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_FRAME 512

struct vector {
	char name[64];
	char op[24];
	char updated_ies_hex[512];
	int expect_u8;
	int expect_flags;
	int expect_pktlen;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "updated_ies_hex", v->updated_ies_hex,
				  sizeof(v->updated_ies_hex));
	host_json_parse_int_in(obj, len, "expect_u8", &v->expect_u8);
	host_json_parse_int_in(obj, len, "expect_flags", &v->expect_flags);
	host_json_parse_int_in(obj, len, "expect_pktlen", &v->expect_pktlen);
	return 0;
}

static int load_ies(_adapter *a, struct vector *v, size_t *ies_len)
{
	if (!v->updated_ies_hex[0]) {
		*ies_len = 0;
		return 0;
	}
	if (host_hex_decode(v->updated_ies_hex, a->mlmepriv.ft_roam.updated_ft_ies,
			    RTW_FT_MAX_IE_SZ, ies_len))
		return 1;
	a->mlmepriv.ft_roam.updated_ft_ies_len = (u16)*ies_len;
	return 0;
}

static int run_vec(struct vector *v)
{
	_adapter adapter;
	struct ft_roam_info ft;
	u8 frame[MAX_FRAME];
	u8 *pframe = frame;
	struct pkt_attrib attrib;
	size_t ies_len = 0;
	u8 ret;

	memset(&adapter, 0, sizeof(adapter));
	memset(&ft, 0, sizeof(ft));
	memset(&attrib, 0, sizeof(attrib));

	if (!strcmp(v->op, "info_init")) {
		host_ft_info_init(&ft);
		if ((int)ft.ft_flags != v->expect_flags || ft.ft_updated_bcn)
			return 1;
	} else if (!strcmp(v->op, "update_rsnie_peek") ||
		   !strcmp(v->op, "update_rsnie_write")) {
		if (load_ies(&adapter, v, &ies_len))
			return 1;
		ret = host_ft_update_rsnie(&adapter, (u8)(strcmp(v->op, "update_rsnie_write") == 0),
					   &attrib, &pframe);
		if (ret != (u8)v->expect_u8)
			return 1;
		if (!strcmp(v->op, "update_rsnie_write") &&
		    (int)attrib.pktlen != v->expect_pktlen)
			return 1;
	} else {
		return 1;
	}
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
