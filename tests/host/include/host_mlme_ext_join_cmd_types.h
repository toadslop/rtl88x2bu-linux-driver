/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_EXT_JOIN_CMD_TYPES_H
#define HOST_MLME_EXT_JOIN_CMD_TYPES_H

#include "host_types.h"
#include "host_autoconf.h"

#define MAX_IE_SZ 768

#include <stddef.h>
#include <stdbool.h>

#define ETH_ALEN 6

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define H2C_SUCCESS 0x00
#define H2C_PARAMETERS_ERROR 0x04

#define _HW_STATE_STATION_ 0x02
#define WIFI_FW_NULL_STATE 0
#define WIFI_FW_STATION_STATE _HW_STATE_STATION_
#define WIFI_FW_ASSOC_SUCCESS 0x00004000
#define WIFI_STATION_STATE 0x00000001

#define MLME_IS_STA(adapter) \
	((((adapter)->mlmepriv.fw_state) & WIFI_STATION_STATE) != 0)

#define MLME_STA_CONNECTING 1
#define MLME_STA_DISCONNECTED 2
#define MLME_ADHOC_STARTED 3

#define HW_VAR_BSSID 0
#define HW_VAR_MLME_DISCONNECT 1
#define HW_VAR_MLME_JOIN 2
#define HW_VAR_DO_IQK 3

#define _FIXED_IE_LENGTH_ 12
#define _VENDOR_SPECIFIC_IE_ 221
#define _HT_CAPABILITY_IE_ 45
#define _HT_EXTRA_INFO_IE_ 61
#define EID_VHTCapability 191
#define EID_VHTOperation 192

#define WLAN_REASON_DEAUTH_LEAVING 3
#define WLAN_STATUS_UNSPECIFIED_FAILURE 1

#define FIELD_OFFSET(type, field) offsetof(type, field)

#define RTW_INFO(...) do { } while (0)
#define RTW_ERR(...) do { } while (0)

static inline int _rtw_memcmp(const void *a, const void *b, size_t n)
{
	return memcmp(a, b, n) == 0;
}

typedef unsigned char NDIS_802_11_MAC_ADDRESS[ETH_ALEN];
typedef long NDIS_802_11_RSSI;

typedef struct _NDIS_802_11_SSID {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

typedef struct _NDIS_802_11_CONFIGURATION {
	u32 Length;
	u32 BeaconPeriod;
	u32 ATIMWindow;
	u32 DSConfig;
} NDIS_802_11_CONFIGURATION;

typedef struct _NDIS_802_11_RATES_EX {
	u8 data[16];
} NDIS_802_11_RATES_EX;

typedef struct _WLAN_PHY_INFO {
	u8 SignalStrength;
	u8 SignalQuality;
	u8 Optimum_antenna;
} WLAN_PHY_INFO;

typedef struct _WLAN_BSSID_EX {
	u32 Length;
	NDIS_802_11_MAC_ADDRESS MacAddress;
	u8 Reserved[2];
	NDIS_802_11_SSID Ssid;
	NDIS_802_11_SSID mesh_id;
	u32 Privacy;
	NDIS_802_11_RSSI Rssi;
	NDIS_802_11_CONFIGURATION Configuration;
	u32 InfrastructureMode;
	NDIS_802_11_RATES_EX SupportedRates;
	WLAN_PHY_INFO PhyInfo;
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

typedef struct {
	u8 ElementID;
	u8 Length;
	u8 data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

struct mlme_ext_info {
	u32 state;
	u16 bcn_interval;
	u8 WMM_enable;
	u8 ERP_enable;
	u8 HT_enable;
	u8 HT_caps_enable;
	u8 HT_info_enable;
	u8 agg_enable_bitmap;
	u8 candidate_tid_bitmap;
	u8 bwmode_updated;
	u8 VHT_enable;
	WLAN_BSSID_EX network;
};

struct _timer {
	int cancelled;
};

struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
	struct _timer link_timer;
	u8 cur_channel;
	u8 cur_bwmode;
	u8 cur_ch_offset;
};

struct mlme_priv {
	u32 fw_state;
};

struct _adapter {
	struct mlme_ext_priv mlmeextpriv;
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;

extern unsigned char WMM_OUI[4];

u8 join_cmd_hdl(_adapter *padapter, u8 *pbuf);

void issue_deauth_ex(_adapter *a, u8 *addr, u16 reason, u8 try_cnt, int try_ms);
void flush_all_cam_entry(_adapter *a);
void _cancel_timer_ex(struct _timer *t);
void Set_MSR(_adapter *a, u8 type);
void rtw_hal_set_hwreg(_adapter *a, int id, u8 *val);
void rtw_hal_rcr_set_chk_bssid(_adapter *a, int mode);
void rtw_joinbss_reset(_adapter *a);
u32 report_join_res(_adapter *a, int aid_res, u16 status);
u16 get_beacon_interval(WLAN_BSSID_EX *bss);
int WMM_param_handler(_adapter *a, PNDIS_802_11_VARIABLE_IEs pIE);
void rtw_adjust_chbw(_adapter *a, u8 ch, u8 *bw, u8 *offset);
int rtw_chk_start_clnt_join(_adapter *a, u8 *ch, u8 *bw, u8 *offset);
void rtw_btcoex_connect_notify(_adapter *a, u8 join_type);
void set_channel_bwmode(_adapter *a, u8 ch, u8 offset, u8 bw);
void start_clnt_join(_adapter *a);
void rtw_bss_get_chbw(WLAN_BSSID_EX *bss, u8 *ch, u8 *bw, u8 *offset, u8 ht,
		      u8 vht);

/* Test fixture hooks (shim) */
extern struct host_join_cmd_trace host_join_cmd_trace;
extern int host_chk_start_clnt_join_result;

struct host_join_cmd_trace {
	int deauth_called;
	int flush_cam;
	int cancel_link_timer;
	int joinbss_reset;
	int report_join_res_called;
	int report_join_res_status;
	int start_clnt_join;
	int wmm_handler;
	int set_channel;
	u8 last_ch, last_bw, last_offset;
	u32 mlme_disconnect;
	u32 bssid_set;
	u8 join_type;
	u8 do_iqk_on;
	u8 do_iqk_off;
};

#endif /* HOST_MLME_EXT_JOIN_CMD_TYPES_H */
