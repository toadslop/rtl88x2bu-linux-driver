/* SPDX-License-Identifier: GPL-2.0 */
#include "host_br_ext_types.h"

static void gen_ipv4(unsigned char *na, unsigned int *ip)
{
	memset(na, 0, MAX_NETWORK_ADDR_LEN);
	na[0] = NAT25_IPV4;
	memcpy(na + 7, ip, 4);
}

static void gen_pppoe(unsigned char *na, unsigned char *mac, unsigned short *sid)
{
	memset(na, 0, MAX_NETWORK_ADDR_LEN);
	na[0] = NAT25_PPPOE;
	memcpy(na + 1, sid, 2);
	memcpy(na + 3, mac, 6);
}

static int network_hash(unsigned char *na)
{
	unsigned long x = 0;

	if (na[0] == NAT25_IPV4)
		x = na[7] ^ na[8] ^ na[9] ^ na[10];
	else if (na[0] == NAT25_PPPOE)
		x = na[0] ^ na[1] ^ na[2] ^ na[3] ^ na[4] ^ na[5] ^ na[6] ^
		    na[7] ^ na[8];
	else if (na[0] == NAT25_IPV6)
		x = na[1] ^ na[2] ^ na[3] ^ na[4] ^ na[5] ^ na[6] ^ na[7] ^
		    na[8] ^ na[9] ^ na[10] ^ na[11] ^ na[12] ^ na[13] ^ na[14] ^
		    na[15] ^ na[16];
	else {
		int i;

		for (i = 0; i < MAX_NETWORK_ADDR_LEN; i++)
			x ^= na[i];
	}
	return (int)(x & (NAT25_HASH_SIZE - 1));
}

void host_nat25_gen_ipv4(u8 *na, u32 ip) { gen_ipv4(na, &ip); }
void host_nat25_gen_pppoe(u8 *na, u8 *mac, u16 sid) { gen_pppoe(na, mac, &sid); }
int host_nat25_network_hash(u8 *na) { return network_hash(na); }
