/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_GID_ORACLE_H
#define HOST_BEAMFORMING_BF_GID_ORACLE_H

#include <string.h>

#include "host_beamforming_bf_types.h"

#define BF_HOST_CAT_VHT_GID 21
#define BF_HOST_ACT_VHT_GID 1

struct bf_host_gid_xmit_tr {
	u8 ok;
	u8 ra[BF_HOST_ETH_ALEN];
	u8 gid[8];
	u8 position[16];
	u8 frame[64];
	u16 pktlen;
};

struct bf_host_gid_set_tr {
	unsigned count;
	u8 ta[BF_HOST_ETH_ALEN];
	u8 gid[8];
	u8 position[16];
};

extern struct bf_host_gid_xmit_tr bf_host_gid_xmit_tr;
extern struct bf_host_gid_set_tr bf_host_gid_set_tr;

void bf_host_bfer_set_gid(bf_host_padpt a, u8 *ta, u8 *gid, u8 *pos);

#ifndef HOST_BF_GID_RUST
static inline u8 o_bf_send_vht_gid_mgnt(bf_host_padpt adapter, u8 *ra, u8 *gid,
					u8 *position)
{
	u8 *pframe = bf_host_gid_xmit_tr.frame;
	struct bf_host_mlme *mlmepriv = &adapter->mlmepriv;

	memset(&bf_host_gid_xmit_tr, 0, sizeof(bf_host_gid_xmit_tr));
	memcpy(bf_host_gid_xmit_tr.ra, ra, BF_HOST_ETH_ALEN);
	memcpy(bf_host_gid_xmit_tr.gid, gid, 8);
	memcpy(bf_host_gid_xmit_tr.position, position, 16);
	memcpy(pframe + 4, ra, BF_HOST_ETH_ALEN);
	memcpy(pframe + 10, mlmepriv->mac_addr, BF_HOST_ETH_ALEN);
	memcpy(pframe + 16, mlmepriv->bssid, BF_HOST_ETH_ALEN);
	pframe[24] = BF_HOST_CAT_VHT_GID;
	pframe[25] = BF_HOST_ACT_VHT_GID;
	memcpy(pframe + 26, gid, 8);
	memcpy(pframe + 34, position, 16);
	bf_host_gid_xmit_tr.pktlen = 54;
	bf_host_gid_xmit_tr.ok = 1;
	return 1;
}

static inline void o_bf_get_vht_gid_mgnt(bf_host_padpt adapter,
					 struct bf_host_recv_frame *rf)
{
	u8 *pframe = rf->hdr.data;
	u8 ta[BF_HOST_ETH_ALEN];

	memcpy(ta, pframe + 10, BF_HOST_ETH_ALEN);
	ta[0] &= 0xFE;
	bf_host_bfer_set_gid(adapter, ta, pframe + 26, pframe + 34);
}
#else
u8 o_bf_send_vht_gid_mgnt(bf_host_padpt adapter, u8 *ra, u8 *gid, u8 *position);
void o_bf_get_vht_gid_mgnt(bf_host_padpt adapter, struct bf_host_recv_frame *rf);
#endif

#endif
