// SPDX-License-Identifier: GPL-2.0
/* MI / adapter extras for W4-128 on top of hal_com_rcr_shim.c */

#include "host_hal_com_rcr_chk_bssid_types.h"
#include <string.h>

struct _adapter_ext {
	struct registry_priv registrypriv;
	struct dvobj_priv *dvobj;
};

static struct _adapter_ext g_ext;
static struct dvobj_priv g_dvobj;
static struct mi_state g_mi_no_self;
static struct mi_state g_mi_no_others;
static u8 g_clt_num;

struct _adapter;

struct hal_data *host_hal_rcr_hal_data(struct _adapter *adapter);

struct hal_data *host_hal_rcr_chk_hal(struct _adapter *adapter)
{
	return host_hal_rcr_hal_data(adapter);
}

void host_hal_rcr_chk_reset(void)
{
	memset(&g_ext, 0, sizeof(g_ext));
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_mi_no_self, 0, sizeof(g_mi_no_self));
	memset(&g_mi_no_others, 0, sizeof(g_mi_no_others));
	g_ext.dvobj = &g_dvobj;
	g_clt_num = 0;
	host_hal_rcr_reset();
	host_hal_rcr_chk_hal(host_hal_rcr_chk_adapter())->rx_tsf_filter = 1;
}

struct _adapter *host_hal_rcr_chk_adapter(void)
{
	return host_hal_rcr_adapter();
}

void host_hal_rcr_chk_set_mi_no_self(const struct mi_state *s) { g_mi_no_self = *s; }
void host_hal_rcr_chk_set_mi_no_others(const struct mi_state *s) { g_mi_no_others = *s; }

void host_hal_rcr_chk_set_in_cta_test(u8 v)
{
	host_hal_rcr_chk_hal(host_hal_rcr_chk_adapter())->in_cta_test = v;
}

void host_hal_rcr_chk_set_wifi_spec(u8 v) { g_ext.registrypriv.wifi_spec = v; }
void host_hal_rcr_chk_set_periodic_tsf(u32 v) { g_dvobj.periodic_tsf_update_etime = v; }
void host_hal_rcr_chk_set_clt_num(u8 v) { g_clt_num = v; }

void host_hal_rcr_chk_set_initial_rcr(u32 rcr)
{
	host_hal_rcr_set_cached(rcr);
	host_hal_rcr_set_register(rcr);
}

u32 host_hal_rcr_chk_last_set_rcr(void) { return host_hal_rcr_trace.last_set_rcr; }

u8 host_hal_rcr_chk_wifi_spec(struct _adapter *adapter)
{
	(void)adapter;
	return g_ext.registrypriv.wifi_spec;
}

void rtw_mi_status_no_self(struct _adapter *a, struct mi_state *m)
{
	(void)a;
	*m = g_mi_no_self;
}

void rtw_mi_status_no_others(struct _adapter *a, struct mi_state *m)
{
	(void)a;
	*m = g_mi_no_others;
}

void rtw_mi_status_merge(struct mi_state *d, struct mi_state *a)
{
	d->sta_num += a->sta_num;
	d->ld_sta_num += a->ld_sta_num;
	d->lg_sta_num += a->lg_sta_num;
	d->ld_tdls_num += a->ld_tdls_num;
	d->ap_num += a->ap_num;
	d->scan_enter_num += a->scan_enter_num;
	d->scan_num += a->scan_num;
	d->mesh_num += a->mesh_num;
}

u8 get_clt_num(struct _adapter *a)
{
	(void)a;
	return g_clt_num;
}

void rtw_warn_on(int c) { (void)c; }

struct dvobj_priv *adapter_to_dvobj(struct _adapter *a)
{
	(void)a;
	return g_ext.dvobj;
}

void rtw_hal_tsf_update_pause(struct _adapter *a) { (void)a; }
void rtw_hal_tsf_update_restore(struct _adapter *a) { (void)a; }

int rtw_rust_hal_com_rcr_use_mbssid_shortcut(void)
{
#if defined(CONFIG_MI_WITH_MBSSID_CAM) && !defined(CONFIG_CLIENT_PORT_CFG)
	return 1;
#else
	return 0;
#endif
}

void rtw_rust_hal_com_mi_status_no_self(void *adapter, struct mi_state *m)
{
	rtw_mi_status_no_self(adapter, m);
}

void rtw_rust_hal_com_mi_status_no_others(void *adapter, struct mi_state *m)
{
	rtw_mi_status_no_others(adapter, m);
}

void rtw_rust_hal_com_mi_status_merge(struct mi_state *d, struct mi_state *a)
{
	rtw_mi_status_merge(d, a);
}

u8 rtw_rust_hal_com_in_cta_test(void *adapter)
{
	return GET_HAL_DATA(adapter)->in_cta_test;
}

u8 rtw_rust_hal_com_rx_tsf_filter(void *adapter)
{
	return GET_HAL_DATA(adapter)->rx_tsf_filter;
}

u32 rtw_rust_hal_com_periodic_tsf_update_etime(void *adapter)
{
	(void)adapter;
	return adapter_to_dvobj(adapter)->periodic_tsf_update_etime;
}

u8 rtw_rust_hal_com_wifi_spec(void *adapter)
{
	(void)adapter;
	return host_hal_rcr_chk_wifi_spec(adapter);
}

u8 rtw_rust_hal_com_get_clt_num(void *adapter)
{
	return get_clt_num(adapter);
}

void rtw_rust_hal_com_warn_on(int c)
{
	rtw_warn_on(c);
}

void rtw_rust_hal_com_tsf_update_pause_for_rcr(void *adapter)
{
	rtw_hal_tsf_update_pause(adapter);
}

void rtw_rust_hal_com_tsf_update_restore_for_rcr(void *adapter)
{
	rtw_hal_tsf_update_restore(adapter);
}