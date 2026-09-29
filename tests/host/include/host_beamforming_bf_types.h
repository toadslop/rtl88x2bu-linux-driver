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

struct bf_host_info {
	struct bf_host_bfee bfee[BF_HOST_MAX_BFEE];
	struct bf_host_bfer bfer[BF_HOST_MAX_BFER];
	u8 bEnableSUTxBFWorkAround;
	struct bf_host_csi TargetCSIInfo;
	struct bf_host_bfee *TargetSUBFee;
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
