// SPDX-License-Identifier: GPL-2.0
/* W3-201 host C oracle bodies (shared by L2 + L1 ref). */

#include "host_beamforming_bf_txvector_cap_oracle.h"

#define WIRELESS_11_24N (1u << 3)
#define WIRELESS_11_5N (1u << 4)
#define WIRELESS_11AC (1u << 6)

#define BF_HOST_TXVEC_TRUE 1u
#define BF_HOST_TXVEC_FALSE 0u

static u8 txvec_chk_fw(struct bf_host_txvec_mlme *mlme, u32 state)
{
	if ((mlme->fw_state & state) != 0)
		return BF_HOST_TXVEC_TRUE;
	return BF_HOST_TXVEC_FALSE;
}

static u8 txvec_supported_ht(u8 mode)
{
	return (mode & (WIRELESS_11_24N | WIRELESS_11_5N)) ? BF_HOST_TXVEC_TRUE : BF_HOST_TXVEC_FALSE;
}

static u8 txvec_supported_vht(u8 mode)
{
	return (mode & WIRELESS_11AC) ? BF_HOST_TXVEC_TRUE : BF_HOST_TXVEC_FALSE;
}

#ifdef BF_TXVECTOR_CAP_KERN_REF
#define BF_TXVEC_ORACLE_LINKAGE static
#else
#define BF_TXVEC_ORACLE_LINKAGE
#endif

BF_TXVEC_ORACLE_LINKAGE void o_rtw_bf_get_txvector_parameter(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
				     u8 *g_id, u16 *p_aid)
{
	struct bf_host_txvec_mlme *mlme;
	u16 aid;
	u8 *bssid;
	u16 val16;

	mlme = &adapter->mlmepriv;

	if (txvec_chk_fw(mlme, BF_HOST_TXVEC_WIFI_AP_STATE)) {
		aid = sta->cmn.aid;
		bssid = mlme->mac_addr;
		aid &= 0x1FF;
		val16 = ((bssid[5] & 0xF0) >> 4) ^ (bssid[5] & 0xF);
		*p_aid = (aid + (val16 << 5)) & 0x1FF;
		*g_id = 63;
	} else if (txvec_chk_fw(mlme, BF_HOST_TXVEC_WIFI_ADHOC_STATE) ||
		   txvec_chk_fw(mlme, BF_HOST_TXVEC_WIFI_ADHOC_MASTER_STATE)) {
		*p_aid = 0;
		*g_id = 63;
	} else {
		bssid = sta->cmn.mac_addr;
		*p_aid = (bssid[5] << 1) | (bssid[4] >> 7);
		*g_id = 0;
	}
}

BF_TXVEC_ORACLE_LINKAGE void o_rtw_bf_get_sta_beamform_cap(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
				   u8 *sta_bf_cap, u8 *sounding_dim, u8 *comp_steering)
{
	struct bf_host_txvec_mlme *mlme;
	u16 ht_bf_cap;
	u16 vht_bf_cap;

	*sta_bf_cap = 0;
	*sounding_dim = 0;
	*comp_steering = 0;

	mlme = &adapter->mlmepriv;

	if (txvec_supported_ht(sta->wireless_mode) == BF_HOST_TXVEC_FALSE)
		return;

	if (txvec_chk_fw(mlme, BF_HOST_TXVEC_WIFI_AP_STATE)) {
		ht_bf_cap = sta->htpriv.beamform_cap;
		if (ht_bf_cap & BF_HOST_TXVEC_HT_BFEE_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFER_HT_EXPLICIT;
			*sta_bf_cap |= BF_HOST_TXVEC_BFEE_HT_EXPLICIT;
			*comp_steering = (ht_bf_cap & BF_HOST_TXVEC_HT_BFER_STEER) >> 4;
		}
		if (ht_bf_cap & BF_HOST_TXVEC_HT_BFER_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFEE_HT_EXPLICIT;
			*sta_bf_cap |= BF_HOST_TXVEC_BFER_HT_EXPLICIT;
			*sounding_dim = (ht_bf_cap & BF_HOST_TXVEC_HT_BFEE_CHNL) >> 6;
		}
	} else {
		ht_bf_cap = mlme->ht_beamform_cap;
		if (ht_bf_cap & BF_HOST_TXVEC_HT_BFEE_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFEE_HT_EXPLICIT;
			*sta_bf_cap |= BF_HOST_TXVEC_BFER_HT_EXPLICIT;
			*sounding_dim = (ht_bf_cap & BF_HOST_TXVEC_HT_BFEE_CHNL) >> 6;
		}
		if (ht_bf_cap & BF_HOST_TXVEC_HT_BFER_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFER_HT_EXPLICIT;
			*sta_bf_cap |= BF_HOST_TXVEC_BFEE_HT_EXPLICIT;
			*comp_steering = (ht_bf_cap & BF_HOST_TXVEC_HT_BFER_STEER) >> 4;
		}
	}

	if (txvec_supported_vht(sta->wireless_mode) == BF_HOST_TXVEC_FALSE)
		return;

	if (txvec_chk_fw(mlme, BF_HOST_TXVEC_WIFI_AP_STATE)) {
		vht_bf_cap = sta->vhtpriv.beamform_cap;
		if (vht_bf_cap & BF_HOST_TXVEC_VHT_BFEE_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFER_VHT_SU;
			*sta_bf_cap |= BF_HOST_TXVEC_BFEE_VHT_SU;
			if (vht_bf_cap & BF_HOST_TXVEC_VHT_MU_STA) {
				adapter->beamforming_cap |= BF_HOST_TXVEC_BFER_VHT_MU;
				*sta_bf_cap |= BF_HOST_TXVEC_BFEE_VHT_MU;
			}
			*comp_steering = (vht_bf_cap & BF_HOST_TXVEC_VHT_BFER_STS) >> 8;
		}
		if (vht_bf_cap & BF_HOST_TXVEC_VHT_BFER_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFEE_VHT_SU;
			*sta_bf_cap |= BF_HOST_TXVEC_BFER_VHT_SU;
			*sounding_dim = (vht_bf_cap & BF_HOST_TXVEC_VHT_BFEE_DIM) >> 12;
		}
	} else {
		vht_bf_cap = mlme->vht_beamform_cap;
		if (vht_bf_cap & BF_HOST_TXVEC_VHT_BFEE_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFEE_VHT_SU;
			*sta_bf_cap |= BF_HOST_TXVEC_BFER_VHT_SU;
			if (vht_bf_cap & BF_HOST_TXVEC_VHT_MU_STA) {
				adapter->beamforming_cap |= BF_HOST_TXVEC_BFEE_VHT_MU;
				*sta_bf_cap |= BF_HOST_TXVEC_BFER_VHT_MU;
			}
			*sounding_dim = (vht_bf_cap & BF_HOST_TXVEC_VHT_BFEE_DIM) >> 12;
		}
		if (vht_bf_cap & BF_HOST_TXVEC_VHT_BFER_EN) {
			adapter->beamforming_cap |= BF_HOST_TXVEC_BFER_VHT_SU;
			*sta_bf_cap |= BF_HOST_TXVEC_BFEE_VHT_SU;
			*comp_steering = (vht_bf_cap & BF_HOST_TXVEC_VHT_BFER_STS) >> 8;
		}
	}
}
