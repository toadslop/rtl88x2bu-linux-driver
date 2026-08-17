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
#define _RTW_XMIT_REST_C_

#ifdef HOST_XMIT_TEST
#include "host_xmit_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_XMIT_TEST)

u8 rtw_get_tx_bw_mode(_adapter *adapter, struct sta_info *sta)
{
	u8 bw;

	bw = sta->cmn.bw_mode;
	if (MLME_STATE(adapter) & WIFI_ASOC_STATE) {
		if (adapter->mlmeextpriv.cur_channel <= 14)
			bw = rtw_min(bw, ADAPTER_TX_BW_2G(adapter));
		else
			bw = rtw_min(bw, ADAPTER_TX_BW_5G(adapter));
	}

	return bw;
}

void rtw_get_adapter_tx_rate_bmp_by_bw(_adapter *adapter, u8 bw, u16 *r_bmp_cck_ofdm, u32 *r_bmp_ht, u64 *r_bmp_vht)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);
	u8 fix_bw = 0xFF;
	u16 bmp_cck_ofdm = 0;
	u32 bmp_ht = 0;
	u64 bmp_vht = 0;
	int i;

	if (adapter->fix_rate != 0xFF && adapter->fix_bw != 0xFF)
		fix_bw = adapter->fix_bw;

	/* TODO: adapter->fix_rate */

	for (i = 0; i < macid_ctl->num; i++) {
		if (!rtw_macid_is_used(macid_ctl, i))
			continue;
		if (!rtw_macid_is_iface_specific(macid_ctl, i, adapter))
			continue;

		if (bw == CHANNEL_WIDTH_20) /* CCK, OFDM always 20MHz */
			bmp_cck_ofdm |= macid_ctl->rate_bmp0[i] & 0x00000FFF;

		/* bypass mismatch bandwidth for HT, VHT */
		if ((fix_bw != 0xFF && fix_bw != bw) || (fix_bw == 0xFF && macid_ctl->bw[i] != bw))
			continue;

		if (macid_ctl->vht_en[i])
			bmp_vht |= (macid_ctl->rate_bmp0[i] >> 12) | ((u64)macid_ctl->rate_bmp1[i] << 20);
		else
			bmp_ht |= (macid_ctl->rate_bmp0[i] >> 12) | (macid_ctl->rate_bmp1[i] << 20);
	}

	/* TODO: mlmeext->tx_rate*/

	if (r_bmp_cck_ofdm)
		*r_bmp_cck_ofdm = bmp_cck_ofdm;
	if (r_bmp_ht)
		*r_bmp_ht = bmp_ht;
	if (r_bmp_vht)
		*r_bmp_vht = bmp_vht;
}

void rtw_get_shared_macid_tx_rate_bmp_by_bw(struct dvobj_priv *dvobj, u8 bw, u16 *r_bmp_cck_ofdm, u32 *r_bmp_ht, u64 *r_bmp_vht)
{
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);
	u16 bmp_cck_ofdm = 0;
	u32 bmp_ht = 0;
	u64 bmp_vht = 0;
	int i;

	for (i = 0; i < macid_ctl->num; i++) {
		if (!rtw_macid_is_used(macid_ctl, i))
			continue;
		if (!rtw_macid_is_iface_shared(macid_ctl, i))
			continue;

		if (bw == CHANNEL_WIDTH_20) /* CCK, OFDM always 20MHz */
			bmp_cck_ofdm |= macid_ctl->rate_bmp0[i] & 0x00000FFF;

		/* bypass mismatch bandwidth for HT, VHT */
		if (macid_ctl->bw[i] != bw)
			continue;

		if (macid_ctl->vht_en[i])
			bmp_vht |= (macid_ctl->rate_bmp0[i] >> 12) | ((u64)macid_ctl->rate_bmp1[i] << 20);
		else
			bmp_ht |= (macid_ctl->rate_bmp0[i] >> 12) | (macid_ctl->rate_bmp1[i] << 20);
	}

	if (r_bmp_cck_ofdm)
		*r_bmp_cck_ofdm = bmp_cck_ofdm;
	if (r_bmp_ht)
		*r_bmp_ht = bmp_ht;
	if (r_bmp_vht)
		*r_bmp_vht = bmp_vht;
}

u8 rtw_get_tx_bw_bmp_of_ht_rate(struct dvobj_priv *dvobj, u8 rate, u8 max_bw)
{
	struct rf_ctl_t *rf_ctl = dvobj_to_rfctl(dvobj);
	u8 bw;
	u8 bw_bmp = 0;
	u32 rate_bmp;

	if (!IS_HT_RATE(rate)) {
		rtw_warn_on(1);
		goto exit;
	}

	rate_bmp = 1 << (rate - MGN_MCS0);

	if (max_bw > CHANNEL_WIDTH_40)
		max_bw = CHANNEL_WIDTH_40;

	for (bw = CHANNEL_WIDTH_20; bw <= max_bw; bw++) {
		/* RA may use lower rate for retry */
		if (rf_ctl->rate_bmp_ht_by_bw[bw] >= rate_bmp)
			bw_bmp |= ch_width_to_bw_cap(bw);
	}

exit:
	return bw_bmp;
}

u8 rtw_get_tx_bw_bmp_of_vht_rate(struct dvobj_priv *dvobj, u8 rate, u8 max_bw)
{
	struct rf_ctl_t *rf_ctl = dvobj_to_rfctl(dvobj);
	u8 bw;
	u8 bw_bmp = 0;
	u64 rate_bmp;

	if (!IS_VHT_RATE(rate)) {
		rtw_warn_on(1);
		goto exit;
	}

	rate_bmp = BIT_ULL(rate - MGN_VHT1SS_MCS0);

	if (max_bw > CHANNEL_WIDTH_160)
		max_bw = CHANNEL_WIDTH_160;

	for (bw = CHANNEL_WIDTH_20; bw <= max_bw; bw++) {
		/* RA may use lower rate for retry */
		if (rf_ctl->rate_bmp_vht_by_bw[bw] >= rate_bmp)
			bw_bmp |= ch_width_to_bw_cap(bw);
	}

exit:
	return bw_bmp;
}

#ifdef HOST_XMIT_TEST

void rtw_get_adapter_tx_rate_bmp(_adapter *adapter, u16 r_bmp_cck_ofdm[], u32 r_bmp_ht[],
				 u64 r_bmp_vht[])
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	u8 bw;
	u16 bmp_cck_ofdm, tmp_cck_ofdm;
	u32 bmp_ht, tmp_ht;
	u64 bmp_vht, tmp_vht;

	for (bw = CHANNEL_WIDTH_20; bw <= CHANNEL_WIDTH_160; bw++) {
		bmp_cck_ofdm = bmp_ht = bmp_vht = 0;
		if (hal_is_bw_support(adapter, bw)) {
			rtw_get_adapter_tx_rate_bmp_by_bw(adapter, bw, &tmp_cck_ofdm, &tmp_ht,
							&tmp_vht);
			bmp_cck_ofdm |= tmp_cck_ofdm;
			bmp_ht |= tmp_ht;
			bmp_vht |= tmp_vht;
			rtw_get_shared_macid_tx_rate_bmp_by_bw(dvobj, bw, &tmp_cck_ofdm, &tmp_ht,
							       &tmp_vht);
			bmp_cck_ofdm |= tmp_cck_ofdm;
			bmp_ht |= tmp_ht;
			bmp_vht |= tmp_vht;
		}
		if (bw == CHANNEL_WIDTH_20)
			r_bmp_cck_ofdm[bw] = bmp_cck_ofdm;
		if (bw <= CHANNEL_WIDTH_40)
			r_bmp_ht[bw] = bmp_ht;
		if (bw <= CHANNEL_WIDTH_160)
			r_bmp_vht[bw] = bmp_vht;
	}
}

u8 query_ra_short_GI(struct sta_info *psta, u8 bw)
{
	u8 sgi = _FALSE, sgi_20m = _FALSE, sgi_40m = _FALSE, sgi_80m = _FALSE;

#ifdef CONFIG_80211N_HT
#ifdef CONFIG_80211AC_VHT
	if (psta->vhtpriv.vht_option)
		sgi_80m = psta->vhtpriv.sgi_80m;
#endif
	sgi_20m = psta->htpriv.sgi_20m;
	sgi_40m = psta->htpriv.sgi_40m;
#endif

	switch (bw) {
	case CHANNEL_WIDTH_80:
		sgi = sgi_80m;
		break;
	case CHANNEL_WIDTH_40:
		sgi = sgi_40m;
		break;
	case CHANNEL_WIDTH_20:
	default:
		sgi = sgi_20m;
		break;
	}

	return sgi;
}

#endif /* HOST_XMIT_TEST */

#endif /* !CONFIG_RUST || HOST_XMIT_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_XMIT_TEST)

struct dvobj_priv *rtw_rust_xmit_adapter_dvobj(_adapter *adapter)
{
	return adapter_to_dvobj(adapter);
}

u8 rtw_rust_xmit_sta_bw_mode(struct sta_info *sta)
{
	return sta->cmn.bw_mode;
}

int rtw_rust_xmit_mlme_state(_adapter *adapter)
{
	return MLME_STATE(adapter);
}

u8 rtw_rust_xmit_cur_channel(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_channel;
}

u8 rtw_rust_xmit_driver_tx_bw_mode(_adapter *adapter)
{
	return adapter->driver_tx_bw_mode;
}

u8 rtw_rust_xmit_fix_rate(_adapter *adapter)
{
	return adapter->fix_rate;
}

u8 rtw_rust_xmit_fix_bw(_adapter *adapter)
{
	return adapter->fix_bw;
}

struct macid_ctl_t *rtw_rust_xmit_macid_ctl(struct dvobj_priv *dvobj)
{
	return dvobj_to_macidctl(dvobj);
}

struct rf_ctl_t *rtw_rust_xmit_rfctl(struct dvobj_priv *dvobj)
{
	return dvobj_to_rfctl(dvobj);
}

u8 rtw_rust_xmit_macid_num(struct macid_ctl_t *macid_ctl)
{
	return macid_ctl->num;
}

u8 rtw_rust_xmit_macid_bw(struct macid_ctl_t *macid_ctl, u8 id)
{
	return macid_ctl->bw[id];
}

u8 rtw_rust_xmit_macid_vht_en(struct macid_ctl_t *macid_ctl, u8 id)
{
	return macid_ctl->vht_en[id];
}

u32 rtw_rust_xmit_macid_rate_bmp0(struct macid_ctl_t *macid_ctl, u8 id)
{
	return macid_ctl->rate_bmp0[id];
}

u32 rtw_rust_xmit_macid_rate_bmp1(struct macid_ctl_t *macid_ctl, u8 id)
{
	return macid_ctl->rate_bmp1[id];
}

u32 rtw_rust_xmit_rf_ht_bmp(struct rf_ctl_t *rfctl, u8 bw)
{
	return rfctl->rate_bmp_ht_by_bw[bw];
}

u64 rtw_rust_xmit_rf_vht_bmp(struct rf_ctl_t *rfctl, u8 bw)
{
	return rfctl->rate_bmp_vht_by_bw[bw];
}

bool rtw_rust_xmit_hal_is_bw_support(_adapter *adapter, u8 bw)
{
	return hal_is_bw_support(adapter, bw);
}

u8 rtw_rust_xmit_sta_ht_sgi_20m(struct sta_info *sta)
{
	return sta->htpriv.sgi_20m;
}

u8 rtw_rust_xmit_sta_ht_sgi_40m(struct sta_info *sta)
{
	return sta->htpriv.sgi_40m;
}

u8 rtw_rust_xmit_sta_vht_option(struct sta_info *sta)
{
	return sta->vhtpriv.vht_option;
}

u8 rtw_rust_xmit_sta_vht_sgi_80m(struct sta_info *sta)
{
	return sta->vhtpriv.sgi_80m;
}

#endif /* CONFIG_RUST && !HOST_XMIT_TEST */
