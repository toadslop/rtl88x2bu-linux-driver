/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_TXVECTOR_CAP_ORACLE_H
#define HOST_BEAMFORMING_BF_TXVECTOR_CAP_ORACLE_H

#include "host_beamforming_bf_types.h"

#define BF_HOST_TXVEC_WIFI_AP_STATE 0x00000010U
#define BF_HOST_TXVEC_WIFI_ADHOC_STATE 0x00000020U
#define BF_HOST_TXVEC_WIFI_ADHOC_MASTER_STATE 0x00000040U

#define BF_HOST_TXVEC_HT_BFER_EN (1u << 0)
#define BF_HOST_TXVEC_HT_BFEE_EN (1u << 1)
#define BF_HOST_TXVEC_HT_BFER_STEER ((1u << 4) | (1u << 5))
#define BF_HOST_TXVEC_HT_BFEE_CHNL ((1u << 6) | (1u << 7))

#define BF_HOST_TXVEC_VHT_BFER_EN (1u << 0)
#define BF_HOST_TXVEC_VHT_BFEE_EN (1u << 1)
#define BF_HOST_TXVEC_VHT_MU_STA (1u << 3)
#define BF_HOST_TXVEC_VHT_MU_AP (1u << 2)
#define BF_HOST_TXVEC_VHT_BFER_STS ((1u << 8) | (1u << 9) | (1u << 10))
#define BF_HOST_TXVEC_VHT_BFEE_DIM ((1u << 12) | (1u << 13) | (1u << 14))

#define BF_HOST_TXVEC_BFER_HT_EXPLICIT 0x1u
#define BF_HOST_TXVEC_BFEE_HT_EXPLICIT 0x2u
#define BF_HOST_TXVEC_BFER_VHT_SU 0x4u
#define BF_HOST_TXVEC_BFEE_VHT_SU 0x8u
#define BF_HOST_TXVEC_BFER_VHT_MU 0x10u
#define BF_HOST_TXVEC_BFEE_VHT_MU 0x20u

struct bf_host_txvec_sta_ht {
	u16 beamform_cap;
};

struct bf_host_txvec_sta_vht {
	u16 beamform_cap;
};

struct bf_host_txvec_sta_cmn {
	u8 mac_addr[BF_HOST_ETH_ALEN];
	u16 aid;
};

struct bf_host_txvec_sta {
	u8 wireless_mode;
	struct bf_host_txvec_sta_cmn cmn;
	struct bf_host_txvec_sta_ht htpriv;
	struct bf_host_txvec_sta_vht vhtpriv;
};

struct bf_host_txvec_mlme {
	u32 fw_state;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	u16 ht_beamform_cap;
	u16 vht_beamform_cap;
};

struct bf_host_txvec_adpt {
	struct bf_host_txvec_mlme mlmepriv;
	u8 beamforming_cap;
	u8 _pad[3];
	struct bf_host_txvec_sta sta;
};

typedef struct bf_host_txvec_adpt *bf_host_txvec_padpt;

#endif
