/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 mlme_ext chset tests (W3-54).
 */
#ifndef HOST_MLME_EXT_TYPES_H
#define HOST_MLME_EXT_TYPES_H

#include "host_types.h"
#include "host_autoconf.h"
#include "host_rf_types.h"

#include <stdbool.h>
#include <stdio.h>

typedef unsigned long systime;

#define MAX_CHANNEL_NUM 59

#define RTW_CHF_NO_IR (1 << 0)
#define RTW_CHF_DFS (1 << 1)
#define RTW_CHF_NON_OCP (1 << 3)
#define RTW_CHF_NO_HT40U (1 << 4)
#define RTW_CHF_NO_HT40L (1 << 5)
#define RTW_CHF_NO_80MHZ (1 << 6)
#define RTW_CHF_NO_160MHZ (1 << 7)

#define NON_OCP_TIME_MS (30 * 60 * 1000)

#define RTW_PRINT(...) do { } while (0)
#define RTW_INFO(...) do { } while (0)

typedef struct _RT_CHANNEL_INFO {
	u8 ChannelNum;
	u8 flags;
	systime non_ocp_end_time;
} RT_CHANNEL_INFO;

bool rtw_chbw_to_freq_range(u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo);
int rtw_ch2freq(int chan);
u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);
u8 rtw_get_op_chs_by_cch_bw(u8 cch, u8 bw, u8 **op_chs, u8 *op_ch_num);
u8 rtw_get_offset_by_chbw(u8 ch, u8 bw, u8 *r_offset);
void rtw_sync_chbw(u8 *req_ch, u8 *req_bw, u8 *req_offset, u8 *g_ch,
		   u8 *g_bw, u8 *g_offset);

systime rtw_get_current_time(void);
systime rtw_ms_to_systime(int ms);
u32 rtw_systime_to_ms(systime stime);
bool _rtw_time_after(systime a, systime b);
bool rtw_time_after(systime a, systime b);

#define CH_IS_NON_OCP(rt_ch_info) \
	(rtw_time_after((rt_ch_info)->non_ocp_end_time, rtw_get_current_time()))

void host_mlme_ext_set_current_time(systime t);

int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, const u32 ch);
u8 rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset,
			    bool allow_primary_passive, bool allow_passive);
void rtw_chset_sync_chbw(RT_CHANNEL_INFO *ch_set, u8 *req_ch, u8 *req_bw,
			 u8 *req_offset, u8 *g_ch, u8 *g_bw, u8 *g_offset,
			 bool allow_primary_passive, bool allow_passive);

#ifdef CONFIG_DFS_MASTER
bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset);
bool rtw_chset_is_ch_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch);
u32 rtw_chset_get_ch_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				u8 offset);
bool rtw_chset_update_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset);
bool rtw_chset_update_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				 u8 offset, int ms);
#endif

#endif /* HOST_MLME_EXT_TYPES_H */
