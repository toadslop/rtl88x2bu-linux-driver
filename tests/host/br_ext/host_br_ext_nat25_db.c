/* SPDX-License-Identifier: GPL-2.0 */
#include <stdlib.h>
#include <string.h>

#include "host_br_ext_db_types.h"

int host_nat25_network_hash(u8 *na);
int host_nat25_has_expired(_adapter *priv, struct nat25_network_db_entry *fdb);

static int db_has_expired(host_nat25_db_adapter *priv,
			  struct host_nat25_db_entry *fdb)
{
	struct nat25_network_db_entry shim;

	(void)priv;
	shim.ageing_timer = fdb->ageing_timer;
	return host_nat25_has_expired((_adapter *)priv, &shim);
}

static void network_hash_link(host_nat25_db_adapter *priv,
			      struct host_nat25_db_entry *ent, int hash)
{
	ent->next_hash = priv->nethash[hash];
	if (ent->next_hash)
		ent->next_hash->pprev_hash = &ent->next_hash;
	priv->nethash[hash] = ent;
	ent->pprev_hash = &priv->nethash[hash];
}

void host_nat25_db_network_insert(host_nat25_db_adapter *priv, u8 *mac_addr,
				    u8 *network_addr)
{
	struct host_nat25_db_entry *db;
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

int host_nat25_db_network_lookup_and_replace(host_nat25_db_adapter *priv,
					     struct host_sk_buff *skb,
					     u8 *network_addr)
{
	struct host_nat25_db_entry *db;

	db = priv->nethash[host_nat25_network_hash(network_addr)];
	while (db) {
		if (!memcmp(db->networkAddr, network_addr, MAX_NETWORK_ADDR_LEN)) {
			if (!db_has_expired(priv, db)) {
				memcpy(skb->data, db->macAddr, ETH_ALEN);
				db->use_count++;
			}
			return 1;
		}
		db = db->next_hash;
	}
	return 0;
}

static void network_hash_unlink(struct host_nat25_db_entry *ent)
{
	*(ent->pprev_hash) = ent->next_hash;
	if (ent->next_hash)
		ent->next_hash->pprev_hash = ent->pprev_hash;
	ent->next_hash = NULL;
	ent->pprev_hash = NULL;
}

void host_nat25_db_cleanup(host_nat25_db_adapter *priv)
{
	int i;

	for (i = 0; i < NAT25_HASH_SIZE; i++) {
		struct host_nat25_db_entry *f = priv->nethash[i];

		while (f) {
			struct host_nat25_db_entry *g = f->next_hash;

			if (priv->scdb_entry == f) {
				memset(priv->scdb_mac, 0, ETH_ALEN);
				memset(priv->scdb_ip, 0, 4);
				priv->scdb_entry = NULL;
			}
			network_hash_unlink(f);
			free(f);
			f = g;
		}
	}
}

void host_nat25_db_expire(host_nat25_db_adapter *priv)
{
	int i;

	for (i = 0; i < NAT25_HASH_SIZE; i++) {
		struct host_nat25_db_entry *f = priv->nethash[i];

		while (f) {
			struct host_nat25_db_entry *g = f->next_hash;

			if (db_has_expired(priv, f) && --f->use_count == 0) {
				if (priv->scdb_entry == f) {
					memset(priv->scdb_mac, 0, ETH_ALEN);
					memset(priv->scdb_ip, 0, 4);
					priv->scdb_entry = NULL;
				}
				network_hash_unlink(f);
				free(f);
			}
			f = g;
		}
	}
}

int host_nat25_db_count(host_nat25_db_adapter *priv)
{
	int i, n = 0;

	for (i = 0; i < NAT25_HASH_SIZE; i++) {
		struct host_nat25_db_entry *db = priv->nethash[i];

		while (db) {
			n++;
			db = db->next_hash;
		}
	}
	return n;
}
