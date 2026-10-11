/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 oracle API for WAPI QoS PN cache helpers (W3-141).
 * Provenance: core/rtw_wapi_sms4_rest.c
 */
#ifndef HOST_WAPI_SMS4_QOS_PN_ORACLE_H
#define HOST_WAPI_SMS4_QOS_PN_ORACLE_H

#include <stddef.h>

typedef unsigned char u8;

struct host_list_head {
	void *next;
	void *prev;
};

typedef struct host_rt_wapi_key {
	u8 dataKey[16];
	u8 micKey[16];
	u8 keyId;
	u8 bSet;
	u8 bTxEnable;
} host_rt_wapi_key;

typedef struct host_rt_wapi_sta_info {
	struct host_list_head list;
	u8 PeerMacAddr[6];
	host_rt_wapi_key wapiUsk;
	host_rt_wapi_key wapiUskUpdate;
	host_rt_wapi_key wapiMsk;
	host_rt_wapi_key wapiMskUpdate;
	u8 lastRxUnicastPN[16];
	u8 lastTxUnicastPN[16];
	u8 lastRxMulticastPN[16];
	u8 lastRxUnicastPNBEQueue[16];
	u8 lastRxUnicastPNBKQueue[16];
	u8 lastRxUnicastPNVIQueue[16];
	u8 lastRxUnicastPNVOQueue[16];
	u8 bSetkeyOk;
	u8 bAuthenticateInProgress;
	u8 bAuthenticatorInUpdata;
} host_rt_wapi_sta_info;

typedef host_rt_wapi_sta_info *host_prt_wapi_sta_info;

void host_wapi_get_last_rx_unicast_pn_for_qos_data(u8 user_priority,
						  host_prt_wapi_sta_info sta,
						  u8 *pn_out);

void host_wapi_set_last_rx_unicast_pn_for_qos_data(u8 user_priority, u8 *pn_in,
						   host_prt_wapi_sta_info sta);

u8 host_wapi_check_pn_in_sw_decrypt(void *padapter, void *pskb);

#endif /* HOST_WAPI_SMS4_QOS_PN_ORACLE_H */
