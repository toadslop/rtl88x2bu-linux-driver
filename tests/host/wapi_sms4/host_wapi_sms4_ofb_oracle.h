/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 oracle API for WAPI SMS4 OFB payload crypt (W3-139 / T17 extension).
 * Provenance: core/rtw_wapi_sms4_rest.c (WapiSMS4Cryption + wrappers).
 */
#ifndef HOST_WAPI_SMS4_OFB_ORACLE_H
#define HOST_WAPI_SMS4_OFB_ORACLE_H

#include "host_types.h"

void host_wapi_sms4_encryption(const u8 *key, const u8 *iv, const u8 *input,
			       u16 input_length, u8 *output, u16 *output_length);

void host_wapi_sms4_decryption(const u8 *key, const u8 *iv, const u8 *input,
			       u16 input_length, u8 *output, u16 *output_length);

#endif /* HOST_WAPI_SMS4_OFB_ORACLE_H */
