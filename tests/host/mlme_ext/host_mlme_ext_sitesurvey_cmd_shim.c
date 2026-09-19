// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_ext_sitesurvey_cmd_types.h"
#include <string.h>

struct host_sitesurvey_cmd_trace host_sitesurvey_cmd_trace;
int host_ps_annc_result;

void host_sitesurvey_cmd_reset_trace(void)
{
	memset(&host_sitesurvey_cmd_trace, 0, sizeof(host_sitesurvey_cmd_trace));
	host_ps_annc_result = 0;
}

RT_CHANNEL_INFO *adapter_to_chset(_adapter *a)
{
	return a->rfctl.channel_set;
}

int rtw_scan_ch_decision(_adapter *a, struct rtw_ieee80211_channel *out,
			 u8 out_max, struct rtw_ieee80211_channel *in, u8 in_num,
			 u8 acs, int reason)
{
	u8 i, n = in_num > out_max ? out_max : in_num;

	(void)a;
	(void)acs;
	(void)reason;
	for (i = 0; i < n; i++)
		out[i] = in[i];
	return n;
}

u8 rtw_ps_annc(_adapter *a, bool ps)
{
	u8 ret;

	(void)a;
	(void)ps;
	host_sitesurvey_cmd_trace.ps_annc_calls++;
	ret = host_ps_annc_result ? 1 : 0;
	return ret;
}

void rtw_phydm_ability_backup(_adapter *a) { (void)a; }
void rtw_phydm_func_for_offchannel(_adapter *a) { (void)a; }

void sitesurvey_set_igi(_adapter *a)
{
	if (mlmeext_scan_state(&a->mlmeextpriv) == SCAN_ENTER)
		host_sitesurvey_cmd_trace.set_igi_enter = 1;
}

void sitesurvey_set_msr(_adapter *a, bool enter)
{
	if (enter)
		host_sitesurvey_cmd_trace.set_msr_enter = 1;
}

void site_survey(_adapter *a, u8 ch, RT_SCAN_TYPE type)
{
	(void)a;
	host_sitesurvey_cmd_trace.site_survey_ch = ch;
	host_sitesurvey_cmd_trace.site_survey_type = type;
}

void set_survey_timer(struct mlme_ext_priv *e, u32 ms)
{
	(void)e;
	host_sitesurvey_cmd_trace.survey_timer_ms = (u8)(ms > 255 ? 255 : ms);
}

void rtw_hal_set_hwreg(_adapter *a, int id, u8 *val)
{
	(void)a;
	if (id == HW_VAR_MLME_SITESURVEY && val && *val)
		host_sitesurvey_cmd_trace.hw_survey_on = 1;
}

void rtw_hal_macid_sleep_all_used(_adapter *a) { (void)a; }
void rtw_rx_ampdu_apply(_adapter *a) { (void)a; }

u8 sitesurvey_pick_ch_behavior(_adapter *a, u8 *ch, RT_SCAN_TYPE *type)
{
	struct ss_res *ss = &a->mlmeextpriv.sitesurvey_res;

	host_sitesurvey_cmd_trace.pick_ch_calls++;
	if (ss->channel_idx >= ss->ch_num)
		return SCAN_COMPLETE;
	*ch = (u8)ss->ch[ss->channel_idx].hw_value;
	*type = SCAN_ACTIVE;
	return SCAN_PROCESS;
}
