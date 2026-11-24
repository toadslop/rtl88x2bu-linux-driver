/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_TSF_TYPES_H
#define HOST_HAL_COM_TSF_TYPES_H

typedef unsigned char u8;
typedef unsigned int u32;
typedef u32 systime;

struct mlme_ext_priv {
	u8 tsf_update_required;
	u8 en_hw_update_tsf;
	systime tsf_update_pause_stime;
};

struct _adapter {
	struct mlme_ext_priv mlmeextpriv;
};

struct host_hal_com_tsf_trace {
	unsigned set_tsf_update_count;
	u8 last_set_tsf_en;
};

extern struct host_hal_com_tsf_trace host_hal_com_tsf_trace;

void host_hal_com_tsf_reset(void);
struct _adapter *host_hal_com_tsf_adapter(void);

void rtw_rust_hal_com_tsf_set_pause_stime(void *adapter, systime stime);
void rtw_rust_hal_com_tsf_set_update_required(void *adapter, u8 v);
void rtw_rust_hal_com_tsf_set_en_hw_update(void *adapter, u8 v);
u8 rtw_rust_hal_com_tsf_update_required(void *adapter);
u8 rtw_rust_hal_com_tsf_en_hw_update(void *adapter);
systime rtw_rust_hal_com_tsf_pause_stime(void *adapter);
void rtw_rust_hal_com_tsf_hal_set_tsf_update(void *adapter, u8 en);

void rtw_iface_enable_tsf_update(struct _adapter *adapter);
void rtw_iface_disable_tsf_update(struct _adapter *adapter);

#endif