// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_ext_scan_types.h"

static systime host_now;
static u32 host_pass_ms;
static u8 host_busy, host_mirc;
static u8 host_dfs_unknown;
static struct mi_state host_mi;

u8 host_p2p_social;
u8 host_p2p_needed;

u8 host_p2p_social_get(void) { return host_p2p_social; }
u8 host_p2p_needed_get(void) { return host_p2p_needed; }

void host_scan_set_current_time(systime t) { host_now = t; }
systime host_scan_current_time(void) { return host_now; }
void host_scan_set_passing_time_ms(u32 ms) { host_pass_ms = ms; }
void host_scan_set_busy_traffic(u8 v) { host_busy = v; }
void host_scan_set_miracast(u8 v) { host_mirc = v; }
void host_scan_set_mi_state(const struct mi_state *m) { host_mi = *m; }
void host_scan_set_p2p_social(u8 on) { host_p2p_social = on; }
void host_scan_set_p2p_needed(u8 on) { host_p2p_needed = on; }
void host_scan_set_dfs_domain_unknown(u8 on) { host_dfs_unknown = on; }

bool rtw_mlme_band_check(_adapter *adapter, const u32 ch)
{
	if (adapter->setband == WIFI_FREQUENCY_BAND_AUTO ||
	    (adapter->setband == WIFI_FREQUENCY_BAND_2GHZ && ch < 35) ||
	    (adapter->setband == WIFI_FREQUENCY_BAND_5GHZ && ch > 35))
		return _TRUE;
	return _FALSE;
}

bool rtw_mlme_ignore_chan(_adapter *adapter, const u32 ch)
{
	if ((adapter->bandskip & BAND_24G) && ch < 35)
		return _TRUE;
	if ((adapter->bandskip & BAND_5G) && ch > 35)
		return _TRUE;
	return _FALSE;
}

systime rtw_get_current_time(void) { return host_now; }
u32 rtw_get_passing_time_ms(systime s) { (void)s; return host_pass_ms; }
bool rtw_mi_busy_traffic_check(_adapter *a) { (void)a; return host_busy; }
bool rtw_mi_check_miracast_enabled(_adapter *a) { (void)a; return host_mirc; }
void rtw_mi_status(_adapter *a, struct mi_state *m) { (void)a; *m = host_mi; }

int rtw_chset_search_ch(struct RT_CHANNEL_INFO *ch_set, const u32 ch)
{
	int i;

	for (i = 0; i < MAX_CHANNEL_NUM; i++) {
		if (ch_set[i].ChannelNum == ch)
			return i;
	}
	return -1;
}

u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl)
{
	(void)rfctl;
	return host_dfs_unknown;
}

systime rtw_rust_scan_last_scan_time(_adapter *a)
{
	return a->mlmeextpriv.last_scan_time;
}

void rtw_rust_scan_set_last_scan_time(_adapter *a, systime t)
{
	a->mlmeextpriv.last_scan_time = t;
}

u32 rtw_rust_scan_wireless_mode(_adapter *a)
{
	return a->registrypriv.wireless_mode;
}

u16 rtw_rust_scan_ch_ms(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.scan_ch_ms;
}

u16 rtw_rust_scan_duration(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.duration;
}

u8 rtw_rust_scan_cnt_max(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.scan_cnt_max;
}

u16 rtw_rust_scan_backop_ms(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.backop_ms;
}

void rtw_rust_scan_set_timeout_ms(_adapter *a, u32 ms)
{
	a->mlmeextpriv.sitesurvey_res.scan_timeout_ms = ms;
}

u8 rtw_rust_scan_backop_flags_sta(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.backop_flags_sta;
}

u8 rtw_rust_scan_backop_flags_ap(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.backop_flags_ap;
}

u8 rtw_rust_pick_ch_scan_abort(_adapter *a)
{
	return a->mlmeextpriv.scan_abort;
}

void rtw_rust_pick_ch_set_channel_idx(_adapter *a, int idx)
{
	a->mlmeextpriv.sitesurvey_res.channel_idx = idx;
}

int rtw_rust_pick_ch_channel_idx(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.channel_idx;
}

u8 rtw_rust_pick_ch_force_ssid_scan(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.force_ssid_scan;
}

void rtw_rust_pick_ch_set_force_ssid_scan(_adapter *a, u8 v)
{
	a->mlmeextpriv.sitesurvey_res.force_ssid_scan = v;
}

u8 rtw_rust_pick_ch_ssid_num(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.ssid_num;
}

u8 rtw_rust_pick_ch_ch_num(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.ch_num;
}

u16 rtw_rust_pick_ch_ch_hw_value(_adapter *a, int idx)
{
	return a->mlmeextpriv.sitesurvey_res.ch[idx].hw_value;
}

u32 rtw_rust_pick_ch_ch_flags(_adapter *a, int idx)
{
	return a->mlmeextpriv.sitesurvey_res.ch[idx].flags;
}

u8 rtw_rust_pick_ch_scan_cnt(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.scan_cnt;
}

void rtw_rust_pick_ch_set_scan_cnt(_adapter *a, u8 v)
{
	a->mlmeextpriv.sitesurvey_res.scan_cnt = v;
}

u8 rtw_rust_pick_ch_scan_cnt_max(_adapter *a)
{
	return a->mlmeextpriv.sitesurvey_res.scan_cnt_max;
}

void rtw_rust_pick_ch_set_backop_flags(_adapter *a, u8 v)
{
	a->mlmeextpriv.sitesurvey_res.backop_flags = v;
}

struct RT_CHANNEL_INFO *rtw_rust_pick_ch_channel_set(_adapter *a)
{
	return a->rfctl.channel_set;
}

u8 rtw_rust_pick_ch_chset_flags(_adapter *a, int idx)
{
	return a->rfctl.channel_set[idx].flags;
}

u8 rtw_rust_pick_ch_hidden_bss_cnt(_adapter *a, int idx)
{
	return a->rfctl.channel_set[idx].hidden_bss_cnt;
}

systime rtw_rust_pick_ch_non_ocp_end_time(_adapter *a, int idx)
{
	return a->rfctl.channel_set[idx].non_ocp_end_time;
}

u8 rtw_rust_pick_ch_dfs_slave_with_rd(_adapter *a)
{
	return a->rfctl.dfs_slave_with_rd;
}

struct rf_ctl_t *rtw_rust_pick_ch_rfctl(_adapter *a)
{
	return adapter_to_rfctl(a);
}

u8 rtw_rust_pick_ch_p2p_state_not_none(_adapter *a)
{
#ifdef CONFIG_P2P
	return !rtw_p2p_chk_state(&a->wdinfo, P2P_STATE_NONE);
#else
	(void)a;
	return 0;
#endif
}

void rtw_rust_pick_ch_p2p_findphase_ex_max(_adapter *a)
{
#ifdef CONFIG_P2P
	rtw_p2p_findphase_ex_set(&a->wdinfo, P2P_FINDPHASE_EX_MAX);
#else
	(void)a;
#endif
}

u8 rtw_rust_pick_ch_rx_scan_op_ch_only(_adapter *a)
{
#ifdef CONFIG_P2P
	return a->wdinfo.rx_invitereq_info.scan_op_ch_only;
#else
	(void)a;
	return 0;
#endif
}

u8 rtw_rust_pick_ch_p2p_scan_op_ch_only(_adapter *a)
{
#ifdef CONFIG_P2P
	return a->wdinfo.p2p_info.scan_op_ch_only;
#else
	(void)a;
	return 0;
#endif
}

u8 rtw_rust_pick_ch_rx_op_ch(_adapter *a, int idx)
{
#ifdef CONFIG_P2P
	return a->wdinfo.rx_invitereq_info.operation_ch[idx];
#else
	(void)a;
	(void)idx;
	return 0;
#endif
}

u8 rtw_rust_pick_ch_p2p_op_ch(_adapter *a, int idx)
{
#ifdef CONFIG_P2P
	return a->wdinfo.p2p_info.operation_ch[idx];
#else
	(void)a;
	(void)idx;
	return 0;
#endif
}

u8 rtw_rust_pick_ch_social_chan(_adapter *a, int idx)
{
#ifdef CONFIG_P2P
	return a->wdinfo.social_chan[idx];
#else
	(void)a;
	(void)idx;
	return 0;
#endif
}

u8 rtw_rust_pick_ch_p2p_social(_adapter *a)
{
#ifdef CONFIG_P2P
	return rtw_p2p_findphase_ex_is_social(&a->wdinfo);
#else
	(void)a;
	return 0;
#endif
}

u8 rtw_rust_pick_ch_p2p_needed(_adapter *a)
{
#ifdef CONFIG_P2P
	return rtw_p2p_findphase_ex_is_needed(&a->wdinfo);
#else
	(void)a;
	return 0;
#endif
}

/* Host L2 stubs for W3-70 ch_decision Rust oracle (PR6). */
u8 rtw_rust_scan_regsty_wifi_spec(_adapter *a)
{
	return a->registrypriv.wifi_spec;
}

u8 rtw_rust_scan_max_chan_nums(_adapter *a)
{
	return a->rfctl.max_chan_nums;
}

u8 rtw_rust_scan_chset_channel_num(_adapter *a, int idx)
{
	return a->rfctl.channel_set[idx].ChannelNum;
}

u8 rtw_rust_scan_chset_flags(_adapter *a, int idx)
{
	return a->rfctl.channel_set[idx].flags;
}

void rtw_rust_scan_chset_clear_hidden_bss(_adapter *a, int idx)
{
	a->rfctl.channel_set[idx].hidden_bss_cnt = 0;
}

void *rtw_rust_scan_chset(_adapter *a)
{
	return a->rfctl.channel_set;
}

struct ss_res *rtw_rust_ss_res(_adapter *adapter)
{
	return &adapter->mlmeextpriv.sitesurvey_res;
}

void rtw_rust_ss_clear_scan_counters(struct ss_res *ss)
{
	ss->bss_cnt = 0;
	ss->activate_ch_cnt = 0;
	ss->channel_idx = 0;
	ss->force_ssid_scan = 0;
	ss->igi_scan = 0;
	ss->igi_before_scan = 0;
#ifdef CONFIG_SCAN_BACKOP
	ss->scan_cnt = 0;
#endif
	ss->ssid_num = 0;
}

u8 rtw_rust_ss_copy_ssids_from_parm(struct ss_res *ss, struct sitesurvey_parm *parm)
{
	int i;

	ss->ssid_num = 0;
	for (i = 0; i < RTW_SSID_SCAN_AMOUNT; i++) {
		if (parm->ssid[i].SsidLength) {
			_rtw_memcpy(ss->ssid[i].Ssid, parm->ssid[i].Ssid, IW_ESSID_MAX_SIZE);
			ss->ssid[i].SsidLength = parm->ssid[i].SsidLength;
			ss->ssid_num++;
		} else
			ss->ssid[i].SsidLength = 0;
	}
	return ss->ssid_num;
}

struct rtw_ieee80211_channel *rtw_rust_ss_ch(struct ss_res *ss)
{
	return ss->ch;
}

u8 *rtw_rust_ss_ch_num_ptr(struct ss_res *ss)
{
	return &ss->ch_num;
}

void rtw_rust_ss_set_from_parm_tail(struct ss_res *ss, struct sitesurvey_parm *parm)
{
	ss->bw = parm->bw;
	ss->igi = parm->igi;
	ss->token = parm->token;
	ss->duration = parm->duration;
	ss->scan_mode = (u8)parm->scan_mode;
	ss->acs = parm->acs;
}

u8 rtw_rust_parm_ch_num(struct sitesurvey_parm *parm)
{
	return parm->ch_num;
}

struct rtw_ieee80211_channel *rtw_rust_parm_ch(struct sitesurvey_parm *parm)
{
	return parm->ch;
}

bool rtw_rust_parm_acs(struct sitesurvey_parm *parm)
{
	return parm->acs;
}

sint rtw_rust_parm_reason(struct sitesurvey_parm *parm)
{
	return parm->reason;
}

u32 rtw_rust_scan_ch_amount(void)
{
	return RTW_CHANNEL_SCAN_AMOUNT;
}

void rtw_rust_scan_clear_all_hidden_bss(_adapter *adapter)
{
	RT_CHANNEL_INFO *chset = adapter_to_chset(adapter);
	int i;

	for (i = 0; i < MAX_CHANNEL_NUM; i++)
		chset[i].hidden_bss_cnt = 0;
}
