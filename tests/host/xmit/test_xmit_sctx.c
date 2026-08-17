// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_xmit_sctx_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

struct vector {
	char name[MAX_NAME];
	char fn[64];
	int status, expect_u8, expect_ret, pre_complete, timeout_ms;
};

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, obj_len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(obj, obj_len, "status", &v->status);
	host_json_parse_int_in(obj, obj_len, "expect_u8", &v->expect_u8);
	host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, obj_len, "pre_complete", &v->pre_complete);
	host_json_parse_int_in(obj, obj_len, "timeout_ms", &v->timeout_ms);
	return 0;
}

static int run_vector(struct vector *v)
{
	if (!strcmp(v->fn, "rtw_sctx_chk_waring_status")) {
		if (rtw_sctx_chk_waring_status(v->status) != (bool)v->expect_u8)
			goto fail;
	} else if (!strcmp(v->fn, "rtw_sctx_init_wait")) {
		struct submit_ctx sctx;
		rtw_sctx_init(&sctx, v->timeout_ms);
		if (v->pre_complete) {
			sctx.status = RTW_SCTX_DONE_SUCCESS;
			complete(&sctx.done);
		}
		if (rtw_sctx_wait(&sctx, "test") != v->expect_ret)
			goto fail;
	} else if (!strcmp(v->fn, "rtw_sctx_done_err")) {
		struct submit_ctx sctx;
		struct submit_ctx *ps = &sctx;
		rtw_sctx_init(&sctx, 0);
		rtw_sctx_done_err(&ps, v->status);
		if (ps != NULL || sctx.status != v->status)
			goto fail;
	} else if (!strcmp(v->fn, "rtw_sctx_done")) {
		struct submit_ctx sctx;
		struct submit_ctx *ps = &sctx;
		rtw_sctx_init(&sctx, 0);
		rtw_sctx_done(&ps);
		if (ps != NULL || sctx.status != RTW_SCTX_DONE_SUCCESS)
			goto fail;
	} else {
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	int failures = 0;

	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count))
		return 2;
	for (i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			failures++;
	printf("%zu vectors, %d failures\n", count, failures);
	return failures ? 1 : 0;
}
