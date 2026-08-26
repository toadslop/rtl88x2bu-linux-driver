// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_cmd_queue_types.h"
#include "host_vector_json.h"

static int g_sema_up;
struct _adapter g_adapter;

void host_cmd_queue_reset(void) { g_sema_up = 0; }
void host_cmd_queue_set_hw_init(int v) { GET_HAL_DATA(&g_adapter)->hw_init_completed = (u8)v; }
void host_cmd_queue_set_cmdthd_running(int v) { ATOMIC_SET(&g_adapter.cmdpriv.cmdthd_running, v); }
int host_cmd_queue_sema_up_count(void) { return g_sema_up; }
void *rtw_zmalloc(u32 sz) { return calloc(1, sz); }
void rtw_mfree(u8 *p, u32 sz) { (void)sz; free(p); }
void _rtw_up_sema(_sema *s) { (void)s; g_sema_up++; }

struct vector { char name[40]; int fn, arg, expect; };

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;
	char s[16];
	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", s, sizeof(s)))
		return -1;
	v->fn = s[0];
	host_json_parse_int_in(o, l, "arg", &v->arg);
	host_json_parse_int_in(o, l, "expect", &v->expect);
	return 0;
}

static struct cmd_obj *mk(u16 c, u8 io)
{
	struct cmd_obj *o = rtw_zmalloc(sizeof(*o));
	if (!o) return NULL;
	o->cmdcode = c; o->no_io = io; o->cmdsz = 4; o->parmbuf = rtw_zmalloc(4);
	_rtw_init_listhead(&o->list);
	return o;
}

static int qlen(_queue *q)
{
	int n = 0;
	for (_list *p = q->queue.next; p != &q->queue; p = p->next) n++;
	return n;
}

static int run(struct vector *v)
{
	struct cmd_priv *cp = &g_adapter.cmdpriv;
	struct cmd_obj *a, *b, *g;
	host_cmd_queue_reset();
	memset(&g_adapter, 0, sizeof(g_adapter));
	_rtw_init_listhead(&cp->cmd_queue.queue);
	cp->padapter = &g_adapter;
	host_cmd_queue_set_hw_init(_TRUE);
	host_cmd_queue_set_cmdthd_running(_TRUE);
	if (v->fn == 'f') {
		a = mk(1, 0); b = mk(v->arg ? 20 : 2, 0);
		if (!a || !b) goto fail;
		_rtw_enqueue_cmd(&cp->cmd_queue, a, 0);
		_rtw_enqueue_cmd(&cp->cmd_queue, b, v->arg);
		g = _rtw_dequeue_cmd(&cp->cmd_queue);
		if (!g || g->cmdcode != (v->arg ? 20 : 1)) goto fail;
		rtw_free_cmd_obj(g);
		g = _rtw_dequeue_cmd(&cp->cmd_queue);
		if (!g) goto fail;
		rtw_free_cmd_obj(g);
	} else if (v->fn == 'l') {
		host_cmd_queue_set_hw_init(v->arg & 1);
		host_cmd_queue_set_cmdthd_running(v->arg & 2 ? _TRUE : _FALSE);
		a = mk(v->arg & 4 ? CMD_SET_CHANPLAN : 1, v->arg & 8 ? 1 : 0);
		if (!a || rtw_cmd_filter(cp, a) != v->expect) goto fail;
		rtw_free_cmd_obj(a);
	} else if (v->fn == 'e') {
		if (!(v->arg & 1)) { host_cmd_queue_set_hw_init(_FALSE); host_cmd_queue_set_cmdthd_running(_FALSE); }
		a = mk(1, 0);
		if (!a || (int)rtw_enqueue_cmd(cp, a) != v->expect) goto fail;
		if (v->expect == _SUCCESS && (host_cmd_queue_sema_up_count() != 1 || qlen(&cp->cmd_queue) != 1)) goto fail;
		if (v->expect == _SUCCESS) rtw_free_cmd_obj(rtw_dequeue_cmd(cp));
	} else {
		struct evt_priv ep;
		memset(&ep, 0, sizeof(ep));
		_rtw_init_listhead(&ep.evt_queue.queue);
		if (!v->arg) {
			struct evt_obj *eo = rtw_zmalloc(sizeof(*eo));
			if (!eo || (int)rtw_enqueue_evt(&ep, eo) != v->expect || qlen(&ep.evt_queue) != 1) goto fail;
			rtw_free_evt_obj(eo);
		} else if (v->arg == 1) {
			rtw_evt_notify_isr(&ep);
			if (ep.evt_done_cnt != 1 || host_cmd_queue_sema_up_count() != 1) goto fail;
		}
	}
	printf("PASS %s\n", v->name); return 0;
fail: fprintf(stderr, "FAIL %s\n", v->name); return -1;
}

int main(int argc, char **argv)
{
	struct vector v[12]; size_t n = 0; int bad = 0;
	const char *p = argc > 1 ? argv[1] : "cmd_queue_vectors.json";
	if (host_load_vectors(p, v, sizeof(v[0]), 12, parse_vec, &n)) return 2;
	for (size_t i = 0; i < n; i++) bad += run(&v[i]) != 0;
	if (!bad) printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
