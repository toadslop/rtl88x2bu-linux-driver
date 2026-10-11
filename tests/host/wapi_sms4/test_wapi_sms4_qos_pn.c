// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for WAPI QoS PN cache (W3-141 / T17).
 */
#include <stdio.h>
#include <string.h>

#include "host_wapi_sms4_qos_pn_oracle.h"

struct up_case {
	const char *name;
	u8 user_priority;
};

static const struct up_case k_ups[] = {
	{ "be-up0", 0 },
	{ "be-up3", 3 },
	{ "bk-up1", 1 },
	{ "bk-up2", 2 },
	{ "vi-up4", 4 },
	{ "vi-up5", 5 },
	{ "vo-up6", 6 },
	{ "vo-up7", 7 },
};

static int run_roundtrip(const struct up_case *c)
{
	host_rt_wapi_sta_info sta;
	u8 pn_in[16];
	u8 pn_out[16];
	int i;

	memset(&sta, 0, sizeof(sta));
	for (i = 0; i < 16; i++)
		pn_in[i] = (u8)(0xa0 + c->user_priority + i);

	host_wapi_set_last_rx_unicast_pn_for_qos_data(c->user_priority, pn_in, &sta);
	memset(pn_out, 0, sizeof(pn_out));
	host_wapi_get_last_rx_unicast_pn_for_qos_data(c->user_priority, &sta, pn_out);

	if (memcmp(pn_in, pn_out, 16) != 0) {
		fprintf(stderr, "%s: get/set roundtrip mismatch\n", c->name);
		return -1;
	}
	return 0;
}

static int run_queue_isolation(void)
{
	host_rt_wapi_sta_info sta;
	u8 be[16], bk[16], vi[16], vo[16];
	u8 out[16];
	int i;

	memset(&sta, 0, sizeof(sta));
	for (i = 0; i < 16; i++) {
		be[i] = (u8)(0x10 + i);
		bk[i] = (u8)(0x20 + i);
		vi[i] = (u8)(0x30 + i);
		vo[i] = (u8)(0x40 + i);
	}

	host_wapi_set_last_rx_unicast_pn_for_qos_data(0, be, &sta);
	host_wapi_set_last_rx_unicast_pn_for_qos_data(1, bk, &sta);
	host_wapi_set_last_rx_unicast_pn_for_qos_data(4, vi, &sta);
	host_wapi_set_last_rx_unicast_pn_for_qos_data(6, vo, &sta);

	host_wapi_get_last_rx_unicast_pn_for_qos_data(3, &sta, out);
	if (memcmp(out, be, 16) != 0)
		return -1;
	host_wapi_get_last_rx_unicast_pn_for_qos_data(2, &sta, out);
	if (memcmp(out, bk, 16) != 0)
		return -1;
	host_wapi_get_last_rx_unicast_pn_for_qos_data(5, &sta, out);
	if (memcmp(out, vi, 16) != 0)
		return -1;
	host_wapi_get_last_rx_unicast_pn_for_qos_data(7, &sta, out);
	if (memcmp(out, vo, 16) != 0)
		return -1;

	return 0;
}

int main(void)
{
	size_t i;

	for (i = 0; i < sizeof(k_ups) / sizeof(k_ups[0]); i++) {
		if (run_roundtrip(&k_ups[i]) != 0)
			return 1;
	}

	if (run_queue_isolation() != 0) {
		fprintf(stderr, "queue isolation failed\n");
		return 1;
	}

	if (host_wapi_check_pn_in_sw_decrypt((void *)1, (void *)2) != 0) {
		fprintf(stderr, "WapiCheckPnInSwDecrypt must always return false\n");
		return 1;
	}

#ifdef RUST_WAPI_QOS_PN_ORACLE
	printf("all wapi_sms4_qos_pn vectors passed (oracle: rust/rtw_wapi_sms4.rs)\n");
#else
	printf("all wapi_sms4_qos_pn vectors passed (oracle: core/rtw_wapi_sms4_rest.c)\n");
#endif
	return 0;
}
