// SPDX-License-Identifier: GPL-2.0
/* W3-129 L2 C oracle: VHT GID mgmt send/get leaf. */
#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_gid_oracle.h"

struct bf_host_gid_xmit_tr bf_host_gid_xmit_tr;
struct bf_host_gid_set_tr bf_host_gid_set_tr;

void bf_host_cmd(bf_host_padpt a, int type, u8 *p, int sz, u8 enq)
{
	(void)a;
	(void)type;
	(void)p;
	(void)sz;
	(void)enq;
}

void bf_host_bfer_set_gid(bf_host_padpt a, u8 *ta, u8 *gid, u8 *pos)
{
	(void)a;
	bf_host_gid_set_tr.count++;
	memcpy(bf_host_gid_set_tr.ta, ta, BF_HOST_ETH_ALEN);
	memcpy(bf_host_gid_set_tr.gid, gid, 8);
	memcpy(bf_host_gid_set_tr.position, pos, 16);
}

static void parse_mac(const char *s, u8 *m)
{
	sscanf(s, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &m[0], &m[1], &m[2], &m[3],
	       &m[4], &m[5]);
}

int main(void)
{
	struct bf_host_adpt a;
	struct bf_host_recv_frame rf;
	u8 ra[6], gid[8], pos[16];
	int bad = 0;

	memset(&a, 0, sizeof(a));
	parse_mac("aa:bb:cc:dd:ee:ff", ra);
	parse_mac("11:22:33:44:55:66", a.mlmepriv.mac_addr);
	parse_mac("77:88:99:aa:bb:cc", a.mlmepriv.bssid);
	memset(gid, 0x11, sizeof(gid));
	memset(pos, 0x22, sizeof(pos));
	if (!o_bf_send_vht_gid_mgnt(&a, ra, gid, pos) ||
	    bf_host_gid_xmit_tr.pktlen != 54 ||
	    bf_host_gid_xmit_tr.frame[24] != BF_HOST_CAT_VHT_GID) {
		fprintf(stderr, "FAIL send_vht_gid\n");
		bad++;
	} else {
		printf("PASS send_vht_gid\n");
	}

	memset(&bf_host_gid_set_tr, 0, sizeof(bf_host_gid_set_tr));
	memset(&rf, 0, sizeof(rf));
	parse_mac("fe:11:22:33:44:55", rf.hdr.data + 10);
	memcpy(rf.hdr.data + 26, gid, 8);
	memcpy(rf.hdr.data + 34, pos, 16);
	o_bf_get_vht_gid_mgnt(&a, &rf);
	if (bf_host_gid_set_tr.count != 1 || bf_host_gid_set_tr.ta[0] != 0xFE) {
		fprintf(stderr, "FAIL get_vht_gid\n");
		bad++;
	} else {
		printf("PASS get_vht_gid\n");
	}

	if (!bad)
		printf("PASS 2 vectors (builtin)\n");
	return bad ? 1 : 0;
}
