// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_mlme_unassoc_types.h"
#include "host_vector_json.h"

static systime host_now;
struct _adapter g_adapter;

systime rtw_get_current_time(void) { return host_now; }
systime rtw_ms_to_systime(int ms) { return (systime)ms; }
bool rtw_time_before(systime a, systime b) { return a < b; }
bool rtw_time_after(systime a, systime b) { return a > b; }
void rtw_run_in_thread_cmd(_adapter *a, void *f, _adapter *c) { (void)a; (void)f; (void)c; }
void rtw_hal_rcr_set_chk_bssid_act_non(_adapter *a) { (void)a; }
void *rtw_zvmalloc(u32 sz) { return calloc(1, sz); }
void rtw_vmfree(void *p, u32 sz) { (void)sz; free(p); }
int _rtw_memcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n) == 0; }

void rtw_del_unassoc_sta_queue(_adapter *a);
void rtw_del_unassoc_sta(_adapter *a, u8 *addr);
u8 rtw_search_unassoc_sta(_adapter *a, u8 *addr, struct unassoc_sta_info *out);
#ifndef RUST_MLME_UNASSOC_DEL_ONLY
void rtw_add_interested_unassoc_sta(_adapter *a, u8 *addr);
void rtw_undo_interested_unassoc_sta(_adapter *a, u8 *addr);
void rtw_rx_add_unassoc_sta(_adapter *a, u8 stype, u8 *addr, s8 rssi);
#endif

static inline void _rtw_init_listhead(_list *l) { l->next = l->prev = l; }

static struct unassoc_sta_info *pop_free(struct mlme_priv *m)
{
	_irqL irqL; _queue *fq = &m->free_unassoc_sta_queue; struct unassoc_sta_info *s; _list *l;

	_enter_critical_bh(&fq->lock, &irqL);
	if (_rtw_queue_empty(fq)) { s = NULL; goto out; }
	l = get_next(&fq->queue);
	s = LIST_CONTAINOR(l, struct unassoc_sta_info, list);
	rtw_list_delete(&s->list);
	_rtw_memset(s->addr, 0, ETH_ALEN);
	s->recv_signal_power = 0; s->time = 0; s->interested = 0;
out:
	_exit_critical_bh(&fq->lock, &irqL);
	return s;
}

static void reset_adapter(u32 max)
{
	struct mlme_priv *m = &g_adapter.mlmepriv; u32 i;

	host_now = 5000;
	memset(m, 0, sizeof(*m));
	m->max_unassoc_sta_cnt = max;
	m->unassoc_sta_mode_of_stype[0] = UNASOC_STA_MODE_ALL;
	m->unassoc_sta_mode_of_stype[1] = UNASOC_STA_MODE_ALL;
	_rtw_init_listhead(&m->unassoc_sta_queue.queue);
	_rtw_init_listhead(&m->free_unassoc_sta_queue.queue);
	m->free_unassoc_sta_buf = rtw_zvmalloc(max * sizeof(struct unassoc_sta_info));
	for (i = 0; i < max; i++) {
		struct unassoc_sta_info *s = (struct unassoc_sta_info *)m->free_unassoc_sta_buf + i;

		_rtw_init_listhead(&s->list);
		rtw_list_insert_tail(&s->list, &m->free_unassoc_sta_queue.queue);
	}
}

static int qlen(void)
{
	int n = 0; _list *p;

	for (p = g_adapter.mlmepriv.unassoc_sta_queue.queue.next;
	     p != &g_adapter.mlmepriv.unassoc_sta_queue.queue; p = p->next)
		n++;
	return n;
}

static void seed(const u8 *mac, u8 interested, s8 rssi, systime t)
{
	struct mlme_priv *m = &g_adapter.mlmepriv; struct unassoc_sta_info *s = pop_free(m);

	if (!s) return;
	_rtw_memcpy(s->addr, mac, ETH_ALEN);
	s->interested = interested; s->recv_signal_power = rssi; s->time = t;
	if (interested) m->interested_unassoc_sta_cnt++;
	rtw_list_insert_tail(&s->list, &m->unassoc_sta_queue.queue);
}

struct vector { char name[40], fn[32], mac[13]; int stype, interested, rssi, expect, expect_q; };

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_string_in(o, l, "mac", v->mac, sizeof(v->mac));
	host_json_parse_int_in(o, l, "stype", &v->stype);
	host_json_parse_int_in(o, l, "interested", &v->interested);
	host_json_parse_int_in(o, l, "rssi", &v->rssi);
	host_json_parse_int_in(o, l, "expect", &v->expect);
	host_json_parse_int_in(o, l, "expect_q", &v->expect_q);
	return 0;
}

static int run(struct vector *v)
{
	u8 addr[ETH_ALEN]; struct unassoc_sta_info found;

	reset_adapter(4);
	if (!strcmp(v->fn, "search_hit")) {
		u8 mac[ETH_ALEN] = {0, 0x11, 0x22, 0x33, 0x44, 0x55};

		seed(mac, 0, -40, 4000);
		if (!rtw_search_unassoc_sta(&g_adapter, mac, &found)) goto fail;
	} else if (!strcmp(v->fn, "search_miss")) {
		u8 miss[ETH_ALEN] = {0xde, 0xad, 0, 0, 0, 1};

		if (rtw_search_unassoc_sta(&g_adapter, miss, &found)) goto fail;
	} else if (!strcmp(v->fn, "del_one")) {
		size_t n = 0;

		if (host_hex_decode(v->mac, addr, ETH_ALEN, &n) || n != ETH_ALEN) goto fail;
		seed(addr, (u8)v->interested, (s8)v->rssi, 100);
		rtw_del_unassoc_sta(&g_adapter, addr);
		if (qlen() != v->expect_q || g_adapter.mlmepriv.interested_unassoc_sta_cnt != (u32)v->expect)
			goto fail;
	} else if (!strcmp(v->fn, "del_queue")) {
		u8 a1[ETH_ALEN] = {1, 0, 0, 0, 0, 1}, a2[ETH_ALEN] = {2, 0, 0, 0, 0, 2};

		seed(a1, 0, -10, 100); seed(a2, 1, -20, 200);
		rtw_del_unassoc_sta_queue(&g_adapter);
		if (qlen()) goto fail;
#ifndef RUST_MLME_UNASSOC_DEL_ONLY
	} else if (!strcmp(v->fn, "add_interested_new")) {
		u8 mac[ETH_ALEN] = {0xaa, 0xbb, 0, 0, 0, 1};

		rtw_add_interested_unassoc_sta(&g_adapter, mac);
		if (qlen() != 1 || g_adapter.mlmepriv.interested_unassoc_sta_cnt != 1) goto fail;
		if (!rtw_search_unassoc_sta(&g_adapter, mac, &found) || !found.interested) goto fail;
	} else if (!strcmp(v->fn, "add_interested_existing")) {
		u8 mac[ETH_ALEN] = {0xcc, 0xdd, 0, 0, 0, 2};

		seed(mac, 0, -50, 1000);
		rtw_add_interested_unassoc_sta(&g_adapter, mac);
		if (qlen() != 1 || g_adapter.mlmepriv.interested_unassoc_sta_cnt != 1) goto fail;
		if (!rtw_search_unassoc_sta(&g_adapter, mac, &found) || !found.interested) goto fail;
	} else if (!strcmp(v->fn, "undo_interested")) {
		u8 mac[ETH_ALEN] = {0xee, 0xff, 0, 0, 0, 3};

		seed(mac, 1, -30, 2000);
		rtw_undo_interested_unassoc_sta(&g_adapter, mac);
		if (g_adapter.mlmepriv.interested_unassoc_sta_cnt != 0) goto fail;
		if (!rtw_search_unassoc_sta(&g_adapter, mac, &found) || found.interested) goto fail;
	} else if (!strcmp(v->fn, "rx_add")) {
		size_t n = 0;
		u8 stype = (u8)v->stype;

		if (host_hex_decode(v->mac, addr, ETH_ALEN, &n) || n != ETH_ALEN) goto fail;
		g_adapter.mlmepriv.unassoc_sta_mode_of_stype[stype] = UNASOC_STA_MODE_ALL;
		rtw_rx_add_unassoc_sta(&g_adapter, stype, addr, (s8)v->rssi);
		if (qlen() != v->expect_q) goto fail;
		if (v->expect_q && !rtw_search_unassoc_sta(&g_adapter, addr, &found)) goto fail;
		if (v->expect_q && found.recv_signal_power != (s8)v->rssi) goto fail;
	} else if (!strcmp(v->fn, "rx_add_update")) {
		size_t n = 0;
		u8 stype = (u8)v->stype;

		if (host_hex_decode(v->mac, addr, ETH_ALEN, &n) || n != ETH_ALEN) goto fail;
		seed(addr, 0, -10, 1000);
		g_adapter.mlmepriv.unassoc_sta_mode_of_stype[stype] = UNASOC_STA_MODE_ALL;
		rtw_rx_add_unassoc_sta(&g_adapter, stype, addr, (s8)v->rssi);
		if (qlen() != 1 || !rtw_search_unassoc_sta(&g_adapter, addr, &found)) goto fail;
		if (found.recv_signal_power != (s8)v->rssi) goto fail;
	} else if (!strcmp(v->fn, "rx_add_expire_cleanup")) {
		size_t n = 0;
		u8 stype = (u8)v->stype;

		host_now = 100000;
		if (host_hex_decode(v->mac, addr, ETH_ALEN, &n) || n != ETH_ALEN) goto fail;
		seed(addr, 0, -10, 100);
		g_adapter.mlmepriv.unassoc_sta_mode_of_stype[stype] = UNASOC_STA_MODE_INTERESTED;
		rtw_rx_add_unassoc_sta(&g_adapter, stype, addr, (s8)v->rssi);
		if (qlen() != v->expect_q) goto fail;
#endif
	} else goto fail;
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector v[16]; size_t n = 0; int bad = 0;
	const char *path = argc > 1 ? argv[1] : "mlme_unassoc_vectors.json";

	if (host_load_vectors(path, v, sizeof(v[0]), 16, parse_vec, &n)) return 2;
	for (size_t i = 0; i < n; i++) bad += run(&v[i]);
#ifndef RUST_MLME_UNASSOC_ORACLE
	if (!bad) printf("PASS %zu vectors (oracle: core/rtw_mlme_rest.c) (%s)\n", n, path);
#else
	if (!bad) printf("PASS %zu vectors (oracle: rust/rtw_mlme_unassoc.rs) (%s)\n", n, path);
#endif
	return bad ? 1 : 0;
}
