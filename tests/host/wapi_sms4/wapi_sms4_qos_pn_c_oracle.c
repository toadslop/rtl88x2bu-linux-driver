// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle for WAPI QoS PN cache helpers (W3-141).
 * Copied from core/rtw_wapi_sms4_rest.c — keep in sync until Rust swap.
 */
#include <string.h>

#include "host_wapi_sms4_qos_pn_oracle.h"

static void qos_pn_get(u8 user_priority, host_prt_wapi_sta_info sta, u8 *pn_out)
{
	switch (user_priority) {
	case 0:
	case 3:
		memcpy(pn_out, sta->lastRxUnicastPNBEQueue, 16);
		break;
	case 1:
	case 2:
		memcpy(pn_out, sta->lastRxUnicastPNBKQueue, 16);
		break;
	case 4:
	case 5:
		memcpy(pn_out, sta->lastRxUnicastPNVIQueue, 16);
		break;
	case 6:
	case 7:
		memcpy(pn_out, sta->lastRxUnicastPNVOQueue, 16);
		break;
	default:
		break;
	}
}

static void qos_pn_set(u8 user_priority, u8 *pn_in, host_prt_wapi_sta_info sta)
{
	switch (user_priority) {
	case 0:
	case 3:
		memcpy(sta->lastRxUnicastPNBEQueue, pn_in, 16);
		break;
	case 1:
	case 2:
		memcpy(sta->lastRxUnicastPNBKQueue, pn_in, 16);
		break;
	case 4:
	case 5:
		memcpy(sta->lastRxUnicastPNVIQueue, pn_in, 16);
		break;
	case 6:
	case 7:
		memcpy(sta->lastRxUnicastPNVOQueue, pn_in, 16);
		break;
	default:
		break;
	}
}

static u8 qos_pn_check_sw_decrypt(void *padapter, void *pskb)
{
	(void)padapter;
	(void)pskb;
	return 0;
}

#ifndef WAPI_SMS4_QOS_PN_L1_REF
void host_wapi_get_last_rx_unicast_pn_for_qos_data(u8 user_priority,
						   host_prt_wapi_sta_info sta,
						   u8 *pn_out)
{
	qos_pn_get(user_priority, sta, pn_out);
}

void host_wapi_set_last_rx_unicast_pn_for_qos_data(u8 user_priority, u8 *pn_in,
						   host_prt_wapi_sta_info sta)
{
	qos_pn_set(user_priority, pn_in, sta);
}

u8 host_wapi_check_pn_in_sw_decrypt(void *padapter, void *pskb)
{
	return qos_pn_check_sw_decrypt(padapter, pskb);
}
#else
void WapiGetLastRxUnicastPNForQoSData(u8 user_priority,
				      host_prt_wapi_sta_info sta, u8 *pn_out)
{
	qos_pn_get(user_priority, sta, pn_out);
}

void WapiSetLastRxUnicastPNForQoSData(u8 user_priority, u8 *pn_in,
				      host_prt_wapi_sta_info sta)
{
	qos_pn_set(user_priority, pn_in, sta);
}

u8 WapiCheckPnInSwDecrypt(void *padapter, void *pskb)
{
	return qos_pn_check_sw_decrypt(padapter, pskb);
}
#endif
