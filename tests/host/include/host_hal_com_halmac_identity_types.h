/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_HALMAC_IDENTITY_TYPES_H
#define HOST_HAL_COM_HALMAC_IDENTITY_TYPES_H

#define RTW_HALMAC 1

#define HW_PORT0 0
#define HW_PORT1 1

typedef unsigned char u8;

struct dvobj_priv {
	u8 _pad;
};

struct _adapter {
	u8 hw_port;
	u8 clt_port;
	struct dvobj_priv *dvobj;
};

#define adapter_to_dvobj(a) ((a)->dvobj)

#define ETH_ALEN 6

struct host_halmac_identity_trace {
	u8 set_bssid_port;
	unsigned set_bssid_calls;
	u8 set_bssid_addr[ETH_ALEN];
	u8 set_bssid_addr_null;
	u8 set_mac_port;
	unsigned set_mac_calls;
	u8 set_mac_addr[ETH_ALEN];
	u8 get_mac_port;
	unsigned get_mac_calls;
	u8 get_mac_addr[ETH_ALEN];
};

extern struct host_halmac_identity_trace host_halmac_identity_trace;

void host_halmac_identity_reset(void);
void host_halmac_identity_set_hw_port(u8 port);
void host_halmac_identity_set_clt_port(u8 port);
struct _adapter *host_halmac_identity_adapter(void);
void host_halmac_identity_clear_trace(void);

int rtw_halmac_set_bssid(struct dvobj_priv *d, int hwport, u8 *addr);
int rtw_halmac_set_mac_address(struct dvobj_priv *d, int hwport, u8 *addr);
int rtw_halmac_get_mac_address(struct dvobj_priv *d, int hwport, u8 *addr);
void *_rtw_memset(void *s, int c, unsigned long n);

u8 rtw_hal_get_port(struct _adapter *adapter);

void rtw_hal_set_bssid_c_oracle(struct _adapter *adapter, u8 *val);
void rtw_hal_set_macaddr_port_c_oracle(struct _adapter *adapter, u8 *mac_addr);
void rtw_hal_get_macaddr_port_c_oracle(struct _adapter *adapter, u8 *mac_addr);

#endif