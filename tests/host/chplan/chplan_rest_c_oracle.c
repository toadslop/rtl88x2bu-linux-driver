/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 C oracle for rtw_chplan_rest beacon-hint helper (W3-17 PR1).
 * Mirrors core/rtw_chplan_rest.c::rtw_process_beacon_hint.
 */
#include "host_chplan_types.h"

#ifndef RTW_CHPLAN_BEACON_HINT_NON_WORLD_WIDE
#define RTW_CHPLAN_BEACON_HINT_NON_WORLD_WIDE 0
#endif

#ifndef RTW_CHPLAN_BEACON_HINT_ON_2G_CH_1_11
#define RTW_CHPLAN_BEACON_HINT_ON_2G_CH_1_11 0
#endif

#ifndef RTW_CHPLAN_BEACON_HINT_ON_DFS_CH
#define RTW_CHPLAN_BEACON_HINT_ON_DFS_CH 0
#endif

u8 host_rest_process_beacon_hint(_adapter *adapter, WLAN_BSSID_EX *bss)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	RT_CHANNEL_INFO *chset = rfctl->channel_set;
	u8 ch = (u8)bss->Configuration.DSConfig;
	int chset_idx = rtw_chset_search_ch(chset, ch);
	u8 act_cnt = 0;

	if (chset_idx < 0)
		goto exit;

	if ((chset[chset_idx].flags & RTW_CHF_NO_IR)
	    && (RTW_CHPLAN_BEACON_HINT_NON_WORLD_WIDE || !rfctl->country_ent
		|| IS_ALPHA2_WORLDWIDE(rfctl->country_ent->alpha2))
	    && (RTW_CHPLAN_BEACON_HINT_ON_2G_CH_1_11 || !(ch <= 11))
	    && (RTW_CHPLAN_BEACON_HINT_ON_DFS_CH
		|| !(chset[chset_idx].flags & RTW_CHF_DFS))) {
		chset[chset_idx].flags &= ~RTW_CHF_NO_IR;
		act_cnt++;
	}

exit:
	return act_cnt;
}
