// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle for WAPI SMS4 MIC (W3-140).
 * Copied from core/rtw_wapi_sms4_rest.c — keep in sync until Rust swap.
 */
#include <string.h>

#include "host_wapi_sms4_mic_oracle.h"
#include "host_wapi_sms4_oracle.h"

#define ENCRYPT 0

static void wapi_sms4_calculate_mic_inner(const u8 *key, const u8 *iv,
					    const u8 *input1, u8 input1_length,
					    const u8 *input2, u16 input2_length,
					    u8 *output, u8 *output_length)
{
	u32 block_num, i, remainder, rk[32];
	u8 block_in[16], block_out[16], temp_block[16], temp_iv[16], k;

	*output_length = 0;
	remainder = input1_length & 0x0f;
	block_num = input1_length >> 4;

	for (k = 0; k < 16; k++)
		temp_iv[k] = iv[15 - k];

	memcpy(block_in, temp_iv, 16);

	host_sms4_key_ext(key, rk, ENCRYPT);

	host_sms4_crypt(block_in, block_out, rk);

	for (i = 0; i < block_num; i++) {
		host_sms4_xor_block(block_in, input1 + i * 16, block_out);
		host_sms4_crypt(block_in, block_out, rk);
	}

	if (remainder != 0) {
		memset(temp_block, 0, 16);
		memcpy(temp_block, input1 + block_num * 16, remainder);

		host_sms4_xor_block(block_in, temp_block, block_out);
		host_sms4_crypt(block_in, block_out, rk);
	}

	remainder = input2_length & 0x0f;
	block_num = input2_length >> 4;

	for (i = 0; i < block_num; i++) {
		host_sms4_xor_block(block_in, input2 + i * 16, block_out);
		host_sms4_crypt(block_in, block_out, rk);
	}

	if (remainder != 0) {
		memset(temp_block, 0, 16);
		memcpy(temp_block, input2 + block_num * 16, remainder);

		host_sms4_xor_block(block_in, temp_block, block_out);
		host_sms4_crypt(block_in, block_out, rk);
	}

	memcpy(output, block_out, 16);
	*output_length = 16;
}

#ifndef WAPI_SMS4_MIC_L1_REF
void host_wapi_sms4_calculate_mic(const u8 *key, const u8 *iv,
				  const u8 *input1, u8 input1_length,
				  const u8 *input2, u16 input2_length,
				  u8 *output, u8 *output_length)
{
	wapi_sms4_calculate_mic_inner(key, iv, input1, input1_length, input2,
				      input2_length, output, output_length);
}
#else
void WapiSMS4CalculateMic(u8 *key, u8 *iv, u8 *input1, u8 input1_length,
			  u8 *input2, u16 input2_length, u8 *output,
			  u8 *output_length)
{
	wapi_sms4_calculate_mic_inner(key, iv, input1, input1_length, input2,
				      input2_length, output, output_length);
}
#endif
