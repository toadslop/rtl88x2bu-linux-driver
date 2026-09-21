/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BR_EXT_DB_TYPES_H
#define HOST_BR_EXT_DB_TYPES_H

#include "host_br_ext_types.h"

#define ETH_ALEN 6

struct host_nat25_db_entry {
	struct host_nat25_db_entry *next_hash;
	struct host_nat25_db_entry **pprev_hash;
	int use_count;
	u8 macAddr[ETH_ALEN];
	unsigned long ageing_timer;
	u8 networkAddr[MAX_NETWORK_ADDR_LEN];
};

typedef struct {
	struct host_nat25_db_entry *nethash[NAT25_HASH_SIZE];
	struct host_nat25_db_entry *scdb_entry;
	u8 scdb_mac[ETH_ALEN];
	u8 scdb_ip[4];
} host_nat25_db_adapter;

#endif
