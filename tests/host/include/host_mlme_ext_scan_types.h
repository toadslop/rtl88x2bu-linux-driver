/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_EXT_SCAN_TYPES_H
#define HOST_MLME_EXT_SCAN_TYPES_H

#include "host_types.h"
#include <stdbool.h>
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define RTW_INFO(...) do { } while (0)
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
#define RTW_CHANNEL_SCAN_AMOUNT 8
#define RTW_IEEE80211_CHAN_PASSIVE_SCAN (1 << 1)
#define RTW_CHF_NO_IR (1 << 0)
#define RTW_CHF_DFS (1 << 1)

#define SCAN_PASSIVE 0
#define SCAN_ACTIVE 1

#define SCAN_PROCESS 4
#define SCAN_BACKING_OP 5
#define SCAN_TO_P2P_LISTEN 10
#define SCAN_COMPLETE 12

typedef unsigned long systime;
typedef int RT_SCAN_TYPE;

struct rtw_ieee80211_channel { u16 hw_value; u32 flags; };
struct RT_CHANNEL_INFO {
	u8 ChannelNum;
	u8 flags;
	u8 _pad0[6];
	systime non_ocp_end_time;
	u8 hidden_bss_cnt;
	u8 _pad1[7];
	void *os_chan;
};
struct rf_ctl_t {
	struct RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
	u8 dfs_slave_with_rd;
};
struct ss_res {
	u16 scan_ch_ms;
	u32 scan_timeout_ms;
	u16 duration;
	int channel_idx;
	u8 force_ssid_scan;
	u8 ssid_num;
	u8 ch_num;
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT];
#ifdef CONFIG_SCAN_BACKOP
	u8 backop_flags_sta, backop_flags_ap, backop_flags_mesh;
	u8 backop_flags, scan_cnt, scan_cnt_max;
	u16 backop_ms;
#endif
};
struct registry_priv { u32 wireless_mode; };
struct wifidirect_info {
	struct {
		u8 scan_op_ch_only;
		u8 operation_ch[RTW_CHANNEL_SCAN_AMOUNT];
	} rx_invitereq_info;
	struct {
		u8 scan_op_ch_only;
		u8 operation_ch[RTW_CHANNEL_SCAN_AMOUNT];
	} p2p_info;
	u8 social_chan[RTW_CHANNEL_SCAN_AMOUNT];
	u8 find_phase_state_exchange_cnt;
};
struct mlme_ext_priv {
	systime last_scan_time;
	u8 scan_abort;
	struct ss_res sitesurvey_res;
};
struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_ext_priv mlmeextpriv;
	struct rf_ctl_t rfctl;
#ifdef CONFIG_P2P
	struct wifidirect_info wdinfo;
#endif
};
typedef struct _adapter _adapter;
#define adapter_to_rfctl(a) (&(a)->rfctl)
#define IS_DFS_SLAVE_WITH_RD(rfctl) ((rfctl)->dfs_slave_with_rd)
#define CH_IS_NON_OCP(rt_ch_info) \
	((rt_ch_info)->non_ocp_end_time > host_scan_current_time())
struct mi_state { u8 sta_num, ld_sta_num, ap_num, ld_ap_num, mesh_num, ld_mesh_num; };
#define MSTATE_STA_NUM(m) ((m)->sta_num)
#define MSTATE_STA_LD_NUM(m) ((m)->ld_sta_num)
#define MSTATE_AP_NUM(m) ((m)->ap_num)
#define MSTATE_AP_LD_NUM(m) ((m)->ld_ap_num)
#define mlmeext_scan_backop_flags_sta(e) ((e)->sitesurvey_res.backop_flags_sta)
#define mlmeext_chk_scan_backop_flags_sta(e, f) ((e)->sitesurvey_res.backop_flags_sta & (f))
#define mlmeext_scan_backop_flags_ap(e) ((e)->sitesurvey_res.backop_flags_ap)
#define mlmeext_chk_scan_backop_flags_ap(e, f) ((e)->sitesurvey_res.backop_flags_ap & (f))
#define mlmeext_assign_scan_backop_flags(mlmeext, flags) \
	((mlmeext)->sitesurvey_res.backop_flags = (flags))

#ifdef CONFIG_P2P
extern u8 host_p2p_social;
extern u8 host_p2p_needed;
#define P2P_STATE_NONE 0
#define P2P_FINDPHASE_EX_MAX 2
#define rtw_p2p_findphase_ex_is_social(wdinfo) (host_p2p_social)
#define rtw_p2p_findphase_ex_is_needed(wdinfo) (host_p2p_needed)
#define rtw_p2p_findphase_ex_set(wdinfo, value) do { (void)(wdinfo); (void)(value); } while (0)
#define rtw_p2p_chk_state(wdinfo, state) 0
#endif

void host_scan_set_passing_time_ms(u32 ms);
void host_scan_set_current_time(systime t);
systime host_scan_current_time(void);
void host_scan_set_busy_traffic(u8 v);
void host_scan_set_miracast(u8 v);
void host_scan_set_mi_state(const struct mi_state *m);
void host_scan_set_p2p_social(u8 on);
void host_scan_set_p2p_needed(u8 on);
void host_scan_set_dfs_domain_unknown(u8 on);
systime rtw_get_current_time(void);
u32 rtw_get_passing_time_ms(systime start);
bool rtw_mi_busy_traffic_check(_adapter *a);
bool rtw_mi_check_miracast_enabled(_adapter *a);
void rtw_mi_status(_adapter *a, struct mi_state *m);
int rtw_chset_search_ch(struct RT_CHANNEL_INFO *ch_set, const u32 ch);
u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl);
u8 rtw_scan_sparse(_adapter *a, struct rtw_ieee80211_channel *ch, u8 n);
u8 rtw_scan_backop_decision(_adapter *a);
u32 rtw_scan_timeout_decision(_adapter *a);
u8 sitesurvey_pick_ch_behavior(_adapter *a, u8 *ch, RT_SCAN_TYPE *type);

#endif
