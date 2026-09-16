/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_INFO_TYPES_H
#define HOST_AP_STA_INFO_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))

#define TEST_FLAG(__Flag, __testFlag) (((__Flag) & (__testFlag)) != 0)
#define SET_FLAG(__Flag, __setFlag) ((__Flag) |= (__setFlag))

#define BEAMFORMING_HT_BEAMFORMER_ENABLE BIT(0)
#define BEAMFORMING_HT_BEAMFORMEE_ENABLE BIT(1)

#define BIT_LEN_MASK_32(__BitLen) ((u32)(0xFFFFFFFF >> (32 - (__BitLen))))

static inline u32 host_le_bits_to_4byte(const u8 *p, u32 off, u32 len)
{
	u32 val = 0;
	u32 i;

	for (i = 0; i < len; i++) {
		u32 byte_idx = (off + i) / 8;
		u32 bit_idx = (off + i) % 8;

		if (p[byte_idx] & (1U << bit_idx))
			val |= (1U << i);
	}
	return val;
}

#define LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	(host_le_bits_to_4byte(((const u8 *)(__pStart)), (__BitOffset), (__BitLen)) & \
	 BIT_LEN_MASK_32(__BitLen))

#define GET_HT_CAP_TXBF_EXPLICIT_COMP_STEERING_CAP(_pEleStart) \
	LE_BITS_TO_4BYTE(((u8 *)(_pEleStart)) + 21, 10, 1)
#define GET_HT_CAP_TXBF_EXPLICIT_COMP_FEEDBACK_CAP(_pEleStart) \
	LE_BITS_TO_4BYTE(((u8 *)(_pEleStart)) + 21, 15, 2)
#define GET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS(_pEleStart) \
	LE_BITS_TO_4BYTE(((u8 *)(_pEleStart)) + 21, 23, 2)
#define GET_HT_CAP_TXBF_CHNL_ESTIMATION_NUM_ANTENNAS(_pEleStart) \
	LE_BITS_TO_4BYTE(((u8 *)(_pEleStart)) + 21, 27, 2)

struct rtw_ieee80211_ht_cap {
	u16 cap_info;
	u8 ampdu_params_info;
	u8 supp_mcs_set[16];
	u16 extended_ht_cap_info;
	u32 tx_BF_cap_info;
	u8 antenna_selection_info;
} __attribute__((packed));

struct ht_priv {
	u8 ht_option;
	u8 beamform_cap;
	struct rtw_ieee80211_ht_cap ht_cap;
};

struct bf_cmn_info {
	u8 ht_beamform_cap;
};

struct cmn_sta_info {
	struct bf_cmn_info bf_info;
	u16 aid;
};

struct sta_info {
	struct cmn_sta_info cmn;
	struct ht_priv htpriv;
};

struct mlme_priv {
	struct ht_priv htpriv;
};

struct HT_caps_element {
	union {
		struct {
			u16 HT_caps_info;
			u8 AMPDU_para;
			u8 MCS_rate[16];
			u16 HT_ext_caps;
			u32 Beamforming_caps;
			u8 ASEL_caps;
		} HT_cap_element;
		u8 HT_cap[26];
	} u;
} __attribute__((packed));

struct mlme_ext_info {
	u8 _pad[191];
	u8 SM_PS;
	u8 _mid[53];
	struct HT_caps_element HT_caps;
};

struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
};

typedef struct _adapter _adapter;

#define WLAN_HT_CAP_SM_PS_STATIC 0

enum hw_var {
	HW_VAR_AMPDU_MIN_SPACE,
	HW_VAR_AMPDU_FACTOR,
};

void rtw_hal_set_hwreg(_adapter *padapter, enum hw_var variable, u8 *val);
void update_sta_info_apmode_ht_bf_cap(_adapter *padapter, struct sta_info *psta);
void update_hw_ht_param(_adapter *padapter);

#endif /* HOST_AP_STA_INFO_TYPES_H */
