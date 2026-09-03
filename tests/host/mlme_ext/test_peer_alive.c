// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_mlme_ext_peer_alive_types.h"

static _adapter adapter;
static struct sta_info sta;

static int run_ap(const char *name, u64 rx, u64 last, u64 bcn, u64 bcn_last,
		  u8 expect)
{
	u8 ret;
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	sta.sta_stats.rx_data_pkts = rx;
	sta.sta_stats.last_rx_data_pkts = last;
	sta.sta_stats.rx_beacon_pkts = bcn;
	sta.sta_stats.last_rx_beacon_pkts = bcn_last;
	ret = chk_ap_is_alive(&adapter, &sta);
	if (ret != expect) {
		fprintf(stderr, "%s: ret %u expect %u\n", name, ret, expect);
		return -1;
	}
	printf("PASS: %s\n", name);
	return 0;
}

static int run_delba(const char *name, u8 vendor, u8 tid, u8 en, int count,
		     u64 qos, u64 qos_last, u8 timer, u8 xdelba, u8 xdelba_ex,
		     u8 en_after)
{
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	memset(&host_last_delba, 0, sizeof(host_last_delba));
	memset(&host_last_delba_ex, 0, sizeof(host_last_delba_ex));
	adapter.mlmeextpriv.mlmext_info.assoc_AP_vendor = vendor;
	sta.recvreorder_ctrl[tid].enable = en;
	sta.continual_no_rx_packet[tid] = count;
	sta.sta_stats.rx_data_qos_pkts[tid] = qos;
	sta.sta_stats.last_rx_data_qos_pkts[tid] = qos_last;
	rtw_delba_check(&adapter, &sta, timer);
	if (host_last_delba.called != xdelba || host_last_delba_ex.called != xdelba_ex ||
	    sta.recvreorder_ctrl[tid].enable != en_after) {
		fprintf(stderr, "%s: delba mismatch\n", name);
		return -1;
	}
	printf("PASS: %s\n", name);
	return 0;
}

int main(void)
{
	if (run_ap("ap_data_changed", 10, 5, 3, 3, _TRUE) ||
	    run_ap("ap_stale", 5, 5, 3, 3, _FALSE) ||
	    run_ap("ap_beacon_changed", 5, 5, 4, 3, _TRUE))
		return 1;
	memset(&sta, 0, sizeof(sta));
	sta.sta_stats.rx_data_pkts = 2;
	sta.sta_stats.last_rx_data_pkts = 2;
	if (chk_adhoc_peer_is_alive(&sta) != _FALSE) {
		fprintf(stderr, "adhoc_stale failed\n");
		return 1;
	}
	printf("PASS: adhoc_stale\n");
#ifdef CONFIG_TDLS
	memset(&sta, 0, sizeof(sta));
	sta.sta_stats.rx_data_pkts = 5;
	sta.sta_stats.last_rx_data_pkts = 4;
	if (chk_tdls_peer_sta_is_alive(&adapter, &sta) != _TRUE) {
		fprintf(stderr, "tdls_data_changed failed\n");
		return 1;
	}
	printf("PASS: tdls_data_changed\n");
#endif
	if (run_delba("delba_skip_vendor", 0, 0, 1, 4, 1, 1, 0, 0, 0, 1) ||
	    run_delba("delba_bcm_ex", HT_IOT_PEER_BROADCOM, 2, 1, 3, 7, 7, 0, 0, 1, 0) ||
	    run_delba("delba_bcm_timer", HT_IOT_PEER_BROADCOM, 1, 1, 3, 2, 2, 1, 1, 0, 0) ||
	    run_delba("delba_qos_reset", HT_IOT_PEER_BROADCOM, 0, 1, 3, 5, 4, 0, 0, 0, 1))
		return 1;
	printf("All peer-alive vectors passed.\n");
	return 0;
}
