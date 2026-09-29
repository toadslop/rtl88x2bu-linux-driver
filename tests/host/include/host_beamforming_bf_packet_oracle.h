/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_PACKET_ORACLE_H
#define HOST_BEAMFORMING_BF_PACKET_ORACLE_H

#include "host_beamforming_bf_oracle.h"

#define BF_HOST_SUCCESS 1
#define BF_HOST_FAIL 0
#define BF_HOST_CAT_VHT 21
#define BF_HOST_CAT_HT 0
#define BF_HOST_ACT_VHT_BF 0
#define BF_HOST_ACT_HT_BF 6
#define BF_HOST_CMD_SET_CSI 7

struct bf_host_cmd_tr {
	unsigned count;
	u8 last_type;
};

extern struct bf_host_cmd_tr bf_host_cmd_tr;

void bf_host_cmd(bf_host_padpt a, int type, u8 *p, int sz, u8 enq);

static inline u8 *bf_host_addr2(u8 *p)
{
	return p + 10;
}

#ifndef HOST_BF_ENTRY_PACKET_RUST
static inline void o_ndpa(bf_host_padpt a, struct bf_host_recv_frame *f)
{
	(void)a;
	(void)f;
}

static inline u32 o_report(bf_host_padpt adapter, struct bf_host_recv_frame *rf)
{
	struct bf_host_info *info = BF_GET_INFO(adapter);
	struct bf_host_bfee *bfee;
	u8 *pframe, *ta, *body, *mimo;
	u8 Nc = 0, Nr = 0, CH_W = 0, Ng = 0, CodeBook = 0;
	u8 cat, act;

	pframe = rf->hdr.data;
	ta = bf_host_addr2(pframe);
	bfee = bf_host_bfee_by_addr(adapter, ta);
	if (!bfee)
		return BF_HOST_FAIL;
	body = pframe + 24;
	cat = body[0];
	act = body[1];
	if (cat == BF_HOST_CAT_VHT && act == BF_HOST_ACT_VHT_BF) {
		mimo = pframe + 26;
		Nc = *mimo & 0x7;
		Nr = (*mimo & 0x38) >> 3;
		CH_W = (*mimo & 0xC0) >> 6;
		Ng = *(mimo + 1) & 0x3;
		CodeBook = ((*(mimo + 1)) & 0x4) >> 2;
		info->TargetCSIInfo.bVHT = 1;
	} else if (cat == BF_HOST_CAT_HT && act == BF_HOST_ACT_HT_BF) {
		mimo = pframe + 26;
		Nc = *mimo & 0x3;
		Nr = (*mimo & 0xC) >> 2;
		CH_W = (*mimo & 0x10) >> 4;
		Ng = (*mimo & 0x60) >> 5;
		CodeBook = ((*(mimo + 1)) & 0x6) >> 1;
		info->TargetCSIInfo.bVHT = 0;
	}
	if (info->bEnableSUTxBFWorkAround && info->TargetSUBFee == bfee) {
		struct bf_host_csi *csi = &info->TargetCSIInfo;

		if (csi->Nc != Nc || csi->Nr != Nr || csi->ChnlWidth != CH_W ||
		    csi->Ng != Ng || csi->CodeBook != CodeBook) {
			csi->Nc = Nc;
			csi->Nr = Nr;
			csi->ChnlWidth = CH_W;
			csi->Ng = Ng;
			csi->CodeBook = CodeBook;
			bf_host_cmd(adapter, BF_HOST_CMD_SET_CSI, (u8 *)csi, sizeof(*csi), 1);
		}
	}
	return BF_HOST_SUCCESS;
}
#else
void o_ndpa(bf_host_padpt a, struct bf_host_recv_frame *f);
u32 o_report(bf_host_padpt adapter, struct bf_host_recv_frame *rf);
#endif

#endif
