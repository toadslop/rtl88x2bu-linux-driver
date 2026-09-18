// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_mlme_join_select_types.h"
#include "host_vector_json.h"

#define MAX_NETS 8
static int host_desired_ok = 1;
static int host_scan_deny_drv;
struct _adapter g_adapter;
static struct wlan_network net_pool[MAX_NETS];
static int net_pool_used;
struct wlan_network *host_join_candidate;

int _rtw_memcmp(const void *a, const void *b, size_t n)
{
	return memcmp(a, b, n) == 0;
}

int rtw_chset_search_ch(RT_CHANNEL_INFO *cs, u32 ch)
{
	u8 i;

	for (i = 0; i < 14; i++)
		if (cs[i].ChannelNum == ch)
			return (int)i;
	return -1;
}

int rtw_is_desired_network(_adapter *a, struct wlan_network *n)
{
	(void)a;
	(void)n;
	return host_desired_ok ? _TRUE : _FALSE;
}

int rtw_is_scan_deny(_adapter *a)
{
	(void)a;
	return host_scan_deny_drv ? _TRUE : _FALSE;
}

int rtw_joinbss_cmd(_adapter *a, struct wlan_network *n)
{
	(void)a;
	host_join_candidate = n;
	return _SUCCESS;
}

static void init_list(_list *l)
{
	l->next = l->prev = l;
}

static void fill_bss(WLAN_BSSID_EX *b, const char *ssid, int rssi, u8 ch, const char *mac)
{
	u8 m[ETH_ALEN];
	size_t n = 0;

	memset(b, 0, sizeof(*b));
	b->Ssid.SsidLength = (u32)strlen(ssid);
	memcpy(b->Ssid.Ssid, ssid, b->Ssid.SsidLength);
	b->Configuration.DSConfig = ch;
	b->Rssi = rssi;
	if (mac && mac[0] && !host_hex_decode(mac, m, sizeof(m), &n))
		memcpy(b->MacAddress, m, ETH_ALEN);
}

static struct wlan_network *mk_net(const char *ssid, int rssi, u8 ch, const char *mac)
{
	struct wlan_network *w;

	if (net_pool_used >= MAX_NETS)
		return NULL;
	w = &net_pool[net_pool_used++];
	init_list(&w->list);
	fill_bss(&w->network, ssid, rssi, ch, mac);
	return w;
}

struct vector {
	char name[64];
	char fn[16];
	char assoc_ssid[33];
	char assoc_bssid[13];
	char net0_ssid[33], net0_mac[13], net1_ssid[33], net1_mac[13];
	char expect_mac[13];
	int assoc_by_bssid, desired, fw_state, dvobj_scan_deny, scan_deny_drv;
	int net0_rssi, net0_ch, net1_rssi, net1_ch;
	int expect, expect_ssc;
};

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn)))
		return -1;
#define S(f, fld) host_json_parse_string_in(o, l, f, v->fld, sizeof(v->fld))
#define I(f, fld) host_json_parse_int_in(o, l, f, &v->fld)
	S("assoc_ssid", assoc_ssid);
	I("assoc_by_bssid", assoc_by_bssid);
	S("assoc_bssid", assoc_bssid);
	I("desired", desired);
	I("fw_state", fw_state);
	I("dvobj_scan_deny", dvobj_scan_deny);
	I("scan_deny_drv", scan_deny_drv);
	S("net0_ssid", net0_ssid);
	I("net0_rssi", net0_rssi);
	I("net0_ch", net0_ch);
	S("net0_mac", net0_mac);
	S("net1_ssid", net1_ssid);
	I("net1_rssi", net1_rssi);
	I("net1_ch", net1_ch);
	S("net1_mac", net1_mac);
	I("expect", expect);
	I("expect_ssc", expect_ssc);
	S("expect_mac", expect_mac);
	return 0;
#undef S
#undef I
}

static void setup(struct vector *v)
{
	struct mlme_priv *m = &g_adapter.mlmepriv;
	struct rf_ctl_t *rf;
	size_t n = 0;

	net_pool_used = 0;
	host_join_candidate = NULL;
	memset(&g_adapter, 0, sizeof(g_adapter));
	init_list(&m->scanned_queue.queue);
	m->nic_hdl = &g_adapter;
	m->fw_state = v->fw_state;
	m->assoc_by_bssid = (u32)v->assoc_by_bssid;
	host_desired_ok = v->desired;
	host_scan_deny_drv = v->scan_deny_drv;
	g_adapter.dvobj.scan_deny = v->dvobj_scan_deny ? _TRUE : _FALSE;
	rf = &g_adapter.rfctl;
	rf->channel_set[0].ChannelNum = 6;
	rf->channel_set[1].ChannelNum = 11;
	if (v->assoc_ssid[0]) {
		m->assoc_ssid.SsidLength = (u32)strlen(v->assoc_ssid);
		memcpy(m->assoc_ssid.Ssid, v->assoc_ssid, m->assoc_ssid.SsidLength);
	}
	if (v->assoc_bssid[0])
		host_hex_decode(v->assoc_bssid, m->assoc_bssid, ETH_ALEN, &n);
	if (v->net0_ssid[0]) {
		struct wlan_network *w = mk_net(v->net0_ssid, v->net0_rssi, (u8)v->net0_ch, v->net0_mac);

		if (w)
			rtw_list_insert_tail(&w->list, &m->scanned_queue.queue);
	}
	if (v->net1_ssid[0]) {
		struct wlan_network *w = mk_net(v->net1_ssid, v->net1_rssi, (u8)v->net1_ch, v->net1_mac);

		if (w)
			rtw_list_insert_tail(&w->list, &m->scanned_queue.queue);
	}
}

static int mac_ok(struct wlan_network *w, const char *mac)
{
	u8 m[ETH_ALEN];
	size_t n = 0;

	if (!mac || !mac[0] || host_hex_decode(mac, m, sizeof(m), &n) || n != ETH_ALEN)
		return 0;
	return _rtw_memcmp(w->network.MacAddress, m, ETH_ALEN);
}

static int run(struct vector *v)
{
	struct mlme_priv *m = &g_adapter.mlmepriv;

	setup(v);
	if (!strcmp(v->fn, "ssc")) {
		u8 r = _rtw_sitesurvey_condition_check("test", &g_adapter, 0);

		return (int)r == v->expect_ssc ? 0 : -1;
	}
	if (!strcmp(v->fn, "check")) {
		struct wlan_network *c = NULL;
		struct wlan_network *comp = mk_net(v->net0_ssid, v->net0_rssi, (u8)v->net0_ch, v->net0_mac);

		return (rtw_check_join_candidate(m, &c, comp) == _TRUE) == v->expect ? 0 : -1;
	}
	if (!strcmp(v->fn, "select")) {
		int r = rtw_select_and_join_from_scanned_queue(m);

		if ((r == _SUCCESS) != v->expect)
			return -1;
		if (v->expect && v->expect_mac[0] &&
		    (!host_join_candidate || !mac_ok(host_join_candidate, v->expect_mac)))
			return -1;
		return 0;
	}
	return -1;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "mlme_join_select_vectors.json";

	if (host_load_vectors(path, v, sizeof(v[0]), 16, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad += run(&v[i]) ? (fprintf(stderr, "FAIL %s\n", v[i].name), 1) :
				    (printf("PASS %s\n", v[i].name), 0);
#ifndef RUST_MLME_JOIN_SELECT_ORACLE
	if (!bad)
		printf("PASS %zu vectors (oracle: core/rtw_mlme_rest.c) (%s)\n", n, path);
#else
	if (!bad)
		printf("PASS %zu vectors (oracle: rust/rtw_mlme_join_select.rs) (%s)\n", n, path);
#endif
	return bad ? 1 : 0;
}
