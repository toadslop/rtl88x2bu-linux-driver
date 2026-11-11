/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-236 proc p2p_wowlan_info dump. */
#ifndef HOST_RTW_DEBUG_PROC_WOW_P2P_TYPES_H
#define HOST_RTW_DEBUG_PROC_WOW_P2P_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;

#define ETH_ALEN 6
#define _TRUE 1

enum P2P_WOWLAN_RECV_FRAME_TYPE {
	P2P_WOWLAN_RECV_NEGO_REQ = 0,
	P2P_WOWLAN_RECV_INVITE_REQ = 1,
	P2P_WOWLAN_RECV_PROVISION_REQ = 2,
};

struct p2p_wowlan_info {
	u8 is_trigger;
	enum P2P_WOWLAN_RECV_FRAME_TYPE wowlan_recv_frame_type;
	u8 wowlan_peer_addr[ETH_ALEN];
	u16 wowlan_peer_wpsconfig;
	u8 wowlan_peer_is_persistent;
	u8 wowlan_peer_invitation_type;
};

struct wifidirect_info {
	struct p2p_wowlan_info p2p_wow_info;
};

struct _adapter {
	struct wifidirect_info wdinfo;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(addr)                                                        \
	(int)(addr)[0], (int)(addr)[1], (int)(addr)[2], (int)(addr)[3],         \
		(int)(addr)[4], (int)(addr)[5]

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

void *rtw_rust_debug_proc_adapter(void *m);

void rtw_rust_debug_proc_emit_p2p_wowlan_info(void *sel, void *adapter);

int proc_get_p2p_wowlan_info(struct seq_file *m, void *v);

#endif /* HOST_RTW_DEBUG_PROC_WOW_P2P_TYPES_H */
