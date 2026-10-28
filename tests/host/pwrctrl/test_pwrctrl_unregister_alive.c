// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_pwrctrl_alive.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_FN 12

typedef unsigned char u8;
typedef unsigned int u32;

struct vector {
	char name[MAX_NAME];
	char fn[MAX_FN];
	int alive_tag;
	int initial_alives;
	int pwr_mode;
	int bFwCurrentInPSMode;
	int cpwm;
	int rpwm;
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
	host_json_parse_int_in(obj, len, "initial_alives", &v->initial_alives);
	host_json_parse_int_in(obj, len, "pwr_mode", &v->pwr_mode);
	host_json_parse_int_in(obj, len, "bFwCurrentInPSMode", &v->bFwCurrentInPSMode);
	host_json_parse_int_in(obj, len, "cpwm", &v->cpwm);
	host_json_parse_int_in(obj, len, "rpwm", &v->rpwm);
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

static void invoke(struct vector *v)
{
	if (!strcmp(v->fn, "un_task"))
		rtw_unregister_task_alive(&g_adapter, (u32)v->alive_tag);
	else if (!strcmp(v->fn, "un_tx"))
		rtw_unregister_tx_alive(&g_adapter);
	else if (!strcmp(v->fn, "un_cmd"))
		rtw_unregister_cmd_alive(&g_adapter);
	else if (!strcmp(v->fn, "un_rx"))
		rtw_unregister_rx_alive(&g_adapter);
	else if (!strcmp(v->fn, "un_evt"))
		rtw_unregister_evt_alive(&g_adapter);
	else
		fprintf(stderr, "%s: unknown fn %s\n", v->name, v->fn);
}

static int run_vec(struct vector *v)
{
	struct host_pwrctrl_alive_trace *tr;

	host_pwrctrl_alive_reset_trace();
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.pwrctrlpriv.alives = (u32)v->initial_alives;
	g_adapter.pwrctrlpriv.pwr_mode = (u8)v->pwr_mode;
	g_adapter.pwrctrlpriv.bFwCurrentInPSMode = (u8)v->bFwCurrentInPSMode;
	g_adapter.pwrctrlpriv.cpwm = (u8)v->cpwm;
	g_adapter.pwrctrlpriv.rpwm = (u8)v->rpwm;

	invoke(v);
	tr = host_pwrctrl_alive_get_trace();

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
	const char *path = argc > 1 ? argv[1] : "unregister_alive_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
