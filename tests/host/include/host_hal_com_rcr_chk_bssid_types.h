/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_RCR_CHK_BSSID_TYPES_H
#define HOST_HAL_COM_RCR_CHK_BSSID_TYPES_H

#include "host_hal_com_rcr_types.h"

#define RCR_CBSSID_BCN  (1u << 7)
#define RCR_CBSSID_DATA (1u << 6)
#define RCR_AAP         (1u << 0)

#define UNASOC_STA_SRC_RX_NMY_UC 1

#define MAX_CLIENT_PORT_NUM 4

/* From include/rtw_mlme.h — sequential MLME notify codes. */
enum {
	MLME_ACTION_UNKNOWN,
	MLME_ACTION_NONE,
	MLME_SCAN_ENABLE,
	MLME_SCAN_ENTER,
	MLME_SCAN_DONE,
	MLME_SCAN_DISABLE,
	MLME_STA_CONNECTING,
	MLME_STA_CONNECTED,
	MLME_STA_DISCONNECTED,
	MLME_TDLS_LINKED,
	MLME_TDLS_NOLINK,
	MLME_AP_STARTED,
	MLME_AP_STOPPED,
	MLME_ADHOC_STARTED,
	MLME_ADHOC_STOPPED,
	MLME_MESH_STARTED,
	MLME_MESH_STOPPED,
	MLME_OPCH_SWITCH,
};

struct mi_state {
	u8 sta_num;
	u8 ld_sta_num;
	u8 lg_sta_num;
	u8 ld_tdls_num;
	u8 ap_num;
	u8 starting_ap_num;
	u8 ld_ap_num;
	u8 adhoc_num;
	u8 ld_adhoc_num;
	u8 mesh_num;
	u8 ld_mesh_num;
	u8 scan_num;
	u8 scan_enter_num;
	u8 uwps_num;
};

struct registry_priv {
	u8 wifi_spec;
};

struct dvobj_priv {
	unsigned long periodic_tsf_update_etime;
};

struct _adapter;

struct hal_data *host_hal_rcr_hal_data(struct _adapter *adapter);
u32 host_hal_rcr_receive_config(struct _adapter *adapter);

#define GET_HAL_DATA(adapter) (host_hal_rcr_hal_data(adapter))

void host_hal_rcr_reset(void);
void host_hal_rcr_set_cached(u32 rcr);
void host_hal_rcr_set_register(u32 rcr);
struct _adapter *host_hal_rcr_adapter(void);

void host_hal_rcr_chk_reset(void);
struct _adapter *host_hal_rcr_chk_adapter(void);
void host_hal_rcr_chk_set_initial_rcr(u32 rcr);
void host_hal_rcr_chk_set_mi_no_self(const struct mi_state *s);
void host_hal_rcr_chk_set_mi_no_others(const struct mi_state *s);
void host_hal_rcr_chk_set_in_cta_test(u8 v);
void host_hal_rcr_chk_set_wifi_spec(u8 v);
void host_hal_rcr_chk_set_periodic_tsf(unsigned long v);
void host_hal_rcr_chk_set_clt_num(u8 v);
void host_hal_rcr_chk_set_rx_tsf_filter(u8 v);
void host_hal_rcr_chk_set_unassoc_nmy_uc(u8 v);

u8 rtw_unassoc_sta_src_chk(struct _adapter *adapter, u8 stype);
u8 host_hal_rcr_chk_wifi_spec(struct _adapter *adapter);

u32 host_hal_rcr_chk_last_set_rcr(void);

extern struct host_hal_rcr_trace host_hal_rcr_trace;

void rtw_hal_rcr_set_chk_bssid(struct _adapter *adapter, u8 self_action);
void rtw_hal_rcr_set_chk_bssid_act_non(struct _adapter *adapter);

#ifdef RUST_HAL_COM_RCR_CHK_BSSID_ORACLE
void rtw_hal_rcr_set_chk_bssid_c_oracle(struct _adapter *adapter, u8 self_action);
void rtw_hal_rcr_set_chk_bssid_act_non_c_oracle(struct _adapter *adapter);
#endif

#endif