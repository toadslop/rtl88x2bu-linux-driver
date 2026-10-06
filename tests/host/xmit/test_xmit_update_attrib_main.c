// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_xmit_update_attrib_main_types.h"

static int expect_qos(const char *name, struct pkt_attrib *a, u8 pri, u8 hdrlen,
		      u8 subtype)
{
	if (a->priority != pri || a->hdrlen != hdrlen || a->subtype != subtype) {
		fprintf(stderr, "%s: pri=%u hdrlen=%u subtype=0x%x\n", name,
			a->priority, a->hdrlen, a->subtype);
		return -1;
	}
	printf("PASS %s\n", name);
	return 0;
}

static int test_set_qos_null_pkt(void)
{
	struct pkt_attrib a;

	memset(&a, 0, sizeof(a));
	a.wds = 0;
	rtw_xmit_update_attrib_set_qos(NULL, &a);
	return expect_qos("set_qos_null_pkt", &a, 0, WLAN_HDR_A3_QOS_LEN,
			  WIFI_QOS_DATA_TYPE);
}

static int test_set_qos_ipv4_tos(void)
{
	struct pkt_attrib a;
	struct _pkt pkt;
	u8 frame[ETH_HLEN + sizeof(struct iphdr)];

	memset(&a, 0, sizeof(a));
	a.ether_type = 0x0800;
	a.wds = 0;

	memset(frame, 0, sizeof(frame));
	frame[12] = 0x08;
	frame[13] = 0x00;
	frame[15] = 0xa0; /* IP tos → up = 5 (160 >> 5) */

	pkt.data = frame;
	pkt.len = sizeof(frame);

	rtw_xmit_update_attrib_set_qos(&pkt, &a);
	return expect_qos("set_qos_ipv4_tos", &a, 5, WLAN_HDR_A3_QOS_LEN,
			  WIFI_QOS_DATA_TYPE);
}

static int test_set_qos_wds_hdrlen(void)
{
	struct pkt_attrib a;

	memset(&a, 0, sizeof(a));
	a.wds = 1;
	rtw_xmit_update_attrib_set_qos(NULL, &a);
	return expect_qos("set_qos_wds_hdrlen", &a, 0, WLAN_HDR_A4_QOS_LEN,
			  WIFI_QOS_DATA_TYPE);
}

static int expect_lps(const char *name, struct pkt_attrib *a, u8 expect)
{
	u8 got = rtw_xmit_update_attrib_lps_chk_packet_type(a);

	if (got != expect) {
		fprintf(stderr, "%s: got %u expect %u\n", name, got, expect);
		return -1;
	}
	printf("PASS %s\n", name);
	return 0;
}

static int test_lps_icmp(void)
{
	struct pkt_attrib a;

	memset(&a, 0, sizeof(a));
	a.icmp_pkt = 1;
	return expect_lps("lps_icmp", &a, LPS_PT_ICMP);
}

static int test_lps_dhcp(void)
{
	struct pkt_attrib a;

	memset(&a, 0, sizeof(a));
	a.dhcp_pkt = 1;
	return expect_lps("lps_dhcp", &a, LPS_PT_SP);
}

static int test_lps_normal(void)
{
	struct pkt_attrib a;

	memset(&a, 0, sizeof(a));
	return expect_lps("lps_normal", &a, LPS_PT_NORMAL);
}

int main(void)
{
	int fail = 0;

	fail |= test_set_qos_null_pkt();
	fail |= test_set_qos_ipv4_tos();
	fail |= test_set_qos_wds_hdrlen();
	fail |= test_lps_icmp();
	fail |= test_lps_dhcp();
	fail |= test_lps_normal();

	return fail ? 1 : 0;
}
