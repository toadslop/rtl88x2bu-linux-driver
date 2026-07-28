/* SPDX-License-Identifier: GPL-2.0 */
/*
 * RF frequency helpers for host chplan L2 tests (oracle: core/rtw_rf.c).
 */
#include "host_chplan_types.h"

int rtw_ch2freq(int chan)
{
	if (chan >= 1 && chan <= 14) {
		if (chan == 14)
			return 2484;
		else if (chan < 14)
			return 2407 + chan * 5;
	} else if (chan >= 36 && chan <= 177)
		return 5000 + chan * 5;

	return 0;
}

int rtw_freq2ch(int freq)
{
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq <= 45000)
		return (freq - 5000) / 5;
	else if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;
	else
		return 0;
}

u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset)
{
	u8 cch = ch;

	if (bw == CHANNEL_WIDTH_160) {
		if (ch % 4 == 0) {
			if (ch >= 36 && ch <= 64)
				cch = 50;
			else if (ch >= 100 && ch <= 128)
				cch = 114;
		} else if (ch % 4 == 1) {
			if (ch >= 149 && ch <= 177)
				cch = 163;
		}
	} else if (bw == CHANNEL_WIDTH_80) {
		if (ch <= 14)
			cch = 7;
		else if (ch % 4 == 0) {
			if (ch >= 36 && ch <= 48)
				cch = 42;
			else if (ch >= 52 && ch <= 64)
				cch = 58;
			else if (ch >= 100 && ch <= 112)
				cch = 106;
			else if (ch >= 116 && ch <= 128)
				cch = 122;
			else if (ch >= 132 && ch <= 144)
				cch = 138;
		} else if (ch % 4 == 1) {
			if (ch >= 149 && ch <= 161)
				cch = 155;
			else if (ch >= 165 && ch <= 177)
				cch = 171;
		}
	} else if (bw == CHANNEL_WIDTH_40) {
		if (offset == HAL_PRIME_CHNL_OFFSET_LOWER)
			cch = ch + 2;
		else if (offset == HAL_PRIME_CHNL_OFFSET_UPPER)
			cch = ch - 2;
	}

	return cch;
}

bool rtw_chbw_to_freq_range(u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo)
{
	u8 c_ch;
	u32 freq;
	u32 hi_ret = 0, lo_ret = 0;
	bool valid = _FALSE;

	if (hi)
		*hi = 0;
	if (lo)
		*lo = 0;

	c_ch = rtw_get_center_ch(ch, bw, offset);
	freq = rtw_ch2freq(c_ch);

	if (!freq)
		goto exit;

	if (bw == CHANNEL_WIDTH_160) {
		hi_ret = freq + 80;
		lo_ret = freq - 80;
	} else if (bw == CHANNEL_WIDTH_80) {
		hi_ret = freq + 40;
		lo_ret = freq - 40;
	} else if (bw == CHANNEL_WIDTH_40) {
		hi_ret = freq + 20;
		lo_ret = freq - 20;
	} else if (bw == CHANNEL_WIDTH_20) {
		hi_ret = freq + 10;
		lo_ret = freq - 10;
	}

	if (hi)
		*hi = hi_ret;
	if (lo)
		*lo = lo_ret;

	valid = _TRUE;

exit:
	return valid;
}

void rtw_chplan_warn_regd_mismatch(u8 id, u8 regd_2g, u8 regd_5g)
{
	(void)id;
	(void)regd_2g;
	(void)regd_5g;
}

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
