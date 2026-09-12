/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_EXPIRE_TIMEOUT_TYPES_H
#define HOST_AP_EXPIRE_TIMEOUT_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define NUM_STA 32
/* Matches include/sta_info.h (BW|RATE|PROTECTION|CAP|HT|VHT). */
#define STA_INFO_UPDATE_ALL 0x3f

typedef struct {
	int dummy;
} _adapter;

void associated_clients_update(_adapter *padapter, u8 updated, int flags);

#endif /* HOST_AP_EXPIRE_TIMEOUT_TYPES_H */
