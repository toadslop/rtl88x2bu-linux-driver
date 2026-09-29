/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_ORACLE_H
#define HOST_BEAMFORMING_BF_ORACLE_H

#include <string.h>

#include "host_beamforming_bf_types.h"

static inline int bf_host_mac_eq(const u8 *a, const u8 *b)
{
	return memcmp(a, b, BF_HOST_ETH_ALEN) == 0;
}

static inline struct bf_host_bfer *bf_host_bfer_by_addr(bf_host_padpt a, u8 *ra)
{
	struct bf_host_info *info = BF_GET_INFO(a);
	u8 i;

	for (i = 0; i < BF_HOST_MAX_BFER; i++) {
		if (!info->bfer[i].used)
			continue;
		if (bf_host_mac_eq(ra, info->bfer[i].mac_addr))
			return &info->bfer[i];
	}
	return NULL;
}

static inline struct bf_host_bfee *bf_host_bfee_by_addr(bf_host_padpt a, u8 *ra)
{
	struct bf_host_info *info = BF_GET_INFO(a);
	u8 i;

	for (i = 0; i < BF_HOST_MAX_BFEE; i++) {
		if (!info->bfee[i].used)
			continue;
		if (bf_host_mac_eq(ra, info->bfee[i].mac_addr))
			return &info->bfee[i];
	}
	return NULL;
}

static inline enum bf_host_cap bf_host_cap_by_macid(bf_host_padpt a, u8 macid)
{
	struct bf_host_info *info = BF_GET_INFO(a);
	u8 i;

	for (i = 0; i < BF_HOST_MAX_BFER; i++) {
		if (!info->bfee[i].used)
			continue;
		if (info->bfee[i].mac_id == macid)
			return info->bfee[i].cap;
	}
	return BF_HOST_CAP_NONE;
}

#ifndef HOST_BF_ENTRY_PACKET_RUST
static inline enum bf_host_cap o_cap_by_macid(void *mlme, u8 macid)
{
	return bf_host_cap_by_macid(bf_host_mlme_to_adpt((struct bf_host_mlme *)mlme), macid);
}

static inline struct bf_host_bfer *o_bfer_by_addr(bf_host_padpt a, u8 *ra)
{
	return bf_host_bfer_by_addr(a, ra);
}

static inline struct bf_host_bfee *o_bfee_by_addr(bf_host_padpt a, u8 *ra)
{
	return bf_host_bfee_by_addr(a, ra);
}
#else
enum bf_host_cap o_cap_by_macid(void *mlme, u8 macid);
struct bf_host_bfer *o_bfer_by_addr(bf_host_padpt a, u8 *ra);
struct bf_host_bfee *o_bfee_by_addr(bf_host_padpt a, u8 *ra);
#endif

#endif
