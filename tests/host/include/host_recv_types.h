/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host L2 recv_rest tests (W3-39).
 */
#ifndef HOST_RECV_TYPES_H
#define HOST_RECV_TYPES_H

#include <stdbool.h>
#include <string.h>

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define TID_NUM 16
#define MAX_CONTINUAL_NORXPACKET_COUNT 4
#define ETH_ALEN 6
#define WLAN_EID_VENDOR_SPECIFIC 221
#define MAX_IE_SZ 768
#define HOST_RECV_MAX_FRAME 512

typedef int ATOMIC_T;
#define ATOMIC_INC_RETURN(v) __sync_add_and_fetch((v), 1)
#define ATOMIC_SET(v, x) (*(v) = (x))

struct sta_info {
	ATOMIC_T continual_no_rx_packet[TID_NUM];
};

struct rtw_ieee80211_hdr_3addr {
	u16 frame_ctl;
	u16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	u16 seq_ctl;
} __attribute__((packed));

struct recv_frame_hdr {
	u32 _pad;
	unsigned int len;
	u8 *rx_data;
};

union recv_frame {
	struct {
		struct recv_frame_hdr hdr;
	} u;
};

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index);
bool rtw_rframe_del_wfd_ie(union recv_frame *rframe, u8 ies_offset);
u8 *rtw_get_wfd_ie(const u8 *in_ie, int in_len, u8 *wfd_ie, unsigned int *wfd_ielen);
unsigned int rtw_del_wfd_ie(u8 *ies, unsigned int ies_len_ori, const char *msg);

#endif /* HOST_RECV_TYPES_H */
