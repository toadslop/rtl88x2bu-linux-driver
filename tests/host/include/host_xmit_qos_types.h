/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L1/L2 rtw_xmit_qos_rest tests (W3-48).
 */
#ifndef HOST_XMIT_QOS_TYPES_H
#define HOST_XMIT_QOS_TYPES_H

#include "host_types.h"

#define _TKIP_ 0x02
#define CONFIG_RTW_UP_MAPPING_RULE 0
#define CONFIG_RTW_MESH 1

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

struct ieee80211_snap_hdr {
	u8 dsap;
	u8 ssap;
	u8 ctrl;
	u8 oui[3];
} __attribute__((packed));

#define SNAP_SIZE sizeof(struct ieee80211_snap_hdr)
#define P80211_OUI_LEN 3

struct pkt_attrib {
	u8 hdrlen;
	u8 iv_len;
	u8 meshctrl_len;
	u32 pktlen;
	u8 encrypt;
	u8 bswenc;
	u8 icv_len;
};

#define XATTRIB_GET_MCTRL_LEN(xattrib) ((xattrib)->meshctrl_len)

static inline u16 host_htons(u16 v)
{
	return ((v & 0xff) << 8) | ((v >> 8) & 0xff);
}

#define htons host_htons

#endif /* HOST_XMIT_QOS_TYPES_H */
