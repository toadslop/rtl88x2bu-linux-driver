// SPDX-License-Identifier: GPL-2.0
/* W3-197 L2: legacy update_attrib_txbf_info (bf_info → pkt_attrib). */
#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_update_txbf_attrib_oracle.h"

#ifndef HOST_BF_UPDATE_TXBF_ATTRIB_RUST
void update_attrib_txbf_info(bf_host_cmd_traffic_padpt padapter, struct bf_host_pkt_attrib *pattrib,
			     struct bf_host_sta *psta)
{
	o_update_attrib_txbf_info(padapter, pattrib, psta);
}
#endif

extern void update_attrib_txbf_info(bf_host_cmd_traffic_padpt padapter,
				    struct bf_host_pkt_attrib *pattrib,
				    struct bf_host_sta *psta);

int main(void)
{
	struct bf_host_cmd_traffic_adpt adpt;
	struct bf_host_pkt_attrib attrib;
	struct bf_host_sta *sta;
	int bad = 0;

	memset(&adpt, 0, sizeof(adpt));
	memset(&attrib, 0, sizeof(attrib));
	sta = &adpt.stapriv.stas[0];
	sta->used = 1;
	sta->cmn.g_id = 42;
	sta->cmn.p_aid = 0x1234;

	update_attrib_txbf_info(&adpt, &attrib, sta);
	if (attrib.txbf_g_id != 42 || attrib.txbf_p_aid != 0x1234) {
		fprintf(stderr, "FAIL sta bf_info copied\n");
		bad++;
	} else {
		printf("PASS sta bf_info copied\n");
	}

	memset(&attrib, 0x55, sizeof(attrib));
	update_attrib_txbf_info(&adpt, &attrib, NULL);
	if (attrib.txbf_g_id != 0x5555 || attrib.txbf_p_aid != 0x5555) {
		fprintf(stderr, "FAIL null sta no-op\n");
		bad++;
	} else {
		printf("PASS null sta no-op\n");
	}

	if (!bad)
		printf("PASS 2 vectors (builtin)\n");
	return bad ? 1 : 0;
}
