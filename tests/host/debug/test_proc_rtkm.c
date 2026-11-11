// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"
#include "host_rtw_debug_proc_rtkm_types.h"

struct vector {
	char name[48];
	char expect_contains[56];
	char expect_contains2[56];
	u32 alloc_sz;
};

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "expect_contains", v->expect_contains,
				  sizeof(v->expect_contains));
	host_json_parse_string_in(obj, len, "expect_contains2", v->expect_contains2,
				  sizeof(v->expect_contains2));
	if (host_json_parse_int_in(obj, len, "alloc_sz", &t) == 0)
		v->alloc_sz = (u32)t;
	return 0;
}

static int run_one(const struct vector *v)
{
	struct _adapter a;
	struct recv_buf rb;
	struct net_device d;
	struct seq_file s;
	char *cap = NULL;
	size_t cap_len = 0;
	FILE *mem;

	memset(&a, 0, sizeof(a));
	memset(&d, 0, sizeof(d));
	memset(&rb, 0, sizeof(rb));
	rb.alloc_sz = v->alloc_sz ? v->alloc_sz : HOST_RTKM_RECVBUF_SZ;
	a.recvpriv.precv_buf = (u8 *)&rb;

	d.priv = &a;
	s.private = &d;
	mem = open_memstream(&cap, &cap_len);
	if (!mem)
		return -1;
	s.out = mem;

	proc_get_rtkm_info(&s, NULL);
	fclose(mem);

	if (v->expect_contains[0] && (!cap || !strstr(cap, v->expect_contains)))
		goto fail;
	if (v->expect_contains2[0] && (!cap || !strstr(cap, v->expect_contains2)))
		goto fail;
	free(cap);
	return 0;

fail:
	free(cap);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vecs[4];
	size_t n = 0;
	size_t i;

	if (host_load_vectors(
		    argc > 1 ? argv[1] : "debug_proc_rtkm_vectors.json", vecs,
		    sizeof(vecs[0]), 4, parse_vector, &n))
		return 1;
	for (i = 0; i < n; i++) {
		if (run_one(&vecs[i])) {
			fprintf(stderr, "fail: %s\n", vecs[i].name);
			return 1;
		}
	}
	printf("ok: %zu rtkm proc vectors\n", n);
	return 0;
}
