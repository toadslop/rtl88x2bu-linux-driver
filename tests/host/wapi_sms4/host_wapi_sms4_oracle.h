/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 oracle API for SMS4 block primitives (T17).
 * Provenance: core/rtw_wapi_sms4.c (CONFIG_WAPI_SW_SMS4 section).
 */
#ifndef HOST_WAPI_SMS4_ORACLE_H
#define HOST_WAPI_SMS4_ORACLE_H

#include "host_types.h"

#define HOST_SMS4_ENCRYPT 0
#define HOST_SMS4_DECRYPT 1

void host_sms4_crypt(const u8 *input, u8 *output, u32 *rk);
void host_sms4_key_ext(const u8 *key, u32 *rk, u32 crypt_flag);
void host_sms4_xor_block(void *dst, const void *src1, const void *src2);

#endif /* HOST_WAPI_SMS4_ORACLE_H */
