/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_HT_RESTRUCTURE_TYPES_H
#define HOST_MLME_HT_RESTRUCTURE_TYPES_H

#include "host_types.h"
#include <stdbool.h>
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT5 0x20
#define BIT6 0x40

typedef unsigned int uint;
typedef int sint;
typedef uint32_t u32;

#define TEST_FLAG(_flag, _test) (((_flag) & (_test)) != 0)
#define SET_FLAG(_v, _f) ((_v) |= (_f))
#define rtw_warn_on(c) ((void)(c))
#define RTW_INFO(...) do { } while (0)
#define RTW_WARN(...) do { } while (0)
#define __FUNCTION__ "rtw_restructure_ht_ie"

enum channel_width {
	CHANNEL_WIDTH_20 = 0,
	CHANNEL_WIDTH_40 = 1,
	CHANNEL_WIDTH_80 = 2,
};

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HAL_PRIME_CHNL_OFFSET_UPPER 2

#define BW_CAP_40M BIT3
#define BW_MODE_2G(bw_mode) ((bw_mode) & 0x0F)
#define BW_MODE_5G(bw_mode) ((bw_mode) >> 4)
#define REGSTY_BW_2G(regsty) BW_MODE_2G((regsty)->bw_mode)
#define REGSTY_BW_5G(regsty) BW_MODE_5G((regsty)->bw_mode)
#define REGSTY_IS_BW_2G_SUPPORT(regsty, bw) (REGSTY_BW_2G((regsty)) >= (bw))
#define REGSTY_IS_BW_5G_SUPPORT(regsty, bw) (REGSTY_BW_5G((regsty)) >= (bw))

#define WIFI_STATION_STATE 0x00000008
#define check_fwstate(pmlmepriv, state) \
	((int)((((pmlmepriv)->fw_state) & (state)) != 0))

#define _HT_CAPABILITY_IE_ 45
#define _HT_ADD_INFO_IE_ 61
#define WLAN_EID_HT_OPERATION 61
#define WLAN_EID_HT_CAP 45
#define HT_CAP_IE_LEN 26
#define HT_OP_IE_LEN 22

#define IEEE80211_HT_CAP_LDPC_CODING 0x0001
#define IEEE80211_HT_CAP_SUP_WIDTH 0x0002
#define IEEE80211_HT_CAP_SM_PS 0x000C
#define IEEE80211_HT_CAP_SGI_20 0x0020
#define IEEE80211_HT_CAP_SGI_40 0x0040
#define IEEE80211_HT_CAP_TX_STBC 0x0080
#define IEEE80211_HT_CAP_MAX_AMSDU 0x0800
#define IEEE80211_HT_CAP_DSSSCCK40 0x1000
#define IEEE80211_HT_CAP_AMPDU_FACTOR 0x03
#define IEEE80211_HT_CAP_AMPDU_DENSITY 0x1C

#define LDPC_HT_ENABLE_RX BIT0
#define STBC_HT_ENABLE_TX BIT1
#define STBC_HT_ENABLE_RX BIT0

#define BEAMFORMING_HT_BEAMFORMER_ENABLE BIT0
#define BEAMFORMING_HT_BEAMFORMEE_ENABLE BIT1

#define MCS_RATE_1R 0x000000ff
#define MCS_RATE_2R 0x0000ffff
#define MCS_RATE_2R_13TO15_OFF 0x00001fff
#define MCS_RATE_3R 0x00ffffff
#define MCS_RATE_4R 0xffffffff

#define _AES_ 0x04
#define SCA 1
#define SCB 3
#define MAX_CHANNEL_NUM 59
#define IS_DFS_SLAVE_WITH_RD(rfctl) ((rfctl)->dfs_slave_with_rd)

#define LE_BITS_TO_1BYTE(p, o, l) \
	(((*((u8 *)(p)) >> (o)) & ((1U << (l)) - 1)))
#define SET_BITS_TO_LE_1BYTE(p, o, l, v)                                       \
	do {                                                                   \
		u8 *__x = (u8 *)(p);                                           \
		u8 __m = (u8)(((1U << (l)) - 1) << (o));                       \
		*__x = (*__x & ~__m) |                                         \
		       (((v) & ((1U << (l)) - 1)) << (o));                     \
	} while (0)
#define SET_BITS_TO_LE_4BYTE(p, o, l, v)                                       \
	do {                                                                   \
		u32 *__x = (u32 *)(p);                                         \
		u32 __m = (((1U << (l)) - 1) << (o));                          \
		*__x = (*__x & ~__m) | (((v) & ((1U << (l)) - 1)) << (o));     \
	} while (0)

#define GET_HT_OP_ELE_STA_CHL_WIDTH(p) LE_BITS_TO_1BYTE((p) + 1, 2, 1)
#define GET_HT_OP_ELE_2ND_CHL_OFFSET(p) LE_BITS_TO_1BYTE((p) + 1, 0, 2)
#define GET_HT_CAP_ELE_CHL_WIDTH(p) LE_BITS_TO_1BYTE(p, 1, 1)
#define SET_HT_CAP_ELE_RX_STBC(p, v) SET_BITS_TO_LE_1BYTE(((u8 *)(p)) + 1, 0, 2, (v))
#define SET_HT_CAP_TXBF_RECEIVE_NDP_CAP(p, v) \
	SET_BITS_TO_LE_4BYTE(((u8 *)(p)) + 21, 3, 1, (v))
#define SET_HT_CAP_TXBF_TRANSMIT_NDP_CAP(p, v) \
	SET_BITS_TO_LE_4BYTE(((u8 *)(p)) + 21, 4, 1, (v))
#define SET_HT_CAP_TXBF_EXPLICIT_COMP_STEERING_CAP(p, v) \
	SET_BITS_TO_LE_4BYTE(((u8 *)(p)) + 21, 10, 1, (v))
#define SET_HT_CAP_TXBF_EXPLICIT_COMP_FEEDBACK_CAP(p, v) \
	SET_BITS_TO_LE_4BYTE(((u8 *)(p)) + 21, 15, 2, (v))
#define SET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS(p, v) \
	SET_BITS_TO_LE_4BYTE(((u8 *)(p)) + 21, 23, 2, (v))
#define SET_HT_CAP_TXBF_CHNL_ESTIMATION_NUM_ANTENNAS(p, v) \
	SET_BITS_TO_LE_4BYTE(((u8 *)(p)) + 21, 27, 2, (v))

typedef enum {
	HAL_DEF_RX_PACKET_OFFSET = 1,
	HAL_DEF_MAX_RECVBUF_SZ = 2,
	HAL_DEF_RX_STBC = 3,
	HW_VAR_MAX_RX_AMPDU_FACTOR = 4,
	HW_VAR_BEST_AMPDU_DENSITY = 5,
	HAL_DEF_BEAMFORMER_CAP = 6,
	HAL_DEF_BEAMFORMEE_CAP = 7,
} hal_def_var_e;

typedef u8 HT_CAP_AMPDU_FACTOR;
typedef u8 HT_CAP_AMPDU_DENSITY;

struct rtw_ieee80211_ht_cap {
	u16 cap_info;
	u8 ampdu_params_info;
	u8 supp_mcs_set[16];
	u16 extended_ht_cap_info;
	u32 tx_BF_cap_info;
	u8 antenna_selection_info;
} __attribute__((packed));

struct ieee80211_ht_addt_info {
	u8 control_chan;
	u8 ht_param;
	u16 operation_mode;
	u16 stbc_param;
	u8 basic_set[16];
} __attribute__((packed));

typedef struct {
	u8 ChannelNum;
	u8 flags;
} RT_CHANNEL_INFO;

struct ht_priv {
	u8 ht_option;
	u8 sgi_20m;
	u8 sgi_40m;
	u8 ldpc_cap;
	u8 stbc_cap;
	u8 beamform_cap;
};

struct registry_priv {
	u8 bw_mode;
	u8 rx_stbc;
	u8 wifi_spec;
};

struct vht_priv {
	u8 _pad;
};

struct mlme_ext_info {
	u8 _pad;
};

struct mlme_priv {
	u32 fw_state;
	struct ht_priv htpriv;
	struct vht_priv vhtpriv;
};

struct mlme_ext_priv {
	u8 default_supported_mcs_set[16];
	u8 cur_bwmode;
	struct mlme_ext_info mlmext_info;
};

struct security_priv {
	u32 dot11PrivacyAlgrthm;
};

struct rf_ctl_t {
	u8 dfs_slave_with_rd;
	RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
};

struct host_ht_fixture {
	u8 hal_bw_cap_40;
	u8 chset_allow_40;
	u8 chbw_non_ocp_40;
	u8 dfs_domain_unknown;
	u8 rx_nss;
	u8 rx_stbc_nss;
	u8 beamformer_cap;
	u8 beamformee_cap;
	u32 rx_packet_offset;
	u32 max_recvbuf_sz;
	u8 max_rx_ampdu_factor;
	u8 best_ampdu_density;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct security_priv securitypriv;
	struct rf_ctl_t rf_ctl;
	u8 driver_rx_ampdu_factor;
	u8 driver_rx_ampdu_spacing;
	struct host_ht_fixture host_fixture;
};

typedef struct _adapter _adapter;

#define adapter_to_rfctl(a) (&(a)->rf_ctl)
#define GET_HAL_RX_NSS(a) ((a)->host_fixture.rx_nss)
#define GET_HAL_RFPATH(a) (0)

extern _adapter *host_mlme_ht_restructure_adapter;

bool hal_chk_bw_cap(_adapter *adapter, u8 cap);
void rtw_hal_get_def_var(_adapter *padapter, hal_def_var_e def_var, void *val);
void set_mcs_rate_by_mask(u8 *mcs_set, u32 mask);
u8 *rtw_get_ie(const u8 *pbuf, sint index, uint *len, sint limit);
u8 *rtw_set_ie(u8 *pbuf, sint index, uint len, const u8 *source, uint *frlen);
bool rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset,
			     u8 a, u8 b);
bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset);
u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl);

unsigned int rtw_restructure_ht_ie(_adapter *padapter, u8 *in_ie, u8 *out_ie,
				     uint in_len, uint *pout_len, u8 channel);

#endif /* HOST_MLME_HT_RESTRUCTURE_TYPES_H */
