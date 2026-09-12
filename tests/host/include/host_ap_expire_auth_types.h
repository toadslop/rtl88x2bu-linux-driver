/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_EXPIRE_AUTH_TYPES_H
#define HOST_AP_EXPIRE_AUTH_TYPES_H

#include "host_types.h"

#define HOST_EXPIRE_AUTH_ADAPTER_SZ 96

void host_expire_auth_adapter_init(u8 *buf);
void host_expire_auth_add_sta(u8 *buf, u8 expire_to);
void host_expire_auth_step(u8 *buf);
u8 host_expire_auth_sta_expire_to(const u8 *buf, u8 index);
u8 host_expire_auth_flush_count(void);

#endif /* HOST_AP_EXPIRE_AUTH_TYPES_H */
