// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_mlme_roaming_types.h"
#include "host_vector_json.h"

#define MAX_NETS 8
static systime host_now;
static int host_desired_ok = 1;
struct _adapter g_adapter;
static struct wlan_network net_pool[MAX_NETS];
static int net_pool_used;

systime rtw_get_current_time(void) { return host_now; }
u32 rtw_get_passing_time_ms(systime s) { return (u32)(host_now - s); }
int _rtw_memcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n) == 0; }
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
	(void)a; (void)n;
	return host_desired_ok ? _TRUE : _FALSE;
}

static void init_list(_list *l) { l->next = l->prev = l; }

static void fill_bss(WLAN_BSSID_EX *b, const char *ssid, int rssi, u8 ch, const char *mac)
{
	u8 m[ETH_ALEN]; size_t n = 0;
	memset(b, 0, sizeof(*b));
	b->Ssid.SsidLength = (u32)strlen(ssid);
	memcpy(b->Ssid.Ssid, ssid, b->Ssid.SsidLength);
	b->Configuration.DSConfig = ch;
	b->Rssi = rssi;
	if (mac && mac[0] && !host_hex_decode(mac, m, sizeof(m), &n))
		memcpy(b->MacAddress, m, ETH_ALEN);
}

static struct wlan_network *mk_net(const char *ssid, int rssi, u8 ch, const char *mac, u32 age)
{
	struct wlan_network *w;
	if (net_pool_used >= MAX_NETS)
		return NULL;
	w = &net_pool[net_pool_used++];
	init_list(&w->list);
	fill_bss(&w->network, ssid, rssi, ch, mac);
	w->last_scanned = host_now - age;
	return w;
}

struct vector {
	char name[64], fn[16], cur_ssid[33], cur_mac[13], roam_tgt_mac[13];
	char comp_ssid[33], comp_mac[13], net0_ssid[33], net0_mac[13], net1_ssid[33];
	char net1_mac[13], expect_mac[13];
	int cur_rssi, cur_ch, need_to_roam, rssi_diff_th, scanr_exp_ms, desired;
	int comp_rssi, comp_ch, comp_age_ms, net0_rssi, net0_ch, net0_age_ms;
	int net1_rssi, net1_ch, net1_age_ms, expect;
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
	S("cur_ssid", cur_ssid); I("cur_rssi", cur_rssi); I("cur_ch", cur_ch);
	S("cur_mac", cur_mac); I("need_to_roam", need_to_roam); I("rssi_diff_th", rssi_diff_th);
	I("scanr_exp_ms", scanr_exp_ms); I("desired", desired); S("roam_tgt_mac", roam_tgt_mac);
	S("comp_ssid", comp_ssid); I("comp_rssi", comp_rssi); I("comp_ch", comp_ch);
	S("comp_mac", comp_mac); I("comp_age_ms", comp_age_ms);
	S("net0_ssid", net0_ssid); I("net0_rssi", net0_rssi); I("net0_ch", net0_ch);
	S("net0_mac", net0_mac); I("net0_age_ms", net0_age_ms);
	S("net1_ssid", net1_ssid); I("net1_rssi", net1_rssi); I("net1_ch", net1_ch);
	S("net1_mac", net1_mac); I("net1_age_ms", net1_age_ms);
	I("expect", expect); S("expect_mac", expect_mac);
	return 0;
#undef S
#undef I
}

static void setup(struct vector *v)
{
	struct mlme_priv *m = &g_adapter.mlmepriv;
	struct wlan_network *cur;
	struct rf_ctl_t *rf;
	size_t n = 0;

	host_now = 100000;
	net_pool_used = 0;
	memset(&g_adapter, 0, sizeof(g_adapter));
	init_list(&m->scanned_queue.queue);
	m->nic_hdl = &g_adapter;
	host_desired_ok = v->desired;
	rf = &g_adapter.rfctl;
	rf->channel_set[0].ChannelNum = (u8)v->cur_ch;
	rf->channel_set[1].ChannelNum = 6;
	rf->channel_set[2].ChannelNum = 11;
	cur = mk_net(v->cur_ssid, v->cur_rssi, (u8)v->cur_ch, v->cur_mac, 0);
	m->cur_network_scanned = cur;
	fill_bss(&m->cur_network.network, v->cur_ssid, v->cur_rssi, (u8)v->cur_ch, v->cur_mac);
	m->need_to_roam = (u8)v->need_to_roam;
	m->roam_rssi_diff_th = v->rssi_diff_th;
	m->roam_scanr_exp_ms = (u32)v->scanr_exp_ms;
	if (v->roam_tgt_mac[0])
		host_hex_decode(v->roam_tgt_mac, m->roam_tgt_addr, ETH_ALEN, &n);
	if (v->net0_ssid[0]) {
		struct wlan_network *w = mk_net(v->net0_ssid, v->net0_rssi, (u8)v->net0_ch,
						v->net0_mac, (u32)v->net0_age_ms);
		if (w)
			rtw_list_insert_tail(&w->list, &m->scanned_queue.queue);
	}
	if (v->net1_ssid[0]) {
		struct wlan_network *w = mk_net(v->net1_ssid, v->net1_rssi, (u8)v->net1_ch,
						v->net1_mac, (u32)v->net1_age_ms);
		if (w)
			rtw_list_insert_tail(&w->list, &m->scanned_queue.queue);
	}
}

static int mac_ok(struct wlan_network *w, const char *mac)
{
	u8 m[ETH_ALEN]; size_t n = 0;
	if (!mac || !mac[0] || host_hex_decode(mac, m, sizeof(m), &n) || n != ETH_ALEN)
		return 0;
	return _rtw_memcmp(w->network.MacAddress, m, ETH_ALEN);
}

static int run(struct vector *v)
{
	struct mlme_priv *m = &g_adapter.mlmepriv;
	setup(v);
	if (!strcmp(v->fn, "select")) {
		int r = rtw_select_roaming_candidate(m);
		if ((r == _SUCCESS) != v->expect)
			return -1;
		if (v->expect && v->expect_mac[0] &&
		    (!m->roam_network || !mac_ok(m->roam_network, v->expect_mac)))
			return -1;
		return 0;
	}
	if (!strcmp(v->fn, "check")) {
		struct wlan_network *c = NULL;
		struct wlan_network *comp = mk_net(v->comp_ssid, v->comp_rssi, (u8)v->comp_ch,
						   v->comp_mac, (u32)v->comp_age_ms);
		return (rtw_check_roaming_candidate(m, &c, comp) == _TRUE) == v->expect ? 0 : -1;
	}
	return -1;
}

int main(int argc, char **argv)
{
	struct vector v[8];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "mlme_roaming_vectors.json";

	if (host_load_vectors(path, v, sizeof(v[0]), 8, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad += run(&v[i]) ? (fprintf(stderr, "FAIL %s\n", v[i].name), 1) : (printf("PASS %s\n", v[i].name), 0);
	if (!bad)
		printf("PASS %zu vectors (oracle: core/rtw_mlme_rest.c) (%s)\n", n, path);
	return bad ? 1 : 0;
}
