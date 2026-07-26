/******************************************************************************
 *
 * Copyright(c) 2007 - 2018 Realtek Corporation.
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
#define _RTW_CHPLAN_C_

#ifdef HOST_CHPLAN_TEST
#include "host_chplan_types.h"
#else
#include <drv_types.h>
#endif

#include "rtw_chplan.h"

/* Static tables and deferred helpers live in core/rtw_chplan_rest.c (W2-20). */
extern const struct chplan_ent_t RTW_ChannelPlanMap[];
extern const int RTW_ChannelPlanMap_size;
extern const struct country_chplan country_chplan_map[];
extern const unsigned int rtw_country_chplan_map_size;

u8 rtw_chdef_2g_len(u8 chd);
u8 rtw_chdef_2g_ch(u8 chd, u8 i);
u8 rtw_chdef_2g_attrib(u8 chd);
#if CONFIG_IEEE80211_BAND_5GHZ
u8 rtw_chdef_5g_len(u8 chd);
u8 rtw_chdef_5g_ch(u8 chd, u8 i);
u8 rtw_chdef_5g_attrib(u8 chd);
#endif

#define CLA_2G_12_14_PASSIVE	BIT0
#define CLA_5G_B1_PASSIVE		BIT0
#define CLA_5G_B2_PASSIVE		BIT1
#define CLA_5G_B3_PASSIVE		BIT2
#define CLA_5G_B4_PASSIVE		BIT3
#define CLA_5G_B2_DFS			BIT4
#define CLA_5G_B3_DFS			BIT5
#define CLA_5G_B4_DFS			BIT6

#define RTW_CHD_2G_NULL 0
#define RTW_CHD_5G_NULL 0

#if !defined(CONFIG_RUST) && !defined(HOST_CHPLAN_DATA_ONLY)

u8 rtw_chplan_get_default_regd_2g(u8 id)
{
	return RTW_ChannelPlanMap[id].regd_2g;
}

u8 rtw_chplan_get_default_regd_5g(u8 id)
{
#if CONFIG_IEEE80211_BAND_5GHZ
	return RTW_ChannelPlanMap[id].regd_5g;
#else
	return TXPWR_LMT_NONE;
#endif
}

u8 rtw_chplan_get_default_regd(u8 id)
{
	u8 regd_2g = rtw_chplan_get_default_regd_2g(id);
	u8 regd_5g = rtw_chplan_get_default_regd_5g(id);

	if (regd_2g != TXPWR_LMT_NONE && regd_5g != TXPWR_LMT_NONE) {
		if (regd_2g != regd_5g)
			RTW_WARN("channel_plan:0x%02x, regd_2g:%u, regd_5g:%u not the same\n", id, regd_2g, regd_5g);
		return regd_5g;
	}
	return regd_2g != TXPWR_LMT_NONE ? regd_2g : regd_5g;
}

bool rtw_chplan_is_empty(u8 id)
{
	const struct chplan_ent_t *chplan_map = &RTW_ChannelPlanMap[id];

	if (chplan_map->chd_2g == RTW_CHD_2G_NULL
		#if CONFIG_IEEE80211_BAND_5GHZ
		&& chplan_map->chd_5g == RTW_CHD_5G_NULL
		#endif
	)
		return _TRUE;

	return _FALSE;
}

bool rtw_is_channel_plan_valid(u8 id)
{
	return id < RTW_ChannelPlanMap_size && !rtw_chplan_is_empty(id);
}

bool rtw_regsty_is_excl_chs(struct registry_priv *regsty, u8 ch)
{
	int i;

	for (i = 0; i < MAX_CHANNEL_NUM; i++) {
		if (regsty->excl_chs[i] == 0)
			break;
		if (regsty->excl_chs[i] == ch)
			return _TRUE;
	}
	return _FALSE;
}

#endif /* !CONFIG_RUST && !HOST_CHPLAN_DATA_ONLY */

#if !defined(HOST_CHPLAN_DATA_ONLY) && (!defined(CONFIG_RUST) || defined(HOST_CHPLAN_TEST))

static u8 init_channel_set_from_rtk_priv(_adapter *padapter, RT_CHANNEL_INFO *channel_set)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	struct registry_priv *regsty = adapter_to_regsty(padapter);
	u8 ChannelPlan = rfctl->ChannelPlan;
	u8 index, chanset_size = 0;
	u8 b5GBand = _FALSE, b2_4GBand = _FALSE;
	u8 ch, attrib;
#ifdef CONFIG_DFS_MASTER
	int i;
#endif

	if (!rtw_is_channel_plan_valid(ChannelPlan)) {
		RTW_ERR("ChannelPlan ID 0x%02X error !!!!!\n", ChannelPlan);
		return chanset_size;
	}

	_rtw_memset(channel_set, 0, sizeof(RT_CHANNEL_INFO) * MAX_CHANNEL_NUM);

	if (IsSupported24G(regsty->wireless_mode) && hal_chk_band_cap(padapter, BAND_CAP_2G))
		b2_4GBand = _TRUE;

	if (is_supported_5g(regsty->wireless_mode) && hal_chk_band_cap(padapter, BAND_CAP_5G))
		b5GBand = _TRUE;

	if (b2_4GBand == _FALSE && b5GBand == _FALSE) {
		RTW_WARN("HW band_cap has no intersection with SW wireless_mode setting\n");
		return chanset_size;
	}

	if (b2_4GBand) {
		u8 chd_2g = RTW_ChannelPlanMap[ChannelPlan].chd_2g;

		attrib = rtw_chdef_2g_attrib(chd_2g);

		for (index = 0; index < rtw_chdef_2g_len(chd_2g); index++) {
			ch = rtw_chdef_2g_ch(chd_2g, index);
			if (rtw_regsty_is_excl_chs(regsty, ch) == _TRUE)
				continue;

			if (chanset_size >= MAX_CHANNEL_NUM) {
				RTW_WARN("chset size can't exceed MAX_CHANNEL_NUM(%u)\n", MAX_CHANNEL_NUM);
				break;
			}

			channel_set[chanset_size].ChannelNum = ch;

			if (ch >= 12 && ch <= 14 && (attrib & CLA_2G_12_14_PASSIVE))
				channel_set[chanset_size].flags |= RTW_CHF_NO_IR;

			if (channel_set[chanset_size].flags & RTW_CHF_NO_IR) {
				if (rfctl->country_ent || ch <= 11)
					RTW_INFO("ch%u is PASSIVE\n", ch);
			}

			chanset_size++;
		}
	}

#if CONFIG_IEEE80211_BAND_5GHZ
	if (b5GBand) {
		bool dfs;
		u8 chd_5g = RTW_ChannelPlanMap[ChannelPlan].chd_5g;

		attrib = rtw_chdef_5g_attrib(chd_5g);

		for (index = 0; index < rtw_chdef_5g_len(chd_5g); index++) {
			ch = rtw_chdef_5g_ch(chd_5g, index);
			if (rtw_regsty_is_excl_chs(regsty, ch) == _TRUE)
				continue;
			dfs = (rtw_is_5g_band2(ch) && (attrib & CLA_5G_B2_DFS))
				|| (rtw_is_5g_band3(ch) && (attrib & CLA_5G_B3_DFS))
				|| (rtw_is_5g_band4(ch) && (attrib & CLA_5G_B4_DFS));
			#if !CONFIG_DFS
			if (dfs)
				continue;
			#endif

			if (chanset_size >= MAX_CHANNEL_NUM) {
				RTW_WARN("chset size can't exceed MAX_CHANNEL_NUM(%u)\n", MAX_CHANNEL_NUM);
				break;
			}

			channel_set[chanset_size].ChannelNum = ch;

			if ((rtw_is_5g_band1(ch) && (attrib & CLA_5G_B1_PASSIVE))
				|| (rtw_is_5g_band2(ch) && (attrib & CLA_5G_B2_PASSIVE))
				|| (rtw_is_5g_band3(ch) && (attrib & CLA_5G_B3_PASSIVE))
				|| (rtw_is_5g_band4(ch) && (attrib & CLA_5G_B4_PASSIVE))
			)
				channel_set[chanset_size].flags |= RTW_CHF_NO_IR;

			if (dfs)
				channel_set[chanset_size].flags |= RTW_CHF_DFS;

			if (channel_set[chanset_size].flags & RTW_CHF_NO_IR) {
				if (rfctl->country_ent || (channel_set[chanset_size].flags & RTW_CHF_DFS))
					RTW_INFO("ch%u is PASSIVE%s\n", ch, dfs ? " DFS" : "");
			}

			chanset_size++;
		}
	}

	#ifdef CONFIG_DFS_MASTER
	for (i = 0; i < chanset_size; i++)
		channel_set[i].non_ocp_end_time = rtw_get_current_time();
	#endif
#endif /* CONFIG_IEEE80211_BAND_5GHZ */

	if (chanset_size)
		RTW_INFO(FUNC_ADPT_FMT" ChannelPlan ID:0x%02x, ch num:%d\n"
			, FUNC_ADPT_ARG(padapter), ChannelPlan, chanset_size);
	else
		RTW_WARN(FUNC_ADPT_FMT" ChannelPlan ID:0x%02x, final chset has no channel\n"
			, FUNC_ADPT_ARG(padapter), ChannelPlan);

	return chanset_size;
}

u8 init_channel_set(_adapter *adapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	if (rfctl->regd_src == REGD_SRC_RTK_PRIV)
		return init_channel_set_from_rtk_priv(adapter, rfctl->channel_set);
#ifdef CONFIG_REGD_SRC_FROM_OS
	else if (rfctl->regd_src == REGD_SRC_OS)
		return rtw_os_init_channel_set(adapter, rfctl->channel_set);
#endif
	else
		rtw_warn_on(1);

	return 0;
}

#endif /* !HOST_CHPLAN_DATA_ONLY && (!CONFIG_RUST || HOST_CHPLAN_TEST) */

#if !defined(CONFIG_RUST) && !defined(HOST_CHPLAN_DATA_ONLY)

bool rtw_chset_is_dfs_range(struct _RT_CHANNEL_INFO *chset, u32 hi, u32 lo)
{
	u8 hi_ch = rtw_freq2ch(hi);
	u8 lo_ch = rtw_freq2ch(lo);
	int i;

	for (i = 0; i < MAX_CHANNEL_NUM && chset[i].ChannelNum != 0; i++){
		if (!(chset[i].flags & RTW_CHF_DFS))
			continue;
		if (hi_ch > chset[i].ChannelNum && lo_ch < chset[i].ChannelNum)
			return 1;
	}

	return 0;
}

bool rtw_chset_is_dfs_ch(struct _RT_CHANNEL_INFO *chset, u8 ch)
{
	int i;

	for (i = 0; i < MAX_CHANNEL_NUM && chset[i].ChannelNum != 0; i++){
		if (chset[i].ChannelNum == ch)
			return chset[i].flags & RTW_CHF_DFS ? 1 : 0;
	}

	return 0;
}

bool rtw_chset_is_dfs_chbw(struct _RT_CHANNEL_INFO *chset, u8 ch, u8 bw, u8 offset)
{
	u32 hi, lo;

	if (!rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo))
		return 0;

	return rtw_chset_is_dfs_range(chset, hi, lo);
}

const struct country_chplan *rtw_get_chplan_from_country(const char *country_code)
{
	const struct country_chplan *ent = NULL;
	const struct country_chplan *map = NULL;
	u16 map_sz = 0;
	char code[2];
	int i;

	code[0] = alpha_to_upper(country_code[0]);
	code[1] = alpha_to_upper(country_code[1]);

#ifdef CONFIG_CUSTOMIZED_COUNTRY_CHPLAN_MAP
	map = CUSTOMIZED_country_chplan_map;
	map_sz = sizeof(CUSTOMIZED_country_chplan_map) / sizeof(struct country_chplan);
#elif RTW_DEF_MODULE_REGULATORY_CERT
	map_sz = rtw_def_module_country_chplan_map(&map);
#else
	map = country_chplan_map;
	map_sz = rtw_country_chplan_map_size;
#endif

	for (i = 0; i < map_sz; i++) {
		if (strncmp(code, map[i].alpha2, 2) == 0) {
			ent = &map[i];
			break;
		}
	}

	return ent;
}

#endif /* !CONFIG_RUST && !HOST_CHPLAN_DATA_ONLY */
