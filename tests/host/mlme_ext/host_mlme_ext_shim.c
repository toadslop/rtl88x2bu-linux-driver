// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 shims for W3-54 chset tests: fake clock.
 */
#include "host_mlme_ext_types.h"

#undef rtw_warn_on

static systime host_current_time;

void host_mlme_ext_set_current_time(systime t)
{
	host_current_time = t;
}

systime rtw_get_current_time(void)
{
	return host_current_time;
}

systime rtw_ms_to_systime(int ms)
{
<<<<<<< HEAD
	/* Duration in ms (kernel: msecs_to_jiffies); not an absolute time. */
=======
	/* Delta only — callers add rtw_get_current_time() (matches _rtw_ms_to_systime). */
>>>>>>> 9fae534 (fix(mlme_ext): match kernel RT_CHANNEL_INFO 32-byte stride)
	return (systime)ms;
}

u32 rtw_systime_to_ms(systime stime)
{
	/* Duration in ms (kernel: jiffies_to_msecs); not relative to now. */
	return (u32)stime;
}

bool _rtw_time_after(systime a, systime b)
{
	return a > b;
}

bool rtw_time_after(systime a, systime b)
{
	return _rtw_time_after(a, b);
}

int rtw_warn_on(int cond)
{
	(void)cond;
	return 0;
}

bool rtw_is_chbw_grouped(u8 ch_a, u8 bw_a, u8 offset_a, u8 ch_b, u8 bw_b,
			 u8 offset_b)
{
	if (ch_a != ch_b)
		return _FALSE;
	if ((bw_a == CHANNEL_WIDTH_40 || bw_a == CHANNEL_WIDTH_80) &&
	    (bw_b == CHANNEL_WIDTH_40 || bw_b == CHANNEL_WIDTH_80) &&
	    offset_a != offset_b)
		return _FALSE;
	return _TRUE;
}

void rtw_sync_chbw(u8 *req_ch, u8 *req_bw, u8 *req_offset, u8 *g_ch, u8 *g_bw,
		   u8 *g_offset)
{
	*req_ch = *g_ch;

	if (*req_bw == CHANNEL_WIDTH_80 && *g_ch <= 14)
		*req_bw = CHANNEL_WIDTH_40;

	switch (*req_bw) {
	case CHANNEL_WIDTH_80:
		if (*g_bw == CHANNEL_WIDTH_40 || *g_bw == CHANNEL_WIDTH_80)
			*req_offset = *g_offset;
		else if (*g_bw == CHANNEL_WIDTH_20)
			rtw_get_offset_by_chbw(*req_ch, *req_bw, req_offset);

		if (*req_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE) {
			rtw_warn_on(1);
			*req_bw = CHANNEL_WIDTH_20;
		}
		break;
	case CHANNEL_WIDTH_40:
		if (*g_bw == CHANNEL_WIDTH_40 || *g_bw == CHANNEL_WIDTH_80)
			*req_offset = *g_offset;
		else if (*g_bw == CHANNEL_WIDTH_20)
			rtw_get_offset_by_chbw(*req_ch, *req_bw, req_offset);

		if (*req_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE) {
			rtw_warn_on(1);
			*req_bw = CHANNEL_WIDTH_20;
		}
		break;
	case CHANNEL_WIDTH_20:
		*req_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		break;
	default:
		rtw_warn_on(1);
	}

	if (*req_bw > *g_bw) {
		*g_bw = *req_bw;
		*g_offset = *req_offset;
	}
}
