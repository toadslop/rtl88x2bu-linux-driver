// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rm_fsm_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8

struct vector {
	char name[64];
	char op[24];
	int ms, evid;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "ms", &v->ms);
	host_json_parse_int_in(obj, len, "evid", &v->evid);
	return 0;
}

static int run_vec(struct vector *v)
{
	host_rm_fsm_adapter_init(&g_adapter);

	if (!strcmp(v->op, "is_list_linked")) {
		struct rm_obj obj;
		struct _list standalone;

		memset(&obj, 0, sizeof(obj));
		_rtw_init_listhead(&standalone);
		if (is_list_linked(&obj.list) != 0)
			return 1;
		standalone.prev = &standalone;
		if (is_list_linked(&standalone) != 1)
			return 1;
	} else if (!strcmp(v->op, "set_clock")) {
		struct rm_obj prm;
		struct rm_clock clk;

		memset(&prm, 0, sizeof(prm));
		memset(&clk, 0, sizeof(clk));
		prm.pclock = &clk;
		rm_set_clock(&prm, (u32)v->ms, (enum RM_EV_ID)v->evid);
		if (ATOMIC_READ(&clk.counter) != (int)(v->ms / CLOCK_UNIT))
			return 1;
		if (clk.evid != (enum RM_EV_ID)v->evid)
			return 1;
	} else if (!strcmp(v->op, "cancel_clock")) {
		struct rm_obj prm;
		struct rm_clock clk;

		memset(&prm, 0, sizeof(prm));
		clk.counter = 99;
		clk.evid = RM_EV_meas_timer_expire;
		prm.pclock = &clk;
		rm_cancel_clock(&prm);
		if (ATOMIC_READ(&clk.counter) != 0 || clk.evid != RM_EV_max)
			return 1;
	} else if (!strcmp(v->op, "alloc_clock")) {
		struct rm_obj prm;

		memset(&prm, 0, sizeof(prm));
		if (!rm_alloc_clock(&g_adapter, &prm))
			return 1;
		if (g_adapter.rmpriv.clock[0].prm != &prm)
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
	const char *path = argc > 1 ? argv[1] : "rm_fsm_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
