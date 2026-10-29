// SPDX-License-Identifier: GPL-2.0
/* W3-192: kernel helpers for rust/rtw_beamforming_sound.rs (enter SU BFee path). */

#include <drv_types.h>
#include <hal_data.h>

#ifdef CONFIG_RUST

static void _rust_get_sta_beamform_cap(PADAPTER adapter, struct sta_info *sta,
				     u8 *sta_bf_cap, u8 *sounding_dim)
{
	struct beamforming_info *info = GET_BEAMFORM_INFO(adapter);
	struct ht_priv *ht = &adapter->mlmepriv.htpriv;
	u16 ht_bf_cap;

	*sta_bf_cap = 0;
	*sounding_dim = 0;
	if (is_supported_ht(sta->wireless_mode) == _FALSE)
		return;
	ht_bf_cap = ht->beamform_cap;
	if (TEST_FLAG(ht_bf_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE)) {
		info->beamforming_cap |= BEAMFORMER_CAP_HT_EXPLICIT;
		*sta_bf_cap |= BEAMFORMEE_CAP_HT_EXPLICIT;
		*sounding_dim = (ht_bf_cap & BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP) >> 6;
	}
}

static struct beamformee_entry *_rust_bfee_get_entry_by_addr(PADAPTER adapter, u8 *ra)
{
	struct beamforming_info *info = GET_BEAMFORM_INFO(adapter);
	struct beamformee_entry *bfee;
	u8 i;

	for (i = 0; i < MAX_BEAMFORMEE_ENTRY_NUM; i++) {
		bfee = &info->bfee_entry[i];
		if (bfee->used == _FALSE)
			continue;
		if (_rtw_memcmp(ra, bfee->mac_addr, ETH_ALEN) == _TRUE)
			return bfee;
	}
	return NULL;
}

static struct beamformee_entry *_rust_bfee_get_free_entry(PADAPTER adapter)
{
	struct beamforming_info *info = GET_BEAMFORM_INFO(adapter);
	u8 i;

	for (i = 0; i < MAX_BEAMFORMEE_ENTRY_NUM; i++) {
		if (info->bfee_entry[i].used == _FALSE)
			return &info->bfee_entry[i];
	}
	return NULL;
}

static struct beamformee_entry *_rust_bfee_add_entry(PADAPTER adapter, struct sta_info *sta,
						     u8 bf_cap, u8 sounding_dim)
{
	struct beamforming_info *info = GET_BEAMFORM_INFO(adapter);
	struct beamformee_entry *bfee;
	u8 i;
	u8 is_new;

	bfee = _rust_bfee_get_entry_by_addr(adapter, sta->cmn.mac_addr);
	is_new = (bfee == NULL) ? _TRUE : _FALSE;
	if (!bfee) {
		bfee = _rust_bfee_get_free_entry(adapter);
		if (!bfee)
			return NULL;
	}

	bfee->used = _TRUE;
	bfee->aid = sta->cmn.aid;
	bfee->mac_id = sta->cmn.mac_id;
	_rtw_memcpy(bfee->mac_addr, sta->cmn.mac_addr, ETH_ALEN);
	bfee->cap = bf_cap;
	bfee->state = BEAMFORM_ENTRY_HW_STATE_ADD_INIT;
	bfee->bApplySounding = _TRUE;
	bfee->sound_period = 40;
	bfee->SoundCnt = GetInitSoundCnt(bfee->sound_period, info->sounding_info.min_sounding_period);
	bfee->NumofSoundingDim = sounding_dim;

	if (is_new && TEST_FLAG(bf_cap, BEAMFORMEE_CAP_HT_EXPLICIT)) {
		info->beamformee_su_cnt += 1;
		for (i = 0; i < MAX_NUM_BEAMFORMEE_SU; i++) {
			if ((info->beamformee_su_reg_maping & BIT(i)) == 0) {
				info->beamformee_su_reg_maping |= BIT(i);
				bfee->su_reg_index = i;
				break;
			}
		}
	}
	return bfee;
}

static u8 _rust_enter_ht_su_bfee_only(u8 sta_bf_cap)
{
	if (sta_bf_cap == BEAMFORMING_CAP_NONE)
		return _FALSE;
	if (!TEST_FLAG(sta_bf_cap, BEAMFORMEE_CAP_HT_EXPLICIT))
		return _FALSE;
	if (TEST_FLAG(sta_bf_cap, BEAMFORMER_CAP_HT_EXPLICIT)
	    || TEST_FLAG(sta_bf_cap, BEAMFORMEE_CAP_VHT_SU)
	    || TEST_FLAG(sta_bf_cap, BEAMFORMEE_CAP_VHT_MU)
	    || TEST_FLAG(sta_bf_cap, BEAMFORMER_CAP_VHT_SU))
		return _FALSE;
	return _TRUE;
}

u8 rtw_rust_beamforming_enter_impl(PADAPTER adapter, void *p)
{
	struct sta_info *sta_copy, *sta;
	struct beamforming_info *info;
	u8 sta_bf_cap, sounding_dim = 0;
	struct beamformee_entry *bfee;
	u8 wireless_mode;

	if (!adapter || !p)
		return _FALSE;
	if (check_fwstate(&adapter->mlmepriv, WIFI_AP_STATE) == _TRUE)
		return _FALSE;

	sta_copy = (struct sta_info *)p;
	sta = rtw_get_stainfo(&adapter->stapriv, sta_copy->cmn.mac_addr);
	if (!sta)
		return _FALSE;

	wireless_mode = sta->wireless_mode;
	if ((is_supported_ht(wireless_mode) == _FALSE)
	    && (is_supported_vht(wireless_mode) == _FALSE))
		return _FALSE;

	if (adapter->mlmepriv.htpriv.beamform_cap == 0
#ifdef CONFIG_80211AC_VHT
	    && adapter->mlmepriv.vhtpriv.beamform_cap == 0
#endif
	   )
		return _FALSE;

	info = GET_BEAMFORM_INFO(adapter);
	_rust_get_sta_beamform_cap(adapter, sta, &sta_bf_cap, &sounding_dim);
	if (_rust_enter_ht_su_bfee_only(sta_bf_cap) == _FALSE)
		return _FALSE;

	sta_bf_cap |= BEAMFORMEE_CAP;

	bfee = _rust_bfee_add_entry(adapter, sta, sta_bf_cap, sounding_dim);
	if (!bfee)
		return _FALSE;

	rtw_hal_set_hwreg(adapter, HW_VAR_SOUNDING_ENTER, (u8 *)sta);
	if (info->beamformee_su_cnt != 0 && info->sounding_info.state == SOUNDING_STATE_NONE) {
		info->sounding_info.state = SOUNDING_STATE_INIT;
		_set_timer(&info->sounding_timer, 2000);
	}
	return _TRUE;
}

#endif /* CONFIG_RUST */
