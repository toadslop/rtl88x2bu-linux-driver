// SPDX-License-Identifier: GPL-2.0
/* W3-65 host L2 oracle for update_network / update_current_network. */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define MAX_IE_SZ 768
#define WIFI_ASOC_STATE 0x00000001
#define WLAN_CAPABILITY_BSS (1 << 0)
#define WLAN_CAPABILITY_IBSS (1 << 1)

typedef int sint;
typedef unsigned int uint;

typedef struct {
	u8 SignalStrength, SignalQuality, Optimum_antenna, is_cck_rate;
	s8 rx_snr[4];
} WLAN_PHY_INFO;

typedef struct {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

typedef struct {
	u8 Timestamp[8];
	u16 BeaconInterval, Capabilities;
} NDIS_802_11_FIXED_IEs;

typedef struct {
	u32 Length;
	u8 MacAddress[ETH_ALEN], Reserved[2];
	NDIS_802_11_SSID Ssid, mesh_id;
	u32 Privacy;
	long Rssi;
	u32 cfg_len, cfg_bp, cfg_atim, cfg_ds;
	u32 InfrastructureMode;
	u8 SupportedRates[16];
	WLAN_PHY_INFO PhyInfo;
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

struct wlan_network { WLAN_BSSID_EX network; };
struct mlme_priv { sint fw_state; struct wlan_network cur_network; };
struct recv_priv { u8 signal_strength, signal_qual; };
struct _adapter { struct mlme_priv mlmepriv; struct recv_priv recvpriv; };
typedef struct _adapter _adapter;

static inline uint get_WLAN_BSSID_EX_sz(WLAN_BSSID_EX *b)
{
	return (uint)(sizeof(*b) - MAX_IE_SZ + b->IELength);
}

static inline sint check_fwstate(struct mlme_priv *m, sint st)
{
	return (m->fw_state & st) ? _TRUE : _FALSE;
}

static int host_protection_calls;
int _rtw_memcmp(const void *a, const void *b, size_t n)
{
	return memcmp(a, b, n) == 0 ? _TRUE : _FALSE;
}

int rtw_bug_check(void *p1, void *p2, void *p3, void *p4)
{
	(void)p1; (void)p2; (void)p3; (void)p4; return _TRUE;
}

int is_all_null(char *c, int len)
{
	for (; len > 0; len--)
		if (c[len - 1] != '\0') return _FALSE;
	return _TRUE;
}

void rtw_update_protection(_adapter *a, u8 *ie, uint len)
{
	(void)a; (void)ie; (void)len;
	host_protection_calls++;
}

int is_same_network(WLAN_BSSID_EX *src, WLAN_BSSID_EX *dst, u8 feature)
{
	u16 s_cap, d_cap;

	(void)feature;
	_rtw_memcpy((u8 *)&s_cap, src->IEs + 10, 2);
	_rtw_memcpy((u8 *)&d_cap, dst->IEs + 10, 2);
	if (_rtw_memcmp(src->MacAddress, dst->MacAddress, ETH_ALEN) == _TRUE &&
	    ((s_cap & WLAN_CAPABILITY_IBSS) == (d_cap & WLAN_CAPABILITY_IBSS)) &&
	    ((s_cap & WLAN_CAPABILITY_BSS) == (d_cap & WLAN_CAPABILITY_BSS))) {
		if (src->Ssid.SsidLength == dst->Ssid.SsidLength &&
		    (_rtw_memcmp(src->Ssid.Ssid, dst->Ssid.Ssid, src->Ssid.SsidLength) == _TRUE ||
		     is_all_null((char *)src->Ssid.Ssid, src->Ssid.SsidLength) == _TRUE ||
		     is_all_null((char *)dst->Ssid.Ssid, dst->Ssid.SsidLength) == _TRUE))
			return _TRUE;
		if (src->Ssid.SsidLength == 0 || dst->Ssid.SsidLength == 0)
			return _TRUE;
	}
	return _FALSE;
}

void update_network(WLAN_BSSID_EX *dst, WLAN_BSSID_EX *src, _adapter *a, bool update_ie)
{
	long rssi_ori = dst->Rssi;
	u8 sq_smp = src->PhyInfo.SignalQuality, ss, sq;
	long rssi;

	if (check_fwstate(&a->mlmepriv, WIFI_ASOC_STATE) &&
	    is_same_network(&a->mlmepriv.cur_network.network, src, 0)) {
		ss = a->recvpriv.signal_strength;
		sq = a->recvpriv.signal_qual;
		rssi = (sq_smp != 101) ? (src->Rssi + dst->Rssi * 4) / 5 : rssi_ori;
	} else if (sq_smp != 101) {
		ss = ((u32)src->PhyInfo.SignalStrength + (u32)dst->PhyInfo.SignalStrength * 4) / 5;
		sq = ((u32)src->PhyInfo.SignalQuality + (u32)dst->PhyInfo.SignalQuality * 4) / 5;
		rssi = (src->Rssi + dst->Rssi * 4) / 5;
	} else {
		ss = dst->PhyInfo.SignalStrength;
		sq = dst->PhyInfo.SignalQuality;
		rssi = dst->Rssi;
	}
	if (update_ie) {
		dst->Reserved[0] = src->Reserved[0];
		dst->Reserved[1] = src->Reserved[1];
		_rtw_memcpy(dst, src, get_WLAN_BSSID_EX_sz(src));
	}
	dst->PhyInfo.SignalStrength = ss;
	dst->PhyInfo.SignalQuality = sq;
	dst->Rssi = rssi;
}

void update_current_network(_adapter *a, WLAN_BSSID_EX *pn)
{
	struct mlme_priv *m = &a->mlmepriv;

	rtw_bug_check(&m->cur_network.network, &m->cur_network.network,
		      &m->cur_network.network, &m->cur_network.network);
	if (check_fwstate(m, WIFI_ASOC_STATE) == _TRUE &&
	    is_same_network(&m->cur_network.network, pn, 0)) {
		update_network(&m->cur_network.network, pn, a, true);
		rtw_update_protection(a, m->cur_network.network.IEs + sizeof(NDIS_802_11_FIXED_IEs),
				      m->cur_network.network.IELength);
	}
}

struct case_vec {
	const char *name;
	int fn, dst_ss, dst_sq, dst_rssi, src_ss, src_sq, src_rssi;
	int adapter_ss, adapter_sq, assoc, same, update_ie;
	int expect_ss, expect_sq, expect_rssi, expect_prot;
	const char *src_ssid, *expect_ssid;
};

static void fill_bss(WLAN_BSSID_EX *b, int ss, int sq, int rssi, const char *ssid)
{
	u8 ie[12] = {0};

	memset(b, 0, sizeof(*b));
	b->IELength = 12;
	b->PhyInfo.SignalStrength = (u8)ss;
	b->PhyInfo.SignalQuality = (u8)sq;
	b->Rssi = rssi;
	ie[10] = WLAN_CAPABILITY_BSS;
	memcpy(b->IEs, ie, sizeof(ie));
	memset(b->MacAddress, 0xaa, ETH_ALEN);
	if (ssid && ssid[0]) {
		b->Ssid.SsidLength = (u32)strlen(ssid);
		memcpy(b->Ssid.Ssid, ssid, b->Ssid.SsidLength);
	}
}

static int run_case(const struct case_vec *v)
{
	_adapter a;
	WLAN_BSSID_EX dst, src;

	memset(&a, 0, sizeof(a));
	host_protection_calls = 0;
	fill_bss(&dst, v->dst_ss, v->dst_sq, v->dst_rssi, "dst-ap");
	fill_bss(&src, v->src_ss, v->src_sq, v->src_rssi,
		 v->src_ssid ? v->src_ssid : (v->same ? "dst-ap" : "other"));
	fill_bss(&a.mlmepriv.cur_network.network, v->dst_ss, v->dst_sq, v->dst_rssi, "dst-ap");
	if (v->assoc)
		a.mlmepriv.fw_state = WIFI_ASOC_STATE;
	a.recvpriv.signal_strength = (u8)v->adapter_ss;
	a.recvpriv.signal_qual = (u8)v->adapter_sq;
	if (v->fn == 0)
		update_network(&dst, &src, &a, v->update_ie ? true : false);
	else
		update_current_network(&a, &src);
	if (v->fn == 0) {
		if ((int)dst.PhyInfo.SignalStrength != v->expect_ss ||
		    (int)dst.PhyInfo.SignalQuality != v->expect_sq ||
		    (int)dst.Rssi != v->expect_rssi)
			return -1;
		if (v->expect_ssid &&
		    strncmp((char *)dst.Ssid.Ssid, v->expect_ssid, dst.Ssid.SsidLength))
			return -1;
		return 0;
	}
	if ((int)a.mlmepriv.cur_network.network.PhyInfo.SignalStrength != v->expect_ss ||
	    (int)a.mlmepriv.cur_network.network.PhyInfo.SignalQuality != v->expect_sq ||
	    (int)a.mlmepriv.cur_network.network.Rssi != v->expect_rssi ||
	    host_protection_calls != v->expect_prot)
		return -1;
	return 0;
}

int main(void)
{
	static const struct case_vec cases[] = {
		{"avg_signal", 0, 10, 20, -60, 30, 40, -50, 0, 0, 0, 0, 0, 14, 24, -58, 0, NULL, NULL},
		{"wrong_channel", 0, 10, 20, -60, 30, 101, -50, 0, 0, 0, 0, 0, 10, 20, -60, 0, NULL, NULL},
		{"assoc_wrong_channel", 0, 10, 20, -60, 30, 101, -50, 88, 77, 1, 1, 0, 88, 77, -60, 0, NULL, NULL},
		{"assoc_same", 0, 10, 20, -60, 30, 40, -50, 88, 77, 1, 1, 0, 88, 77, -58, 0, NULL, NULL},
		{"update_ie", 0, 10, 40, -60, 50, 40, -50, 0, 0, 0, 0, 1, 18, 40, -58, 0, "new-ssid", "new-ssid"},
		{"update_current", 1, 10, 20, -60, 30, 40, -50, 55, 66, 1, 1, 0, 55, 66, -58, 1, NULL, NULL},
		{"skip_not_assoc", 1, 10, 20, -60, 30, 40, -50, 0, 0, 0, 0, 0, 10, 20, -60, 0, NULL, NULL},
		{"skip_diff_network", 1, 10, 20, -60, 30, 40, -50, 55, 66, 1, 0, 0, 10, 20, -60, 0, "other", NULL},
	};
	int bad = 0;

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
		bad += run_case(&cases[i]) ?
			(fprintf(stderr, "FAIL %s\n", cases[i].name), 1) :
			(printf("PASS %s\n", cases[i].name), 0);
	if (!bad)
		printf("PASS %zu vectors (W3-65 host oracle)\n", sizeof(cases) / sizeof(cases[0]));
	return bad ? 1 : 0;
}
