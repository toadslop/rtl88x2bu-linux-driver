// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_cmd_priv_types.h"
#include "host_vector_json.h"

enum fn_id { FN_CMD, FN_EVT, FN_CMD_FAIL, FN_EVT_FAIL };

struct vector { char name[64]; enum fn_id fn; int expect; int malloc_fail; };

static int parse_fn(const char *o, size_t l, enum fn_id *id)
{
	char s[32];
	if (host_json_parse_string_in(o, l, "fn", s, sizeof(s)))
		return -1;
	if (!strcmp(s, "_rtw_init_cmd_priv"))
		*id = FN_CMD;
	else if (!strcmp(s, "_rtw_init_evt_priv"))
		*id = FN_EVT;
	else if (!strcmp(s, "cmd_init_malloc_fail"))
		*id = FN_CMD_FAIL;
	else if (!strcmp(s, "evt_init_malloc_fail"))
		*id = FN_EVT_FAIL;
	else
		return -1;
	return 0;
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;
	memset(v, 0, sizeof(*v));
	v->malloc_fail = -1;
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    parse_fn(o, l, &v->fn))
		return -1;
	host_json_parse_int_in(o, l, "expect", &v->expect);
	host_json_parse_int_in(o, l, "malloc_fail_after", &v->malloc_fail);
	return 0;
}

static int aligned(const void *p, size_t a)
{
	return p && (((unsigned long)p & (a - 1)) == 0);
}

static int run_vec(struct vector *v)
{
	struct cmd_priv cp;
	struct evt_priv ep;
	int got;

	host_cmd_priv_set_malloc_fail_after(v->malloc_fail);
	memset(&cp, 0, sizeof(cp));
	memset(&ep, 0, sizeof(ep));
	if (v->fn == FN_CMD || v->fn == FN_CMD_FAIL) {
		got = _rtw_init_cmd_priv(&cp);
		if (got != v->expect)
			goto fail;
		if (got == 1 && v->fn == FN_CMD &&
		    (cp.cmd_seq != 1 || !cp.cmd_buf || !cp.rsp_buf || !aligned(cp.cmd_buf, 512) ||
		     !aligned(cp.rsp_buf, 4)))
			goto fail;
		_rtw_free_cmd_priv(&cp);
	} else if (v->fn == FN_EVT_FAIL) {
		got = _rtw_init_evt_priv(&ep);
		if (got != v->expect || !ep.c2h_queue)
			goto fail;
		_rtw_free_evt_priv(&ep);
	} else {
		got = _rtw_init_evt_priv(&ep);
		if (got != v->expect || (got == 1 && (!ep.evt_buf || !ep.c2h_queue || ep.evt_done_cnt)))
			goto fail;
		_rtw_free_evt_priv(&ep);
	}
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "cmd_priv_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]) != 0;
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
