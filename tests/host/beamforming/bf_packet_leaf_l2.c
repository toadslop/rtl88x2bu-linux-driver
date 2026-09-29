// SPDX-License-Identifier: GPL-2.0
/* W3-128 L2 C oracle: NDPA/report packet leaf (core/rtw_beamforming.c). */
#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_packet_oracle.h"

struct bf_host_cmd_tr bf_host_cmd_tr;

void bf_host_cmd(bf_host_padpt a, int type, u8 *p, int sz, u8 enq)
{
	(void)a;
	(void)p;
	(void)sz;
	(void)enq;
	bf_host_cmd_tr.count++;
	bf_host_cmd_tr.last_type = (u8)type;
}

static void parse_mac(const char *s, u8 *m)
{
	sscanf(s, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]);
}

static void setup_bfee(struct bf_host_adpt *a, const char *mac)
{
	parse_mac(mac, BF_GET_INFO(a)->bfee[0].mac_addr);
	BF_GET_INFO(a)->bfee[0].used = 1;
}

int main(void)
{
	struct bf_host_adpt a;
	struct bf_host_recv_frame rf;
	u32 ret;
	int bad = 0;

	memset(&bf_host_cmd_tr, 0, sizeof(bf_host_cmd_tr));
	memset(&a, 0, sizeof(a));
	o_ndpa(&a, &rf);
	printf("PASS ndpa_noop\n");

	memset(&rf, 0, sizeof(rf));
	rf.hdr.len = 64;
	parse_mac("00:11:22:33:44:55", bf_host_addr2(rf.hdr.data));
	ret = o_report(&a, &rf);
	if (ret != BF_HOST_FAIL) {
		fprintf(stderr, "FAIL report_unknown_ta\n");
		bad++;
	} else {
		printf("PASS report_unknown_ta\n");
	}

	memset(&bf_host_cmd_tr, 0, sizeof(bf_host_cmd_tr));
	memset(&a, 0, sizeof(a));
	memset(&rf, 0, sizeof(rf));
	setup_bfee(&a, "00:11:22:33:44:55");
	BF_GET_INFO(&a)->bEnableSUTxBFWorkAround = 1;
	BF_GET_INFO(&a)->TargetSUBFee = &BF_GET_INFO(&a)->bfee[0];
	rf.hdr.len = 80;
	parse_mac("00:11:22:33:44:55", bf_host_addr2(rf.hdr.data));
	rf.hdr.data[24] = BF_HOST_CAT_VHT;
	rf.hdr.data[25] = BF_HOST_ACT_VHT_BF;
	rf.hdr.data[26] = 7;
	rf.hdr.data[27] = 4;
	ret = o_report(&a, &rf);
	if (ret != BF_HOST_SUCCESS || bf_host_cmd_tr.count != 1 ||
	    BF_GET_INFO(&a)->TargetCSIInfo.Nc != 7 ||
	    BF_GET_INFO(&a)->TargetCSIInfo.bVHT != 1) {
		fprintf(stderr, "FAIL report_vht_csi_update\n");
		bad++;
	} else {
		printf("PASS report_vht_csi_update\n");
	}
	if (!bad)
		printf("PASS 3 vectors (builtin)\n");
	return bad ? 1 : 0;
}
