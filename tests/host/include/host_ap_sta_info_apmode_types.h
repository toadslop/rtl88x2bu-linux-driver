/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_INFO_APMODE_TYPES_H
#define HOST_AP_STA_INFO_APMODE_TYPES_H

#include "host_types.h"
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))

#define TEST_FLAG(__Flag, __testFlag) (((__Flag) & (__testFlag)) != 0)
#define SET_FLAG(__Flag, __setFlag) ((__Flag) |= (__setFlag))

#define MLME_IS_MESH(a) 0

#define dot11AuthAlgrthm_Open 0
#define dot11AuthAlgrthm_8021X 2

#define WIFI_ASOC_STATE 0x00000001U
#define WIFI_UNDER_KEY_HANDSHAKE 0x01000000U

#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_40 1

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0

#define IEEE80211_HT_CAP_SUP_WIDTH 0x0002
#define IEEE80211_HT_CAP_SGI_20 0x0020
#define IEEE80211_HT_CAP_SGI_40 0x0040
#define IEEE80211_HT_CAP_AMPDU_DENSITY 0x1c

#define LDPC_HT_ENABLE_TX BIT(1)
#define LDPC_HT_CAP_TX BIT(3)
#define STBC_HT_ENABLE_TX BIT(1)
#define STBC_HT_CAP_TX BIT(3)

#define HT_OP_IE_LEN 22

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

#define LE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	((u8)LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen))

#define GET_HT_CAP_ELE_LDPC_CAP(_pEleStart) LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)), 0, 1)
#define GET_HT_CAP_ELE_RX_STBC(_pEleStart) LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 1, 0, 2)
#define GET_HT_OP_ELE_STA_CHL_WIDTH(_pEleStart) LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 1, 2, 1)

static inline u16 cpu_to_le16(u16 x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return x;
#else
	return (u16)((x >> 8) | (x << 8));
#endif
}

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
	u8 ampdu_enable;
	u8 rx_ampdu_min_spacing;
	u8 ch_offset;
	u8 sgi_20m;
	u8 sgi_40m;
	u8 agg_enable_bitmap;
	u8 candidate_tid_bitmap;
	u8 ldpc_cap;
	u8 stbc_cap;
	u8 op_present;
	struct rtw_ieee80211_ht_cap ht_cap;
	u8 ht_op[HT_OP_IE_LEN];
};

struct rtw_ra_info {
	u8 is_support_sgi;
};

struct cmn_sta_info {
	u8 bw_mode;
	u16 aid;
	u8 mac_addr[6];
	struct rtw_ra_info ra_info;
};

struct stainfo_stats {
	u64 rx_data_pkts;
};

struct _lock {
	int dummy;
};

struct sta_info {
	struct cmn_sta_info cmn;
	struct ht_priv htpriv;
	u32 ieee8021x_blocked;
	u32 qos_option;
	u8 ht_40mhz_intolerant;
	u32 state;
	struct stainfo_stats sta_stats;
	struct _lock lock;
};

struct security_priv {
	u32 dot11AuthAlgrthm;
};

struct mlme_ext_priv {
	u8 cur_bwmode;
	u8 cur_ch_offset;
};

struct mlme_priv {
	struct ht_priv htpriv;
};

typedef struct {
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct security_priv securitypriv;
} _adapter;

typedef u32 _irqL;

#define _enter_critical_bh(lock, irq) ((void)(lock), (void)(irq))
#define _exit_critical_bh(lock, irq) ((void)(lock), (void)(irq))
#define _rtw_memset(p, c, n) memset((p), (c), (n))

enum hal_odm_var {
	HAL_ODM_STA_INFO = 0,
};

void VCS_update(_adapter *padapter, struct sta_info *psta);
void send_delba(_adapter *padapter, int initiator, u8 *addr);
u8 query_ra_short_GI(struct sta_info *psta, u8 bw);
u8 rtw_get_tx_bw_mode(_adapter *padapter, struct sta_info *psta);
void update_ldpc_stbc_cap(struct sta_info *psta);
void rtw_hal_set_odm_var(_adapter *padapter, enum hal_odm_var variable, struct sta_info *psta, u8 val);

void update_sta_vht_info_apmode(_adapter *padapter, void *psta);
void update_sta_info_apmode(_adapter *padapter, struct sta_info *psta);

void host_apmode_reset(void);
u8 host_apmode_vcs_calls(void);
u8 host_apmode_delba_calls(void);
u8 host_apmode_odm_calls(void);
void host_apmode_set_ra_sgi(u8 sgi);
void host_apmode_set_tx_bw(u8 bw);

#endif /* HOST_AP_STA_INFO_APMODE_TYPES_H */
