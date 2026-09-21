/* SPDX-License-Identifier: GPL-2.0 */
#include <stdlib.h>
#include <string.h>

#include "host_br_ext_types.h"

int host_nat25_network_hash(u8 *na);
int host_nat25_has_expired(_adapter *priv, struct nat25_network_db_entry *fdb);
static void network_hash_link(_adapter *priv, struct nat25_network_db_entry *ent,
			      int hash)
{
	ent->next_hash = priv->nethash[hash];
	if (ent->next_hash)
		ent->next_hash->pprev_hash = &ent->next_hash;
	priv->nethash[hash] = ent;
	ent->pprev_hash = &priv->nethash[hash];
}

void host_nat25_db_network_insert(_adapter *priv, u8 *mac_addr,
				    u8 *network_addr)
{
	struct nat25_network_db_entry *db;
	int hash;

	hash = host_nat25_network_hash(network_addr);
	db = priv->nethash[hash];
	while (db) {
		if (!memcmp(db->networkAddr, network_addr, MAX_NETWORK_ADDR_LEN)) {
			memcpy(db->macAddr, mac_addr, ETH_ALEN);
			db->ageing_timer = host_br_ext_jiffies_val;
			return;
		}
		db = db->next_hash;
	}

	db = calloc(1, sizeof(*db));
	if (!db)
		return;

	memcpy(db->networkAddr, network_addr, MAX_NETWORK_ADDR_LEN);
	memcpy(db->macAddr, mac_addr, ETH_ALEN);
	db->use_count = 1;
	db->ageing_timer = host_br_ext_jiffies_val;
	network_hash_link(priv, db, hash);
}

int host_nat25_db_network_lookup_and_replace(_adapter *priv,
					     struct host_sk_buff *skb,
					     u8 *network_addr)
{
	struct nat25_network_db_entry *db;

	db = priv->nethash[host_nat25_network_hash(network_addr)];
	while (db) {
		if (!memcmp(db->networkAddr, network_addr, MAX_NETWORK_ADDR_LEN)) {
			if (!host_nat25_has_expired(priv, db)) {
				memcpy(skb->data, db->macAddr, ETH_ALEN);
				db->use_count++;
			}
			return 1;
		}
		db = db->next_hash;
	}
	return 0;
}
