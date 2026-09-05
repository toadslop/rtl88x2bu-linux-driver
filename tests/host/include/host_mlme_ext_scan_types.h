/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_EXT_SCAN_TYPES_H
#define HOST_MLME_EXT_SCAN_TYPES_H

#include "host_types.h"
#include <stdbool.h>
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define RTW_SCAN_SPARSE_MIRACAST 1
#define RTW_SCAN_SPARSE_BG 1
#define CONFIG_SCAN_BACKOP 1
#define SS_BACKOP_EN (1 << 0)
#define SS_BACKOP_EN_NL (1 << 1)
#define WIRELESS_11B (1 << 0)
#define WIRELESS_11G (1 << 1)
#define WIRELESS_11A (1 << 2)
#define WIRELESS_11_24N (1 << 3)
#define WIRELESS_11_5N (1 << 4)
#define WIRELESS_11AC (1 << 6)
#define SUPPORTED_24G_NETTYPE_MSK (WIRELESS_11B | WIRELESS_11G | WIRELESS_11_24N)
#define SUPPORTED_5G_NETTYPE_MSK (WIRELESS_11A | WIRELESS_11_5N | WIRELESS_11AC)
#define IsSupported24G(n) ((n) & SUPPORTED_24G_NETTYPE_MSK ? _TRUE : _FALSE)
#define is_supported_5g(n) ((n) & SUPPORTED_5G_NETTYPE_MSK ? _TRUE : _FALSE)
#define rtw_min(a, b) (((a) < (b)) ? (a) : (b))
#define MAX_CHANNEL_NUM 59
#define MAX_CHANNEL_NUM_2G 14

typedef unsigned long systime;

struct rtw_ieee80211_channel { u16 hw_value; u32 flags; };
struct ss_res {
	u16 scan_ch_ms; u32 scan_timeout_ms; u16 duration;
#ifdef CONFIG_SCAN_BACKOP
	u8 backop_flags_sta, backop_flags_ap, backop_flags_mesh;
	u8 backop_flags, scan_cnt, scan_cnt_max; u16 backop_ms;
#endif
};
struct registry_priv { u32 wireless_mode; };
struct mlme_ext_priv { systime last_scan_time; struct ss_res sitesurvey_res; };
struct _adapter { struct registry_priv registrypriv; struct mlme_ext_priv mlmeextpriv; };
typedef struct _adapter _adapter;
struct mi_state { u8 sta_num, ld_sta_num, ap_num, ld_ap_num, mesh_num, ld_mesh_num; };
#define MSTATE_STA_NUM(m) ((m)->sta_num)
#define MSTATE_STA_LD_NUM(m) ((m)->ld_sta_num)
#define MSTATE_AP_NUM(m) ((m)->ap_num)
#define MSTATE_AP_LD_NUM(m) ((m)->ld_ap_num)
#define mlmeext_scan_backop_flags_sta(e) ((e)->sitesurvey_res.backop_flags_sta)
#define mlmeext_chk_scan_backop_flags_sta(e, f) ((e)->sitesurvey_res.backop_flags_sta & (f))
#define mlmeext_scan_backop_flags_ap(e) ((e)->sitesurvey_res.backop_flags_ap)
#define mlmeext_chk_scan_backop_flags_ap(e, f) ((e)->sitesurvey_res.backop_flags_ap & (f))

void host_scan_set_passing_time_ms(u32 ms);
void host_scan_set_current_time(systime t);
void host_scan_set_busy_traffic(u8 v);
void host_scan_set_miracast(u8 v);
void host_scan_set_mi_state(const struct mi_state *m);
systime rtw_get_current_time(void);
u32 rtw_get_passing_time_ms(systime start);
bool rtw_mi_busy_traffic_check(_adapter *a);
bool rtw_mi_check_miracast_enabled(_adapter *a);
void rtw_mi_status(_adapter *a, struct mi_state *m);
u8 rtw_scan_sparse(_adapter *a, struct rtw_ieee80211_channel *ch, u8 n);
u8 rtw_scan_backop_decision(_adapter *a);
u32 rtw_scan_timeout_decision(_adapter *a);

#endif
