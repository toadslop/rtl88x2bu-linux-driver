/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 tests of update_attrib main helpers (W3-86 PR21).
 */
#ifndef HOST_XMIT_UPDATE_ATTRIB_MAIN_TYPES_H
#define HOST_XMIT_UPDATE_ATTRIB_MAIN_TYPES_H

#include "host_types.h"

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

#define _TRUE 1
#define _FALSE 0

#define ETH_ALEN 6
#define ETH_HLEN 14
#define ETH_P_ARP 0x0806

#define WLAN_HDR_A3_QOS_LEN 26
#define WLAN_HDR_A4_QOS_LEN 32
#define WIFI_QOS_DATA_TYPE (BIT(7) | BIT(3))

#define LPS_PT_NORMAL 0
#define LPS_PT_SP 1
#define LPS_PT_ICMP 2

struct pkt_attrib {
	u8 wds;
	u16 ether_type;
	u8 icmp_pkt;
	u8 dhcp_pkt;
	u8 priority;
	u8 hdrlen;
	u8 subtype;
};

#define XATTRIB_GET_WDS(xattrib) ((xattrib)->wds)

struct ethhdr {
	u8 h_dest[ETH_ALEN];
	u8 h_source[ETH_ALEN];
	u16 h_proto;
} __attribute__((packed));

struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD) || !defined(__BIG_ENDIAN_BITFIELD)
	u8 ihl:4;
	u8 version:4;
#else
	u8 version:4;
	u8 ihl:4;
#endif
	u8 tos;
	u8 rest[19];
} __attribute__((packed));

struct _pkt {
	const u8 *data;
	u32 len;
};

typedef struct _pkt _pkt;

struct pkt_file {
	const u8 *cur;
	u32 remain;
	u32 pkt_len;
};

u8 tos_to_up(u8 tos);
void _rtw_open_pktfile(_pkt *pkt, struct pkt_file *pf);
s32 _rtw_pktfile_read(struct pkt_file *pf, u8 *buf, u32 len);

void rtw_xmit_update_attrib_set_qos(_pkt *pkt, struct pkt_attrib *pattrib);
u8 rtw_xmit_update_attrib_lps_chk_packet_type(struct pkt_attrib *pattrib);

#endif /* HOST_XMIT_UPDATE_ATTRIB_MAIN_TYPES_H */
