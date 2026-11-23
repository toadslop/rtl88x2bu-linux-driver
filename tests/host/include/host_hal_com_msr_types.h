/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_MSR_TYPES_H
#define HOST_HAL_COM_MSR_TYPES_H

#define RTW_HALMAC 1
#define CONFIG_MI_WITH_MBSSID_CAM 1
#define CONFIG_MBSSID_CAM 1

#define HW_PORT0 0
#define HW_PORT1 1

#define _HW_STATE_NOLINK_ 0x00
#define _HW_STATE_STATION_ 0x02
#define _HW_STATE_AP_ 0x03

typedef unsigned char u8;

struct dvobj_priv {
	u8 _pad;
};
struct _adapter {
	u8 hw_port;
	struct dvobj_priv *dvobj;
};

#define adapter_to_dvobj(a) ((a)->dvobj)

void host_msr_reset_trace(void);
void host_msr_set_hw_port(u8 port);
void host_msr_set_mbid_entry_num(u8 n);
struct _adapter *host_msr_adapter(void);

u8 host_msr_last_get_port(void);
u8 host_msr_last_set_port(void);
u8 host_msr_last_set_type(void);
u8 host_msr_last_get_type(void);
void host_msr_clear_calls(void);

int rtw_halmac_get_network_type(struct dvobj_priv *d, int hwport, u8 *type);
int rtw_halmac_set_network_type(struct dvobj_priv *d, int hwport, u8 type);

u8 rtw_get_mbid_cam_entry_num(struct _adapter *adapter);

void rtw_hal_get_msr(struct _adapter *adapter, u8 *net_type);
void rtw_hal_set_msr(struct _adapter *adapter, u8 net_type);

void rtw_hal_get_msr_c_oracle(struct _adapter *adapter, u8 *net_type);
void rtw_hal_set_msr_c_oracle(struct _adapter *adapter, u8 net_type);

#endif