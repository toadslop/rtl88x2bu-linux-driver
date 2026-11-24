// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-131 default port H2C L2 harness. */

#include <string.h>
#include "host_hal_com_dft_port_types.h"

struct host_dvobj_dft {
	u8 port_id;
	u8 mac_id;
};

struct host_dft_state {
	struct host_dvobj_dft dvobj;
	u8 hal_port;
	u8 client_assoc;
	u8 ap_state;
	u8 assoc_mac_id;
	u8 assoc_sta_found;
};

static struct host_dft_state g_state;
static struct h2c_record {
	u8 cmd;
	unsigned int len;
	u8 buf[8];
} g_h2c[8];
static int g_h2c_count;
static s32 g_fill_ret = _SUCCESS;

static char g_adapter_blob[1];

void host_dft_port_reset(void)
{
	memset(&g_state, 0, sizeof(g_state));
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = _SUCCESS;
}

void host_dft_port_set_fill_ret(s32 v)
{
	g_fill_ret = v;
}

void host_dft_port_set_hal_port(u8 v)
{
	g_state.hal_port = v;
}

void host_dft_port_set_cached(u8 port_id, u8 mac_id)
{
	g_state.dvobj.port_id = port_id;
	g_state.dvobj.mac_id = mac_id;
}

void host_dft_port_set_client_assoc(u8 v, u8 mac_id)
{
	g_state.client_assoc = v;
	g_state.assoc_mac_id = mac_id;
}

void host_dft_port_set_assoc_sta_found(u8 v)
{
	g_state.assoc_sta_found = v;
}

void host_dft_port_set_ap_state(u8 v)
{
	g_state.ap_state = v;
}

int host_dft_port_h2c_count(void)
{
	return g_h2c_count;
}

u8 host_dft_port_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].cmd;
}

unsigned int host_dft_port_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].len;
}

const u8 *host_dft_port_h2c_buf(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return NULL;
	return g_h2c[idx].buf;
}

u8 host_dft_port_dft_port(void)
{
	return g_state.dvobj.port_id;
}

u8 host_dft_port_dft_mac(void)
{
	return g_state.dvobj.mac_id;
}

struct _adapter *host_dft_port_adapter(void)
{
	return (struct _adapter *)g_adapter_blob;
}

u8 rtw_hal_get_port(struct _adapter *adapter)
{
	(void)adapter;
	return g_state.hal_port;
}

s32 rtw_hal_fill_h2c_cmd(struct _adapter *adapter, u8 cmd, unsigned int len, u8 *buf)
{
	(void)adapter;
	if (g_h2c_count >= 8)
		return 0;
	g_h2c[g_h2c_count].cmd = cmd;
	g_h2c[g_h2c_count].len = len;
	if (len > sizeof(g_h2c[0].buf))
		len = sizeof(g_h2c[0].buf);
	memcpy(g_h2c[g_h2c_count].buf, buf, len);
	g_h2c_count++;
	return g_fill_ret;
}

struct dvobj_priv *rtw_rust_hal_com_adapter_to_dvobj(struct _adapter *adapter)
{
	(void)adapter;
	return (struct dvobj_priv *)&g_state.dvobj;
}

u8 rtw_rust_hal_com_dft_port_id(struct dvobj_priv *dvobj)
{
	return ((struct host_dvobj_dft *)dvobj)->port_id;
}

u8 rtw_rust_hal_com_dft_mac_id(struct dvobj_priv *dvobj)
{
	return ((struct host_dvobj_dft *)dvobj)->mac_id;
}

void rtw_rust_hal_com_dft_set(struct dvobj_priv *dvobj, u8 port_id, u8 mac_id)
{
	struct host_dvobj_dft *d = (struct host_dvobj_dft *)dvobj;

	d->port_id = port_id;
	d->mac_id = mac_id;
}

s32 rtw_rust_hal_com_is_client_associated_to_ap(struct _adapter *adapter)
{
	(void)adapter;
	return g_state.client_assoc ? 1 : 0;
}

u8 rtw_rust_hal_com_mlmepriv_is_ap(struct _adapter *adapter)
{
	(void)adapter;
	return g_state.ap_state;
}

u8 rtw_rust_hal_com_assoc_sta_mac_id(struct _adapter *adapter, u8 *found)
{
	(void)adapter;
	*found = g_state.assoc_sta_found;
	return g_state.assoc_mac_id;
}

void rtw_rust_hal_com_log_dft_port_h2c(struct _adapter *adapter, const char *func, u8 *parm,
				     u8 port_id, u8 mac_id)
{
	(void)adapter;
	(void)func;
	(void)parm;
	(void)port_id;
	(void)mac_id;
}