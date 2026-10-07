// SPDX-License-Identifier: GPL-2.0
/* W3-125 follow-up (#982) PR12+: L2 oracle for infrastructure_mode setter. */
#include <stdio.h>
#include <string.h>
#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long _irqL;

typedef enum {
	Ndis802_11IBSS,
	Ndis802_11Infrastructure,
	Ndis802_11AutoUnknown,
	Ndis802_11InfrastructureMax,
	Ndis802_11APMode,
} NDIS_802_11_NETWORK_INFRASTRUCTURE;

#define WIFI_NULL_STATE 0u
#define WIFI_ASOC_STATE 0x00000001u
#define WIFI_STATION_STATE 0x00000008u
#define WIFI_AP_STATE 0x00000010u
#define WIFI_ADHOC_STATE 0x00000020u
#define WIFI_ADHOC_MASTER_STATE 0x00000040u

struct wlan_network { NDIS_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode; };
struct wlan_network_cur { struct wlan_network network; int join_res; };
struct mlme_priv { u32 fw_state; int lock_depth; struct wlan_network_cur cur_network; };
struct adapter { struct mlme_priv mlmepriv; };

static u32 g_stop_ap, g_start_ap, g_disassoc, g_free_res, g_disconnect, g_init_bcmc;

#ifdef HOST_IOCTL_INFRA_MODE_RUST
extern u8 rtw_set_802_11_infrastructure_mode_rust(struct adapter *, NDIS_802_11_NETWORK_INFRASTRUCTURE, u8);
extern void ioctl_infra_mode_test_reset_counters(void);
extern u32 ioctl_infra_mode_test_stop_ap_calls(void);
extern u32 ioctl_infra_mode_test_start_ap_calls(void);
extern u32 ioctl_infra_mode_test_disassoc_calls(void);
extern u32 ioctl_infra_mode_test_free_res_calls(void);
extern u32 ioctl_infra_mode_test_disconnect_calls(void);
extern u32 ioctl_infra_mode_test_init_bcmc_calls(void);
#define INFRA_FN rtw_set_802_11_infrastructure_mode_rust
#else
#define INFRA_FN host_set_infra_mode
static void host_enter(int *d, _irqL *i) { (*d)++; (void)i; }
static void host_exit(int *d, _irqL *i) { (*d)--; (void)i; }
static u8 host_chk(u32 fw, u32 b) { return (fw & b) ? _TRUE : _FALSE; }

static u8 host_set_infra_mode(struct adapter *p, NDIS_802_11_NETWORK_INFRASTRUCTURE nt, u8 flags)
{
	_irqL irqL;
	struct mlme_priv *m = &p->mlmepriv;
	NDIS_802_11_NETWORK_INFRASTRUCTURE *old = &m->cur_network.network.InfrastructureMode;
	u8 ap2sta = _FALSE, ret = _TRUE, linked, adhoc_m;

	if (*old == nt)
		return _TRUE;
	if (*old == Ndis802_11APMode) {
		m->cur_network.join_res = -1;
		ap2sta = _TRUE;
		g_stop_ap++;
	}
	host_enter(&m->lock_depth, &irqL);
	linked = host_chk(m->fw_state, WIFI_ASOC_STATE);
	adhoc_m = host_chk(m->fw_state, WIFI_ADHOC_MASTER_STATE);
	if (flags)
		host_exit(&m->lock_depth, &irqL);
	if (linked || *old == Ndis802_11IBSS) {
		g_disassoc++;
		(void)p;
		(void)flags;
	}
	if (linked || adhoc_m)
		g_free_res++;
	if ((*old == Ndis802_11Infrastructure || *old == Ndis802_11IBSS) && linked)
		g_disconnect++;
	if (flags)
		host_enter(&m->lock_depth, &irqL);
	*old = nt;
	m->fw_state = WIFI_NULL_STATE;
	switch (nt) {
	case Ndis802_11IBSS: m->fw_state |= WIFI_ADHOC_STATE; break;
	case Ndis802_11Infrastructure:
		m->fw_state |= WIFI_STATION_STATE;
		if (ap2sta)
			g_init_bcmc++;
		break;
	case Ndis802_11APMode:
		m->fw_state |= WIFI_AP_STATE;
		g_start_ap++;
		break;
	case Ndis802_11AutoUnknown:
	case Ndis802_11InfrastructureMax:
		break;
	default:
		ret = _FALSE;
	}
	host_exit(&m->lock_depth, &irqL);
	return ret;
}
#endif

struct vector {
	char name[64];
	int old_mode, new_mode, flags, fw_state, join_res;
	int expect_ret, expect_mode, expect_fw_state, expect_join_res;
	int expect_stop_ap, expect_start_ap, expect_disassoc, expect_free_res;
	int expect_disconnect, expect_init_bcmc;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "old_mode", &v->old_mode);
	host_json_parse_int_in(obj, len, "new_mode", &v->new_mode);
	host_json_parse_int_in(obj, len, "flags", &v->flags);
	host_json_parse_int_in(obj, len, "fw_state", &v->fw_state);
	host_json_parse_int_in(obj, len, "join_res", &v->join_res);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_mode", &v->expect_mode);
	host_json_parse_int_in(obj, len, "expect_fw_state", &v->expect_fw_state);
	host_json_parse_int_in(obj, len, "expect_join_res", &v->expect_join_res);
	host_json_parse_int_in(obj, len, "expect_stop_ap", &v->expect_stop_ap);
	host_json_parse_int_in(obj, len, "expect_start_ap", &v->expect_start_ap);
	host_json_parse_int_in(obj, len, "expect_disassoc", &v->expect_disassoc);
	host_json_parse_int_in(obj, len, "expect_free_res", &v->expect_free_res);
	host_json_parse_int_in(obj, len, "expect_disconnect", &v->expect_disconnect);
	host_json_parse_int_in(obj, len, "expect_init_bcmc", &v->expect_init_bcmc);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct adapter a;
	u8 ret;
	u32 stop_ap, start_ap, disassoc, free_res, disconnect, init_bcmc;

	memset(&a, 0, sizeof(a));
	g_stop_ap = g_start_ap = g_disassoc = g_free_res = g_disconnect = g_init_bcmc = 0;
#ifdef HOST_IOCTL_INFRA_MODE_RUST
	ioctl_infra_mode_test_reset_counters();
#endif
	a.mlmepriv.fw_state = (u32)v->fw_state;
	a.mlmepriv.cur_network.network.InfrastructureMode = (NDIS_802_11_NETWORK_INFRASTRUCTURE)v->old_mode;
	a.mlmepriv.cur_network.join_res = v->join_res;
	ret = INFRA_FN(&a, (NDIS_802_11_NETWORK_INFRASTRUCTURE)v->new_mode, (u8)v->flags);
	if (ret != (u8)v->expect_ret ||
	    (int)a.mlmepriv.cur_network.network.InfrastructureMode != v->expect_mode ||
	    (int)a.mlmepriv.fw_state != v->expect_fw_state ||
	    a.mlmepriv.cur_network.join_res != v->expect_join_res) {
		fprintf(stderr, "FAIL %s: state mismatch\n", v->name);
		return 1;
	}
#ifdef HOST_IOCTL_INFRA_MODE_RUST
	stop_ap = ioctl_infra_mode_test_stop_ap_calls();
	start_ap = ioctl_infra_mode_test_start_ap_calls();
	disassoc = ioctl_infra_mode_test_disassoc_calls();
	free_res = ioctl_infra_mode_test_free_res_calls();
	disconnect = ioctl_infra_mode_test_disconnect_calls();
	init_bcmc = ioctl_infra_mode_test_init_bcmc_calls();
#else
	stop_ap = g_stop_ap;
	start_ap = g_start_ap;
	disassoc = g_disassoc;
	free_res = g_free_res;
	disconnect = g_disconnect;
	init_bcmc = g_init_bcmc;
#endif
	if ((int)stop_ap != v->expect_stop_ap || (int)start_ap != v->expect_start_ap ||
	    (int)disassoc != v->expect_disassoc || (int)free_res != v->expect_free_res ||
	    (int)disconnect != v->expect_disconnect || (int)init_bcmc != v->expect_init_bcmc) {
		fprintf(stderr, "FAIL %s: side effects\n", v->name);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_infra_mode_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
