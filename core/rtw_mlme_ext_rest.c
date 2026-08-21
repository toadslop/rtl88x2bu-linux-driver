/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_MLME_EXT_REST_C_

#ifdef HOST_MLME_EXT_TEST
#include "host_mlme_ext_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_MLME_EXT_TEST) || !defined(CONFIG_RUST_MLME_EXT_REST)

#ifdef CONFIG_DFS_MASTER
bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	bool ret = _FALSE;
	u32 hi = 0, lo = 0;
	int i;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (!CH_IS_NON_OCP(&ch_set[i]))
			continue;

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			ret = _TRUE;
			break;
		}
	}

exit:
	return ret;
}

bool rtw_chset_is_ch_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch)
{
	return rtw_chset_is_chbw_non_ocp(ch_set, ch, CHANNEL_WIDTH_20,
					 HAL_PRIME_CHNL_OFFSET_DONT_CARE);
}

u32 rtw_chset_get_ch_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	int ms = 0;
	systime current_time;
	u32 hi = 0, lo = 0;
	int i;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	current_time = rtw_get_current_time();

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (!CH_IS_NON_OCP(&ch_set[i]))
			continue;

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			if ((int)rtw_systime_to_ms(ch_set[i].non_ocp_end_time -
						   current_time) > ms)
				ms = rtw_systime_to_ms(ch_set[i].non_ocp_end_time -
						       current_time);
		}
	}

exit:
	return ms;
}

static bool _rtw_chset_update_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				      u8 offset, int ms)
{
	u32 hi = 0, lo = 0;
	int i;
	bool updated = 0;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			if (ms >= 0)
				ch_set[i].non_ocp_end_time =
					rtw_get_current_time() +
					rtw_ms_to_systime(ms);
			else
				ch_set[i].non_ocp_end_time =
					rtw_get_current_time() +
					rtw_ms_to_systime(NON_OCP_TIME_MS);
			ch_set[i].flags |= RTW_CHF_NON_OCP;
			updated = 1;
		}
	}

exit:
	return updated;
}

bool rtw_chset_update_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	return _rtw_chset_update_non_ocp(ch_set, ch, bw, offset, -1);
}

bool rtw_chset_update_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				 u8 offset, int ms)
{
	return _rtw_chset_update_non_ocp(ch_set, ch, bw, offset, ms);
}
#endif /* CONFIG_DFS_MASTER */

int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, const u32 ch)
{
	int i;

	if (ch == 0)
		return -1;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (ch == ch_set[i].ChannelNum)
			return i;
	}

	return -1;
}

u8 rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset,
			   bool allow_primary_passive, bool allow_passive)
{
	u8 cch;
	u8 *op_chs;
	u8 op_ch_num;
	u8 valid = 0;
	int i;
	int ch_idx;

	cch = rtw_get_center_ch(ch, bw, offset);

	if (!rtw_get_op_chs_by_cch_bw(cch, bw, &op_chs, &op_ch_num))
		goto exit;

	for (i = 0; i < op_ch_num; i++) {
		ch_idx = rtw_chset_search_ch(ch_set, *(op_chs + i));
		if (ch_idx == -1)
			break;
		if (ch_set[ch_idx].flags & RTW_CHF_NO_IR) {
			if ((!allow_primary_passive &&
			     ch_set[ch_idx].ChannelNum == ch) ||
			    (!allow_passive &&
			     ch_set[ch_idx].ChannelNum != ch))
				break;
		}
		if (bw >= CHANNEL_WIDTH_40) {
			if ((ch_set[ch_idx].flags & RTW_CHF_NO_HT40U) &&
			    i % 2 == 0)
				break;
			if ((ch_set[ch_idx].flags & RTW_CHF_NO_HT40L) &&
			    i % 2 == 1)
				break;
		}
		if (bw >= CHANNEL_WIDTH_80 &&
		    (ch_set[ch_idx].flags & RTW_CHF_NO_80MHZ))
			break;
		if (bw >= CHANNEL_WIDTH_160 &&
		    (ch_set[ch_idx].flags & RTW_CHF_NO_160MHZ))
			break;
	}

	if (op_ch_num != 0 && i == op_ch_num)
		valid = 1;

exit:
	return valid;
}

void rtw_chset_sync_chbw(RT_CHANNEL_INFO *ch_set, u8 *req_ch, u8 *req_bw,
			 u8 *req_offset, u8 *g_ch, u8 *g_bw, u8 *g_offset,
			 bool allow_primary_passive, bool allow_passive)
{
	u8 r_ch, r_bw, r_offset;
	u8 u_ch, u_bw, u_offset;
	u8 cur_bw = *req_bw;

	while (1) {
		r_ch = *req_ch;
		r_bw = cur_bw;
		r_offset = *req_offset;
		u_ch = *g_ch;
		u_bw = *g_bw;
		u_offset = *g_offset;

		rtw_sync_chbw(&r_ch, &r_bw, &r_offset, &u_ch, &u_bw, &u_offset);

		if (rtw_chset_is_chbw_valid(ch_set, r_ch, r_bw, r_offset,
					    allow_primary_passive,
					    allow_passive))
			break;
		if (cur_bw == CHANNEL_WIDTH_20) {
			rtw_warn_on(1);
			break;
		}
		cur_bw--;
	}

	*req_ch = r_ch;
	*req_bw = r_bw;
	*req_offset = r_offset;
	*g_ch = u_ch;
	*g_bw = u_bw;
	*g_offset = u_offset;
}

#endif /* !CONFIG_RUST || HOST_MLME_EXT_TEST || !CONFIG_RUST_MLME_EXT_REST */
