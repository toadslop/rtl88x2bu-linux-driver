/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 oracle API for WAPI SMS4 MIC (W3-140 / T17).
 * Provenance: core/rtw_wapi_sms4_rest.c (WapiSMS4CalculateMic).
 */
#ifndef HOST_WAPI_SMS4_MIC_ORACLE_H
#define HOST_WAPI_SMS4_MIC_ORACLE_H

#include "host_types.h"

void host_wapi_sms4_calculate_mic(const u8 *key, const u8 *iv,
				  const u8 *input1, u8 input1_length,
				  const u8 *input2, u16 input2_length,
				  u8 *output, u8 *output_length);

#endif /* HOST_WAPI_SMS4_MIC_ORACLE_H */
