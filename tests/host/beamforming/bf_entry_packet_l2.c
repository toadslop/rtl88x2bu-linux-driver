// SPDX-License-Identifier: GPL-2.0
/* W3-128 L2 C oracle: beamforming entry lookup leaf (core/rtw_beamforming.c). */
#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_oracle.h"

#ifdef HOST_BF_ENTRY_PACKET_RUST
void bf_host_cmd(bf_host_padpt a, int type, u8 *p, int sz, u8 enq)
{
	(void)a;
	(void)type;
	(void)p;
	(void)sz;
	(void)enq;
}
#endif

static void parse_mac(const char *s, u8 *m)
{
	sscanf(s, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]);
}

static void setup_bfer(struct bf_host_adpt *a, const char *mac)
{
	parse_mac(mac, BF_GET_INFO(a)->bfer[0].mac_addr);
	BF_GET_INFO(a)->bfer[0].used = 1;
}

static void setup_bfee(struct bf_host_adpt *a, u8 macid, enum bf_host_cap cap, const char *mac)
{
	parse_mac(mac, BF_GET_INFO(a)->bfee[0].mac_addr);
	BF_GET_INFO(a)->bfee[0].used = 1;
	BF_GET_INFO(a)->bfee[0].mac_id = macid;
	BF_GET_INFO(a)->bfee[0].cap = cap;
}

static int check_bfer(struct bf_host_adpt *a, const char *mac, int expect_null, const char *name)
{
	u8 m[BF_HOST_ETH_ALEN];
	void *p;

	parse_mac(mac, m);
	p = o_bfer_by_addr(a, m);
	if ((p == NULL) != expect_null) {
		fprintf(stderr, "FAIL %s\n", name);
		return -1;
	}
	printf("PASS %s\n", name);
	return 0;
}

static int check_bfee(struct bf_host_adpt *a, const char *mac, int expect_null, const char *name)
{
	u8 m[BF_HOST_ETH_ALEN];
	void *p;

	parse_mac(mac, m);
	p = o_bfee_by_addr(a, m);
	if ((p == NULL) != expect_null) {
		fprintf(stderr, "FAIL %s\n", name);
		return -1;
	}
	printf("PASS %s\n", name);
	return 0;
}

int main(void)
{
	struct bf_host_adpt a;
	int bad = 0;

	memset(&a, 0, sizeof(a));
	if ((int)o_cap_by_macid(&a.mlmepriv, 3) != 0) {
		fprintf(stderr, "FAIL cap_none_empty\n");
		bad++;
	} else {
		printf("PASS cap_none_empty\n");
	}
	memset(&a, 0, sizeof(a));
	setup_bfee(&a, 5, BF_HOST_BFEE_VHT_SU, "00:00:00:00:00:01");
	if ((int)o_cap_by_macid(&a.mlmepriv, 5) != 8) {
		fprintf(stderr, "FAIL cap_vht_su_hit\n");
		bad++;
	} else {
		printf("PASS cap_vht_su_hit\n");
	}
	memset(&a, 0, sizeof(a));
	bad += check_bfer(&a, "00:00:00:00:00:00", 1, "bfer_miss");
	memset(&a, 0, sizeof(a));
	setup_bfer(&a, "aa:bb:cc:dd:ee:01");
	bad += check_bfer(&a, "aa:bb:cc:dd:ee:01", 0, "bfer_hit");
	memset(&a, 0, sizeof(a));
	bad += check_bfee(&a, "11:22:33:44:55:66", 1, "bfee_miss");
	memset(&a, 0, sizeof(a));
	setup_bfee(&a, 1, BF_HOST_BFEE_VHT_SU, "de:ad:be:ef:00:01");
	bad += check_bfee(&a, "de:ad:be:ef:00:01", 0, "bfee_hit");
	if (!bad)
		printf("PASS 6 vectors (builtin)\n");
	return bad ? 1 : 0;
}
