// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_mem_premem_types.h"
#include "host_vector_json.h"

#define MAX_V 16

struct vector {
	char name[48];
	char op[12];
	int index, tag, in_size, seed_count;
	int exp_u16, exp_u8, exp_tag, exp_ret, exp_qlen;
};

static int pi(const char *o, size_t l, const char *k, int *v)
{
	return host_json_parse_int_in(o, l, k, v) ? 0 : *v;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	pi(obj, len, "index", &v->index);
	pi(obj, len, "tag", &v->tag);
	pi(obj, len, "in_size", &v->in_size);
	pi(obj, len, "seed_count", &v->seed_count);
	pi(obj, len, "expect_u16", &v->exp_u16);
	pi(obj, len, "expect_u8", &v->exp_u8);
	pi(obj, len, "expect_tag", &v->exp_tag);
	pi(obj, len, "expect_ret", &v->exp_ret);
	pi(obj, len, "expect_qlen", &v->exp_qlen);
	return 0;
}

static int run_vec(struct vector *v)
{
	if (!strcmp(v->op, "reset"))
		host_mem_premem_reset();
	else if (!strcmp(v->op, "constants")) {
		if (rtw_rtkm_get_buff_size() != (u16)v->exp_u16 ||
		    rtw_rtkm_get_nr_recv_skb() != (u8)v->exp_u8)
			goto bad;
	} else if (!strcmp(v->op, "set_buf"))
		host_mem_premem_set_buf(v->index, (u8)v->tag);
	else if (!strcmp(v->op, "get_buf")) {
		u8 *p = rtw_get_buf_premem(v->index);
		if ((p ? *p : -1) != v->exp_tag)
			goto bad;
	} else if (!strcmp(v->op, "seed")) {
		for (int i = 0; i < v->seed_count; i++)
			host_mem_premem_seed_skb(v->tag + i);
	} else if (!strcmp(v->op, "alloc")) {
		struct host_skb *skb = rtw_alloc_skb_premem((u16)v->in_size);
		if ((skb ? skb->tag : -1) != v->exp_tag)
			goto bad;
	} else if (!strcmp(v->op, "free")) {
		struct host_skb fake = { .tag = v->tag };
		if (rtw_free_skb_premem(v->tag < 0 ? NULL : &fake) != v->exp_ret)
			goto bad;
	} else
		goto bad;

	if (v->exp_qlen >= 0 && host_mem_premem_queue_len() != v->exp_qlen)
		goto bad;
	printf("PASS %s\n", v->name);
	return 0;
bad:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_V];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "mem_premem_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_V, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad |= run_vec(&vecs[i]);
	return bad ? 1 : 0;
}
