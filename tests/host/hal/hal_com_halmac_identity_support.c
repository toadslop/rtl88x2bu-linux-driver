// SPDX-License-Identifier: GPL-2.0
/* Shims + C oracle for W4-135 set_bssid HALMAC L2 (PR1). */

#include <string.h>
#include "host_hal_com_halmac_identity_types.h"

struct host_halmac_identity_trace host_halmac_identity_trace;

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapter;

void host_halmac_identity_reset(void)
{
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&host_halmac_identity_trace, 0, sizeof(host_halmac_identity_trace));
	g_adapter.dvobj = &g_dvobj;
}

void host_halmac_identity_set_hw_port(u8 port) { g_adapter.hw_port = port; }
void host_halmac_identity_set_clt_port(u8 port) { g_adapter.clt_port = port; }
struct _adapter *host_halmac_identity_adapter(void) { return &g_adapter; }

void host_halmac_identity_clear_trace(void)
{
	host_halmac_identity_trace.set_bssid_port = 0xff;
	host_halmac_identity_trace.set_bssid_calls = 0;
	host_halmac_identity_trace.set_bssid_addr_null = 0;
	memset(host_halmac_identity_trace.set_bssid_addr, 0,
	       sizeof(host_halmac_identity_trace.set_bssid_addr));
	host_halmac_identity_trace.set_mac_port = 0xff;
	host_halmac_identity_trace.set_mac_calls = 0;
	memset(host_halmac_identity_trace.set_mac_addr, 0,
	       sizeof(host_halmac_identity_trace.set_mac_addr));
	host_halmac_identity_trace.get_mac_port = 0xff;
	host_halmac_identity_trace.get_mac_calls = 0;
	memset(host_halmac_identity_trace.get_mac_addr, 0,
	       sizeof(host_halmac_identity_trace.get_mac_addr));
}

u8 rtw_hal_get_port(struct _adapter *adapter)
{
	u8 hw_port = adapter->hw_port;

	if (adapter->clt_port)
		hw_port = adapter->clt_port;
	return hw_port;
}

int rtw_halmac_set_bssid(struct dvobj_priv *d, int hwport, u8 *addr)
{
	(void)d;

	host_halmac_identity_trace.set_bssid_port = (u8)hwport;
	host_halmac_identity_trace.set_bssid_calls++;
	if (!addr) {
		host_halmac_identity_trace.set_bssid_addr_null = 1;
		memset(host_halmac_identity_trace.set_bssid_addr, 0,
		       sizeof(host_halmac_identity_trace.set_bssid_addr));
	} else {
		host_halmac_identity_trace.set_bssid_addr_null = 0;
		memcpy(host_halmac_identity_trace.set_bssid_addr, addr, ETH_ALEN);
	}
	return 0;
}

int rtw_halmac_set_mac_address(struct dvobj_priv *d, int hwport, u8 *addr)
{
	(void)d;

	host_halmac_identity_trace.set_mac_port = (u8)hwport;
	host_halmac_identity_trace.set_mac_calls++;
	if (addr)
		memcpy(host_halmac_identity_trace.set_mac_addr, addr, ETH_ALEN);
	return 0;
}

int rtw_halmac_get_mac_address(struct dvobj_priv *d, int hwport, u8 *addr)
{
	static const u8 stored[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

	(void)d;

	host_halmac_identity_trace.get_mac_port = (u8)hwport;
	host_halmac_identity_trace.get_mac_calls++;
	if (addr)
		memcpy(addr, stored, ETH_ALEN);
	return 0;
}

void *_rtw_memset(void *s, int c, unsigned long n)
{
	return memset(s, c, n);
}

void rtw_hal_set_bssid_c_oracle(struct _adapter *adapter, u8 *val)
{
	u8 hw_port = rtw_hal_get_port(adapter);

#ifdef RTW_HALMAC
	rtw_halmac_set_bssid(adapter_to_dvobj(adapter), hw_port, val);
#else
	(void)adapter;
	(void)val;
#endif
}

void rtw_hal_set_macaddr_port_c_oracle(struct _adapter *adapter, u8 *mac_addr)
{
	u8 hwport = adapter->hw_port;

	if (!mac_addr)
		return;
#ifdef RTW_HALMAC
	rtw_halmac_set_mac_address(adapter_to_dvobj(adapter), hwport, mac_addr);
#else
	(void)adapter;
	(void)mac_addr;
#endif
}

void rtw_hal_get_macaddr_port_c_oracle(struct _adapter *adapter, u8 *mac_addr)
{
	u8 hwport = adapter->hw_port;

	if (!mac_addr)
		return;
	_rtw_memset(mac_addr, 0, ETH_ALEN);
#ifdef RTW_HALMAC
	rtw_halmac_get_mac_address(adapter_to_dvobj(adapter), hwport, mac_addr);
#else
	(void)adapter;
#endif
}