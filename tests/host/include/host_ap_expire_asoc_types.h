/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_EXPIRE_ASOC_TYPES_H
#define HOST_AP_EXPIRE_ASOC_TYPES_H

#include "host_types.h"

struct sta_info {
	u8 expire_to;
	u8 keep_alive_trycnt;
};

struct sta_priv {
	u8 expire_to;
};

typedef struct {
	struct sta_priv stapriv;
} _adapter;

void expire_timeout_asoc_step(_adapter *padapter, struct sta_info *psta, u8 sta_alive);

#endif /* HOST_AP_EXPIRE_ASOC_TYPES_H */
