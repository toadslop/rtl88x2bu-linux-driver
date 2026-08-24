// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include "host_cmd_priv_types.h"

static int g_fail = -1, g_n;

void host_cmd_priv_set_malloc_fail_after(int n) { g_fail = n; g_n = 0; }
void *rtw_zmalloc(u32 sz)
{
	if (g_fail >= 0 && g_n++ >= g_fail)
		return NULL;
	return calloc(1, sz);
}
void rtw_mfree(u8 *p, u32 sz) { (void)sz; free(p); }
void _rtw_init_sema(_sema *s, int v) { (void)s; (void)v; }
void _rtw_free_sema(_sema *s) { (void)s; }
void _rtw_mutex_init(_mutex *m) { (void)m; }
void _rtw_mutex_free(_mutex *m) { (void)m; }
void _init_workitem(_workitem *w, void *f, void *c) { (void)w; (void)f; (void)c; }
void _cancel_workitem_sync(_workitem *w) { (void)w; }
void rtw_msleep_os(int ms) { (void)ms; }
struct rtw_cbuf *rtw_cbuf_alloc(u32 n)
{
	struct rtw_cbuf *c = calloc(1, sizeof(*c));
	if (!c || !(c->bufs = calloc(n, sizeof(void *)))) {
		free(c);
		return NULL;
	}
	c->size = n;
	return c;
}
void rtw_cbuf_free(struct rtw_cbuf *c) { free(c ? c->bufs : NULL); free(c); }
bool rtw_cbuf_empty(struct rtw_cbuf *c) { return !c || c->read == c->write; }
void *rtw_cbuf_pop(struct rtw_cbuf *c)
{
	void *b;
	if (!c || rtw_cbuf_empty(c))
		return NULL;
	b = c->bufs[c->read];
	c->bufs[c->read++] = NULL;
	if (c->read >= c->size)
		c->read = 0;
	return b;
}
void c2h_wk_callback(_workitem *work) { (void)work; }
