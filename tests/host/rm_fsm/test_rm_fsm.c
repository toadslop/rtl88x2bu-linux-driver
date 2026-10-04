// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rm_fsm_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16

struct vector {
	char name[64];
	char op[24];
	int ms, evid, to_head;
};

static _adapter g_adapter;

static int qlen(_queue *q)
{
	int n = 0;

	for (_list *p = q->queue.next; p != &q->queue; p = p->next)
		n++;
	return n;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "ms", &v->ms);
	host_json_parse_int_in(obj, len, "evid", &v->evid);
	host_json_parse_int_in(obj, len, "to_head", &v->to_head);
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
		if (ATOMIC_READ(&clk.counter) != (int)(v->ms / CLOCK_UNIT) ||
		    clk.evid != (enum RM_EV_ID)v->evid)
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
		if (!rm_alloc_clock(&g_adapter, &prm) ||
		    g_adapter.rmpriv.clock[0].prm != &prm)
			return 1;
	} else if (!strcmp(v->op, "enqueue_ev")) {
		struct rm_event ev;

		if (rm_enqueue_ev(&g_adapter.rmpriv.ev_queue, NULL, 0) != _FAIL)
			return 1;
		memset(&ev, 0, sizeof(ev));
		_rtw_init_listhead(&ev.list);
		if (rm_enqueue_ev(&g_adapter.rmpriv.ev_queue, &ev, v->to_head) !=
		    _SUCCESS)
			return 1;
		if (qlen(&g_adapter.rmpriv.ev_queue) != 1)
			return 1;
	} else if (!strcmp(v->op, "alloc_rmobj")) {
		struct rm_obj *a = rm_alloc_rmobj(&g_adapter);
		struct rm_obj *b = rm_alloc_rmobj(&g_adapter);

		if (!a || !b || !a->pclock || !b->pclock || a->pclock == b->pclock)
			return 1;
		rm_free_rmobj(b);
		rm_free_rmobj(a);
	} else if (!strcmp(v->op, "free_rmobj")) {
		struct rm_obj *o = rm_alloc_rmobj(&g_adapter);

		if (!o)
			return 1;
		o->q.pssid = rtw_malloc(5);
		strcpy((char *)o->q.pssid, "ssid");
		o->q.opt.bcn.req_start = rtw_malloc(3);
		o->q.opt.bcn.req_len = 3;
		rm_enqueue_rmobj(&g_adapter, o, 0);
		rm_free_rmobj(o);
		if (qlen(&g_adapter.rmpriv.rm_queue) != 0)
			return 1;
	} else if (!strcmp(v->op, "enqueue_rmobj")) {
		struct rm_obj *o = rm_alloc_rmobj(&g_adapter);

		if (!o || rm_enqueue_rmobj(&g_adapter, o, v->to_head) != _SUCCESS ||
		    o->state != RM_ST_IDLE)
			return 1;
		rm_free_rmobj(o);
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
