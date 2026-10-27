// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_pwrctrl_alive.h"
#include "host_vector_json.h"

#define _SUCCESS 1
#define _FAIL 0
#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_FN 8

typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;


struct vector {
	char name[MAX_NAME];
	char fn[MAX_FN];
	int alive_tag;
	int bFwCurrentInPSMode;
	int cpwm;
	int rpwm;
	int set_rpwm_raises_cpwm;
	int expect_success;
	int has_expect_success;
	int expect_alives;
	int has_expect_alives;
	int expect_set_rpwm_calls;
	int has_expect_set_rpwm_calls;
	int expect_last_set_rpwm;
	int has_expect_last_set_rpwm;
};

static struct adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(obj, len, "alive_tag", &v->alive_tag);
	host_json_parse_int_in(obj, len, "bFwCurrentInPSMode", &v->bFwCurrentInPSMode);
	host_json_parse_int_in(obj, len, "cpwm", &v->cpwm);
	host_json_parse_int_in(obj, len, "rpwm", &v->rpwm);
	host_json_parse_int_in(obj, len, "set_rpwm_raises_cpwm", &v->set_rpwm_raises_cpwm);
	if (host_json_parse_int_in(obj, len, "expect_success", &v->expect_success) == 0)
		v->has_expect_success = 1;
	if (host_json_parse_int_in(obj, len, "expect_alives", &v->expect_alives) == 0)
		v->has_expect_alives = 1;
	if (host_json_parse_int_in(obj, len, "expect_set_rpwm_calls",
				   &v->expect_set_rpwm_calls) == 0)
		v->has_expect_set_rpwm_calls = 1;
	if (host_json_parse_int_in(obj, len, "expect_last_set_rpwm",
				   &v->expect_last_set_rpwm) == 0)
		v->has_expect_last_set_rpwm = 1;
	return 0;
}

static s32 invoke(struct vector *v)
{
	if (!strcmp(v->fn, "task"))
		return rtw_register_task_alive(&g_adapter, (u32)v->alive_tag);
	if (!strcmp(v->fn, "tx"))
		return rtw_register_tx_alive(&g_adapter);
	if (!strcmp(v->fn, "cmd"))
		return rtw_register_cmd_alive(&g_adapter);
	if (!strcmp(v->fn, "rx"))
		return rtw_register_rx_alive(&g_adapter);
	if (!strcmp(v->fn, "evt"))
		return rtw_register_evt_alive(&g_adapter);
	fprintf(stderr, "%s: unknown fn %s\n", v->name, v->fn);
	return -1;
}

static int run_vec(struct vector *v)
{
	struct host_pwrctrl_alive_trace *tr;
	s32 ret;

	host_pwrctrl_alive_reset_trace();
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.pwrctrlpriv.bFwCurrentInPSMode = (u8)v->bFwCurrentInPSMode;
	g_adapter.pwrctrlpriv.cpwm = (u8)v->cpwm;
	g_adapter.pwrctrlpriv.rpwm = (u8)v->rpwm;
	host_pwrctrl_alive_set_rpwm_raises_cpwm((u8)v->set_rpwm_raises_cpwm);

	ret = invoke(v);
	tr = host_pwrctrl_alive_get_trace();

	if (v->has_expect_success) {
		u8 expect = v->expect_success ? _SUCCESS : _FAIL;

		if ((u8)ret != expect) {
			fprintf(stderr, "%s: ret %d expect %u\n", v->name, ret, expect);
			return -1;
		}
	}
	if (v->has_expect_alives &&
	    g_adapter.pwrctrlpriv.alives != (u32)v->expect_alives) {
		fprintf(stderr, "%s: alives 0x%x expect 0x%x\n", v->name,
			g_adapter.pwrctrlpriv.alives, (unsigned)v->expect_alives);
		return -1;
	}
	if (v->has_expect_set_rpwm_calls &&
	    (int)tr->set_rpwm_calls != v->expect_set_rpwm_calls) {
		fprintf(stderr, "%s: set_rpwm calls %u expect %d\n", v->name,
			tr->set_rpwm_calls, v->expect_set_rpwm_calls);
		return -1;
	}
	if (v->has_expect_last_set_rpwm &&
	    (int)tr->last_set_rpwm_pslv != v->expect_last_set_rpwm) {
		fprintf(stderr, "%s: last pslv %u expect %d\n", v->name,
			tr->last_set_rpwm_pslv, v->expect_last_set_rpwm);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "register_alive_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
