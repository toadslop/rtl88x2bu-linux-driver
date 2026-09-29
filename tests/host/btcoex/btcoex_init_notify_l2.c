// SPDX-License-Identifier: GPL-2.0
/* W3-126 L2 C oracle: btcoex init + notify leaf (core/rtw_btcoex.c). */
#include <stdio.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

typedef int sint;
#define _TRUE 1
#define _FALSE 0
#define RT_MEDIA_CONNECT 1
#define RT_MEDIA_DISCONNECT 0
#define HW_VAR_DL_RSVD_PAGE 42
#define WIFI_AP_STATE 0x10
#define WIFI_ASOC_STATE 1
#define WIFI_UNDER_SURVEY 0x800

typedef struct {
	u32 fw_state;
} mock_mlme;
typedef struct {
	u8 eeprom_coexist;
} mock_hal;
typedef struct {
	u8 mgmt_tx, roch;
} mock_dv;
typedef struct {
	mock_mlme mlme;
	mock_hal hal;
	mock_dv dv;
	u8 buddy, sreset;
} mock_adpt;

struct host_btcoex_tr {
	unsigned init, pwr_on, ant, hw_init, ips, lps, scan, media, dl_rsvd;
	u8 last_type, last_wifi_only;
};

static struct host_btcoex_tr g_tr;

static void tr_reset(void) { memset(&g_tr, 0, sizeof(g_tr)); }

void host_btcoex_dl_rsvd_inc(void) { g_tr.dl_rsvd++; }

#ifndef HOST_BTCOEX_INIT_NOTIFY_RUST
static sint chk_fw(mock_mlme *m, sint st)
{
	return (!st && !m->fw_state) || (m->fw_state & (u32)st) ? _TRUE : _FALSE;
}

static u8 buddy_survey(mock_adpt *a) { return a->buddy ? _TRUE : _FALSE; }
#endif

void hal_init(mock_adpt *a) { (void)a; g_tr.init++; }
void hal_pwr_on(mock_adpt *a) { (void)a; g_tr.pwr_on++; }
void hal_ant(mock_adpt *a) { (void)a; g_tr.ant++; }
void hal_hw_init(mock_adpt *a, u8 w) { (void)a; g_tr.hw_init++; g_tr.last_wifi_only = w; }
void hal_ips(mock_adpt *a, u8 t) { (void)a; g_tr.ips++; g_tr.last_type = t; }
void hal_lps(mock_adpt *a, u8 t) { (void)a; g_tr.lps++; g_tr.last_type = t; }
void hal_scan(mock_adpt *a, u8 t) { (void)a; g_tr.scan++; g_tr.last_type = t; }
void hal_media(mock_adpt *a, u8 t) { (void)a; g_tr.media++; g_tr.last_type = t; }

#ifndef HOST_BTCOEX_INIT_NOTIFY_RUST
static void o_init(mock_adpt *a) { hal_init(a); }
static void o_pwr_on(mock_adpt *a) { hal_pwr_on(a); }
static void o_ant(mock_adpt *a) { hal_ant(a); }
static void o_hw_init(mock_adpt *a, u8 w) { hal_hw_init(a, w); }
static void o_ips(mock_adpt *a, u8 t)
{
	if (!a->hal.eeprom_coexist)
		return;
	hal_ips(a, t);
}
static void o_lps(mock_adpt *a, u8 t)
{
	if (!a->hal.eeprom_coexist)
		return;
	hal_lps(a, t);
}
static void o_scan(mock_adpt *a, u8 t)
{
	if (!a->hal.eeprom_coexist)
		return;
	if (!t && (buddy_survey(a) || a->dv.mgmt_tx || a->dv.roch))
		return;
	hal_scan(a, t);
}
static void o_media(mock_adpt *a, u8 st)
{
	if (!a->hal.eeprom_coexist || a->sreset)
		return;
	if (st == RT_MEDIA_DISCONNECT && buddy_survey(a))
		return;
	if (st == RT_MEDIA_CONNECT && chk_fw(&a->mlme, WIFI_AP_STATE) == _TRUE)
		g_tr.dl_rsvd++;
	hal_media(a, st);
}
#else
extern void o_init(mock_adpt *a);
extern void o_pwr_on(mock_adpt *a);
extern void o_ant(mock_adpt *a);
extern void o_hw_init(mock_adpt *a, u8 w);
extern void o_ips(mock_adpt *a, u8 t);
extern void o_lps(mock_adpt *a, u8 t);
extern void o_scan(mock_adpt *a, u8 t);
extern void o_media(mock_adpt *a, u8 st);
#endif

struct vector {
	char name[48], fn[24];
	int coex, buddy, mgmt, roch, sreset, fw, arg, wifi_only;
	int exp_hal, exp_dl, exp_last;
};

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(o, l, "coex", &v->coex);
	host_json_parse_int_in(o, l, "buddy", &v->buddy);
	host_json_parse_int_in(o, l, "mgmt", &v->mgmt);
	host_json_parse_int_in(o, l, "roch", &v->roch);
	host_json_parse_int_in(o, l, "sreset", &v->sreset);
	host_json_parse_int_in(o, l, "fw", &v->fw);
	host_json_parse_int_in(o, l, "arg", &v->arg);
	host_json_parse_int_in(o, l, "wifi_only", &v->wifi_only);
	host_json_parse_int_in(o, l, "exp_hal", &v->exp_hal);
	host_json_parse_int_in(o, l, "exp_dl", &v->exp_dl);
	host_json_parse_int_in(o, l, "exp_last", &v->exp_last);
	return 0;
}

static int run_vec(struct vector *v)
{
	mock_adpt a = {0};

	tr_reset();
	a.hal.eeprom_coexist = (u8)v->coex;
	a.buddy = (u8)v->buddy;
	a.dv.mgmt_tx = (u8)v->mgmt;
	a.dv.roch = (u8)v->roch;
	a.sreset = (u8)v->sreset;
	a.mlme.fw_state = (u32)v->fw;

	if (!strcmp(v->fn, "init"))
		o_init(&a);
	else if (!strcmp(v->fn, "pwr_on"))
		o_pwr_on(&a);
	else if (!strcmp(v->fn, "ips"))
		o_ips(&a, (u8)v->arg);
	else if (!strcmp(v->fn, "lps"))
		o_lps(&a, (u8)v->arg);
	else if (!strcmp(v->fn, "ant"))
		o_ant(&a);
	else if (!strcmp(v->fn, "scan"))
		o_scan(&a, (u8)v->arg);
	else if (!strcmp(v->fn, "media"))
		o_media(&a, (u8)v->arg);
	else if (!strcmp(v->fn, "hw_init"))
		o_hw_init(&a, (u8)v->wifi_only);
	else
		return -1;

	unsigned got = 0;

	if (!strcmp(v->fn, "init"))
		got = g_tr.init;
	else if (!strcmp(v->fn, "pwr_on"))
		got = g_tr.pwr_on;
	else if (!strcmp(v->fn, "hw_init"))
		got = g_tr.hw_init;
	else if (!strcmp(v->fn, "ips"))
		got = g_tr.ips;
	else if (!strcmp(v->fn, "lps"))
		got = g_tr.lps;
	else if (!strcmp(v->fn, "ant"))
		got = g_tr.ant;
	else if (!strcmp(v->fn, "scan"))
		got = g_tr.scan;
	else if (!strcmp(v->fn, "media"))
		got = g_tr.media;

	if (got != (unsigned)v->exp_hal || g_tr.dl_rsvd != (unsigned)v->exp_dl ||
	    (v->exp_last && g_tr.last_type != (u8)v->exp_last)) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[24];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "btcoex_init_notify_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 24, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
