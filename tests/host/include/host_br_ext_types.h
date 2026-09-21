/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BR_EXT_TYPES_H
#define HOST_BR_EXT_TYPES_H

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define NAT25_IPV4 1
#define NAT25_IPV6 2
#define NAT25_PPPOE 5
#define NAT25_HASH_SIZE 16
#define MAX_NETWORK_ADDR_LEN 17
#define NAT25_AGEING_TIME 300
#define ETH_HLEN 14
#define HZ 100

#define NDISC_ROUTER_SOLICITATION 133

struct host_sk_buff {
	u8 *data;
	int len;
};

struct icmp6hdr {
	u8 icmp6_type;
	u8 icmp6_code;
	u16 icmp6_cksum;
};

struct ipv6hdr {
	u8 priority_version;
	u8 flow_lbl[3];
	u16 payload_len;
	u8 nexthdr;
	u8 hop_limit;
	struct {
		u32 s6_addr32[4];
	} saddr;
	struct {
		u32 s6_addr32[4];
	} daddr;
};

static inline u8 *skb_tail_pointer(struct host_sk_buff *skb)
{
	return skb->data + skb->len;
}

typedef unsigned long host_jiffies_t;
extern host_jiffies_t host_br_ext_jiffies_val;

typedef struct {
	int _pad;
} _adapter;

struct nat25_network_db_entry {
	unsigned long ageing_timer;
};

static inline int time_before_eq(unsigned long a, unsigned long b)
{
	return (long)(a) - (long)(b) <= 0;
}

#endif
