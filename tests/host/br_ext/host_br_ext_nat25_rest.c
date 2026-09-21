/* SPDX-License-Identifier: GPL-2.0 */
#include "host_br_ext_types.h"

host_jiffies_t host_br_ext_jiffies_val;

void host_nat25_gen_ipv6(u8 *na, const u8 *ip16)
{
	memset(na, 0, MAX_NETWORK_ADDR_LEN);
	na[0] = NAT25_IPV6;
	memcpy(na + 1, ip16, 16);
}

unsigned long host_nat25_timeout(_adapter *priv)
{
	(void)priv;
	return host_br_ext_jiffies_val - NAT25_AGEING_TIME * HZ;
}

int host_nat25_has_expired(_adapter *priv, struct nat25_network_db_entry *fdb)
{
	if (time_before_eq(fdb->ageing_timer, host_nat25_timeout(priv)))
		return 1;
	return 0;
}
