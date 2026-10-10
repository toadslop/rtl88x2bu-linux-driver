/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 oracle API for WAPI PN + IV extension helpers (W3-136 / T17 extension).
 * Provenance: core/rtw_wapi_sms4_rest.c (WapiIncreasePN + IV fill pattern).
 */
#ifndef HOST_WAPI_SMS4_PN_IV_ORACLE_H
#define HOST_WAPI_SMS4_PN_IV_ORACLE_H

#include "host_types.h"

struct host_wapi_extension {
	u8 key_idx;
	u8 reserved;
	u8 pn[16];
};

u8 host_wapi_increase_pn(u8 *pn, u8 add_count);

/* Fill WLAN WAPI extension header; mutates *pn via WapiIncreasePN. Returns overflow flag. */
u8 host_wapi_sms4_fill_extension(struct host_wapi_extension *ext, u8 key_idx,
				 u8 *pn, u8 add_count);

#endif /* HOST_WAPI_SMS4_PN_IV_ORACLE_H */
