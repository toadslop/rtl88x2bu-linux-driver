/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_TYPES_H
#define HOST_BEAMFORMING_BF_TYPES_H

#include "host_types.h"

#define BF_HOST_MAX_BFEE 8
#define BF_HOST_MAX_BFER 3
#define BF_HOST_ETH_ALEN 6
#define BF_HOST_CAP_NONE 0

enum bf_host_cap { BF_HOST_BFEE_VHT_SU = 0x8 };

struct bf_host_bfee {
	u8 used;
	u16 mac_id;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	enum bf_host_cap cap;
};

struct bf_host_bfer {
	u8 used;
	u8 mac_addr[BF_HOST_ETH_ALEN];
};

struct bf_host_csi {
	u8 Nc, Nr, Ng, CodeBook, ChnlWidth, bVHT;
};

#define BF_HOST_SU_SOUND 2
#define BF_HOST_MU_SOUND 6

struct bf_host_sounding_info {
	u8 su_sounding_list[BF_HOST_SU_SOUND];
	u8 mu_sounding_list[BF_HOST_MU_SOUND];
	u8 state;
	u8 su_bfee_curidx;
	u8 candidate_mu_bfee_cnt;
	u16 min_sounding_period;
	u8 sound_remain_cnt_per_period;
};

struct bf_host_info {
	struct bf_host_bfee bfee[BF_HOST_MAX_BFEE];
	struct bf_host_bfer bfer[BF_HOST_MAX_BFER];
	u8 bEnableSUTxBFWorkAround;
	struct bf_host_csi TargetCSIInfo;
	struct bf_host_bfee *TargetSUBFee;
	u8 beamforming_cap;
	u8 beamforming_state;
	u8 sounding_sequence;
	u8 beamformee_su_cnt;
	u8 beamformer_su_cnt;
	u32 beamformee_su_reg_maping;
	u32 beamformer_su_reg_maping;
	u8 beamformee_mu_cnt;
	u8 beamformer_mu_cnt;
	u32 beamformee_mu_reg_maping;
	u8 first_mu_bfee_index;
	u8 mu_bfer_curidx;
	u8 cur_csi_rpt_rate;
	struct bf_host_sounding_info sounding_info;
	u8 SetHalBFEnterOnDemandCnt;
	u8 SetHalBFLeaveOnDemandCnt;
	u8 SetHalSoundownOnDemandCnt;
	s8 sounding_running;
	u8 timer_inits;
};

struct bf_host_rx {
	u32 len;
	u8 data[256];
};

struct bf_host_recv_frame {
	struct bf_host_rx hdr;
};

struct bf_host_hal {
	struct bf_host_info beamforming_info;
};

struct bf_host_mlme {
	u8 pad;
	u8 mac_addr[BF_HOST_ETH_ALEN];
	u8 bssid[BF_HOST_ETH_ALEN];
};

struct bf_host_adpt {
	struct bf_host_mlme mlmepriv;
	struct bf_host_hal hal;
};

typedef struct bf_host_adpt *bf_host_padpt;

#define BF_GET_INFO(a) (&(a)->hal.beamforming_info)
#define bf_host_mlme_to_adpt(m) \
	((bf_host_padpt)((char *)(m) - offsetof(struct bf_host_adpt, mlmepriv)))

#endif
