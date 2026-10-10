// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle for WAPI PN increment + IV extension fill (W3-136).
 * Copied from core/rtw_wapi_sms4_rest.c — keep in sync until Rust swap.
 */
#include <string.h>

#include "host_wapi_sms4_pn_iv_oracle.h"

#ifndef WAPI_PN_IV_L1_REF
u8 host_wapi_increase_pn(u8 *pn, u8 add_count)
#else
u8 WapiIncreasePN(u8 *pn, u8 add_count)
#endif
{
	u8 i;

	if (!pn)
		return 1;

	for (i = 0; i < 16; i++) {
		if (pn[i] + add_count <= 0xff) {
			pn[i] += add_count;
			return 0;
		}
		pn[i] += add_count;
		add_count = 1;
	}
	return 1;
}

#ifndef WAPI_PN_IV_L1_REF
u8 host_wapi_sms4_fill_extension(struct host_wapi_extension *ext, u8 key_idx,
				 u8 *pn, u8 add_count)
{
	u8 overflow;

	if (!ext || !pn)
		return 1;

	ext->key_idx = key_idx;
	ext->reserved = 0;
	overflow = host_wapi_increase_pn(pn, add_count);
	memcpy(ext->pn, pn, 16);
	return overflow;
}
#endif
