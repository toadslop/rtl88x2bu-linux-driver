// SPDX-License-Identifier: GPL-2.0
/* W3-124 follow-up (#979) PR3+PR4: L2 C oracle connect/disassociate + set_bssid. */
#include <stdio.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define WIFI_ASOC_STATE 0x00000001
#define WIFI_UNDER_LINKING 0x00000080
#define WIFI_UNDER_SURVEY 0x00000800

typedef int sint;
typedef struct { u32 SsidLength; u8 Ssid[32]; } NDIS_802_11_SSID;
struct mlme_priv {
	u32 fw_state;
	u8 to_join, assoc_by_bssid;
	u16 assoc_ch;
	u8 assoc_bssid[ETH_ALEN];
	NDIS_802_11_SSID assoc_ssid;
};
struct _adapter {
	u8 hw_init_done, tkip_fail, do_join_ret;
	struct mlme_priv mlmepriv;
};

#ifdef HOST_IOCTL_CONNECT_RUST
extern u8 rtw_set_802_11_disassociate_rust(struct _adapter *p);
extern u8 rtw_set_802_11_connect_rust(struct _adapter *p, u8 *bssid,
				      NDIS_802_11_SSID *ssid, u16 ch);
extern u8 rtw_set_802_11_bssid_rust(struct _adapter *p, u8 *bssid);
extern void ioctl_connect_test_reset_counters(void);
extern u32 ioctl_connect_test_disassoc_calls(void);
extern u32 ioctl_connect_test_join_calls(void);
#define rtw_set_802_11_disassociate rtw_set_802_11_disassociate_rust
#define rtw_set_802_11_connect rtw_set_802_11_connect_rust
#define rtw_set_802_11_bssid rtw_set_802_11_bssid_rust
#else
static u32 g_disassoc_calls, g_join_calls;
#endif

#ifndef HOST_IOCTL_CONNECT_RUST
/* Mirrors core/rtw_ioctl_set.c rtw_validate_* (see ioctl_validate_l2.c / PR2). */
static u8 v_bssid(u8 *b)
{
	if (!(b[0]|b[1]|b[2]|b[3]|b[4]|b[5]) || (b[0]&b[1]&b[2]&b[3]&b[4]&b[5])==0xff)
		return _FALSE;
	return ((b[0]&0x01) && b[0]!=0xff) ? _FALSE : _TRUE;
}
static u8 v_ssid(NDIS_802_11_SSID *s) { return s->SsidLength > 32 ? _FALSE : _TRUE; }
static sint chk(struct mlme_priv *m, sint st)
{
	return (!st && !m->fw_state) || (m->fw_state & (u32)st) ? _TRUE : _FALSE;
}

static u8 rtw_set_802_11_disassociate(struct _adapter *p)
{
	if (chk(&p->mlmepriv, WIFI_ASOC_STATE))
		g_disassoc_calls++;
	return _TRUE;
}

static u8 rtw_set_802_11_connect(struct _adapter *p, u8 *bssid, NDIS_802_11_SSID *ssid, u16 ch)
{
	u8 ok = _SUCCESS, bv = _TRUE, sv = _TRUE;
	struct mlme_priv *m = &p->mlmepriv;

	if (!ssid || !v_ssid(ssid))
		sv = _FALSE;
	if (!bssid || !v_bssid(bssid))
		bv = _FALSE;
	if (!sv && !bv)
		return _FAIL;
	if (!p->hw_init_done)
		return _FAIL;

	/* Order matches core/rtw_ioctl_set.c rtw_set_802_11_connect. */
	if (chk(m, WIFI_UNDER_SURVEY))
		; /* handle_tkip_countermeasure */
	else if (chk(m, WIFI_UNDER_LINKING))
		return _SUCCESS;

	if (p->tkip_fail)
		return _FAIL;
	if (ssid && sv)
		memcpy(&m->assoc_ssid, ssid, sizeof(*ssid));
	else
		memset(&m->assoc_ssid, 0, sizeof(m->assoc_ssid));
	if (bssid && bv) {
		memcpy(m->assoc_bssid, bssid, ETH_ALEN);
		m->assoc_by_bssid = _TRUE;
	} else
		m->assoc_by_bssid = _FALSE;
	m->assoc_ch = ch;
	if (chk(m, WIFI_UNDER_SURVEY))
		m->to_join = _TRUE;
	else {
		g_join_calls++;
		ok = p->do_join_ret ? _SUCCESS : _FAIL;
	}
	return ok;
}
#endif /* !HOST_IOCTL_CONNECT_RUST */

#ifndef HOST_IOCTL_CONNECT_RUST
static sint chk_mlme(struct mlme_priv *m, sint st)
{
	return (!st && !m->fw_state) || (m->fw_state & (u32)st) ? _TRUE : _FALSE;
}

static u8 is_bad_set_bssid(u8 *bssid)
{
	int i, zero = 1, ff = 1;

	for (i = 0; i < ETH_ALEN; i++) {
		if (bssid[i])
			zero = 0;
		if (bssid[i] != 0xff)
			ff = 0;
	}
	return zero || ff;
}

static u8 rtw_set_802_11_bssid(struct _adapter *p, u8 *bssid)
{
	u8 status = _SUCCESS;
	struct mlme_priv *m = &p->mlmepriv;

	if (is_bad_set_bssid(bssid))
		return _FAIL;

	if (chk_mlme(m, WIFI_UNDER_SURVEY))
		;
	else if (chk_mlme(m, WIFI_UNDER_LINKING))
		return _SUCCESS;

	if (p->tkip_fail)
		return _FAIL;

	memset(&m->assoc_ssid, 0, sizeof(m->assoc_ssid));
	memcpy(m->assoc_bssid, bssid, ETH_ALEN);
	m->assoc_ch = 0;
	m->assoc_by_bssid = _TRUE;

	if (chk_mlme(m, WIFI_UNDER_SURVEY))
		m->to_join = _TRUE;
	else {
		g_join_calls++;
		status = p->do_join_ret ? _SUCCESS : _FAIL;
	}
	return status;
}
#endif /* !HOST_IOCTL_CONNECT_RUST */

struct vector {
	char name[48], fn[16], bssid[13], expect_bssid[13];
	int fw_state, hw_init, tkip_fail, do_join_ret, ch, ssid_len;
	int expect_ret, expect_disassoc, expect_join, expect_to_join;
	int expect_assoc_ch, expect_assoc_by_bssid, expect_assoc_ssid_len;
};

static int dec_mac(const char *h, u8 *m)
{
	for (int i = 0; i < ETH_ALEN; i++) {
		int a = h[i*2], b = h[i*2+1], hi, lo;

		if (a >= '0' && a <= '9') hi = a - '0';
		else if (a >= 'a' && a <= 'f') hi = a - 'a' + 10;
		else return -1;
		if (b >= '0' && b <= '9') lo = b - '0';
		else if (b >= 'a' && b <= 'f') lo = b - 'a' + 10;
		else return -1;
		m[i] = (u8)((hi << 4) | lo);
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *k[] = {"fw_state","hw_init","tkip_fail","do_join_ret","ch","ssid_len",
			   "expect_ret","expect_disassoc","expect_join","expect_to_join",
			   "expect_assoc_ch","expect_assoc_by_bssid","expect_assoc_ssid_len", NULL};
	int *p[] = {&v->fw_state,&v->hw_init,&v->tkip_fail,&v->do_join_ret,&v->ch,&v->ssid_len,
		    &v->expect_ret,&v->expect_disassoc,&v->expect_join,&v->expect_to_join,
		    &v->expect_assoc_ch,&v->expect_assoc_by_bssid,&v->expect_assoc_ssid_len};

	memset(v, 0, sizeof(*v));
	v->hw_init = -1;
	v->do_join_ret = -1;
	v->expect_assoc_ch = -1;
	v->expect_assoc_by_bssid = -1;
	v->expect_assoc_ssid_len = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_string_in(obj, len, "bssid", v->bssid, sizeof(v->bssid));
	host_json_parse_string_in(obj, len, "expect_bssid", v->expect_bssid,
				  sizeof(v->expect_bssid));
	for (int i = 0; k[i]; i++)
		host_json_parse_int_in(obj, len, k[i], p[i]);
	if (v->do_join_ret < 0)
		v->do_join_ret = 1;
	if (v->hw_init < 0)
		v->hw_init = 1;
	return 0;
}

static int mlme_ok(struct _adapter *a, struct vector *v)
{
	u8 exp[ETH_ALEN];

	if (v->expect_assoc_ch >= 0 && a->mlmepriv.assoc_ch != (u16)v->expect_assoc_ch)
		return 0;
	if (v->expect_assoc_by_bssid >= 0 &&
	    a->mlmepriv.assoc_by_bssid != (u8)v->expect_assoc_by_bssid)
		return 0;
	if (v->expect_assoc_ssid_len >= 0 &&
	    a->mlmepriv.assoc_ssid.SsidLength != (u32)v->expect_assoc_ssid_len)
		return 0;
	if (v->expect_bssid[0]) {
		if (dec_mac(v->expect_bssid, exp))
			return 0;
		if (memcmp(a->mlmepriv.assoc_bssid, exp, ETH_ALEN))
			return 0;
	}
	return 1;
}

static int run_vector(struct vector *v)
{
	struct _adapter a;
	u8 mac[ETH_ALEN], got;
	NDIS_802_11_SSID ssid;

	memset(&a, 0, sizeof(a));
#ifdef HOST_IOCTL_CONNECT_RUST
	ioctl_connect_test_reset_counters();
#else
	g_disassoc_calls = g_join_calls = 0;
#endif
	a.hw_init_done = (u8)v->hw_init;
	a.tkip_fail = (u8)v->tkip_fail;
	a.do_join_ret = (u8)v->do_join_ret;
	a.mlmepriv.fw_state = (u32)v->fw_state;

	if (!strcmp(v->fn, "disassociate")) {
		got = rtw_set_802_11_disassociate(&a);
		if (got != (u8)v->expect_ret ||
#ifdef HOST_IOCTL_CONNECT_RUST
		    ioctl_connect_test_disassoc_calls() != (u32)v->expect_disassoc)
#else
		    g_disassoc_calls != (u32)v->expect_disassoc)
#endif
			goto fail;
	} else if (!strcmp(v->fn, "connect")) {
		u8 *pb = NULL;

		memset(&ssid, 0, sizeof(ssid));
		ssid.SsidLength = (u32)v->ssid_len;
		if (v->bssid[0] && dec_mac(v->bssid, mac))
			goto fail;
		if (v->bssid[0])
			pb = mac;
		got = rtw_set_802_11_connect(&a, pb, v->ssid_len ? &ssid : NULL, (u16)v->ch);
		if (got != (u8)v->expect_ret ||
#ifdef HOST_IOCTL_CONNECT_RUST
		    ioctl_connect_test_join_calls() != (u32)v->expect_join ||
#else
		    g_join_calls != (u32)v->expect_join ||
#endif
		    a.mlmepriv.to_join != (u8)v->expect_to_join || !mlme_ok(&a, v))
			goto fail;
	} else if (!strcmp(v->fn, "set_bssid")) {
		if (!v->bssid[0] || dec_mac(v->bssid, mac))
			goto fail;
		got = rtw_set_802_11_bssid(&a, mac);
		if (got != (u8)v->expect_ret ||
#ifdef HOST_IOCTL_CONNECT_RUST
		    ioctl_connect_test_join_calls() != (u32)v->expect_join ||
#else
		    g_join_calls != (u32)v->expect_join ||
#endif
		    a.mlmepriv.to_join != (u8)v->expect_to_join || !mlme_ok(&a, v))
			goto fail;
	} else
		return 1;
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[24];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_connect_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 24, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
