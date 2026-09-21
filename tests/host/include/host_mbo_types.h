/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MBO_TYPES_H
#define HOST_MBO_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _VENDOR_SPECIFIC_IE_ 221

int _rtw_memcmp(const void *s1, const void *s2, size_t n);
u8 *rtw_get_ie(const u8 *pbuf, s32 index, s32 *len, s32 limit);

u8 *host_mbo_ie_get(u8 *pie, u32 *plen, u32 limit);
u8 *host_mbo_attrs_get(u8 *pie, u32 limit, u8 attr_id, u32 *attr_len);

#endif /* HOST_MBO_TYPES_H */
