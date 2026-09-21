/* SPDX-License-Identifier: GPL-2.0 */
#include "host_br_ext_types.h"

static unsigned char *scan_tlv(unsigned char *data, int len, unsigned char tag,
			       unsigned char len8b)
{
	while (len > 0) {
		if (*data == tag && *(data + 1) == len8b && len >= len8b * 8)
			return data + 2;
		len -= (*(data + 1)) * 8;
		data += (*(data + 1)) * 8;
	}
	return NULL;
}

unsigned char *host_scan_tlv(u8 *data, int len, u8 tag, u8 len8b)
{
	return scan_tlv(data, len, tag, len8b);
}

int host_update_nd_link_layer_addr(u8 *data, int len, u8 *replace_mac)
{
	struct icmp6hdr *icmphdr = (struct icmp6hdr *)data;
	unsigned char *mac;

	if (icmphdr->icmp6_type == NDISC_ROUTER_SOLICITATION && len >= 8) {
		mac = scan_tlv(&data[8], len - 8, 1, 1);
		if (mac) {
			memcpy(mac, replace_mac, 6);
			return 1;
		}
	}
	return 0;
}

void host_convert_ipv6_mac_to_mc(struct host_sk_buff *skb)
{
	struct ipv6hdr *iph = (struct ipv6hdr *)(skb->data + ETH_HLEN);

	skb->data[0] = 0x33;
	skb->data[1] = 0x33;
	memcpy(&skb->data[2], &iph->daddr.s6_addr32[3], 4);
}

int host_skb_pull_and_merge(struct host_sk_buff *skb, u8 *src, int len)
{
	unsigned long tail = (unsigned long)skb_tail_pointer(skb);
	unsigned long end = (unsigned long)src + len;
	int tail_len;

	if ((src + len) > skb_tail_pointer(skb) || skb->len < len)
		return -1;
	if (tail < end)
		return -1;
	tail_len = (int)(tail - end);
	if (tail_len > 0)
		memmove(src, src + len, tail_len);
	skb->len -= len;
	return 0;
}
