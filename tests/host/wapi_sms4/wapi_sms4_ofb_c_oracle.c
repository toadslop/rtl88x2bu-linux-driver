// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle for WAPI SMS4 OFB payload helpers (W3-139).
 * Copied from core/rtw_wapi_sms4_rest.c — keep in sync until Rust swap.
 */
#include <string.h>

#include "host_wapi_sms4_ofb_oracle.h"
#include "host_wapi_sms4_oracle.h"

#define ENCRYPT 0

static void wapi_sms4_cryption(const u8 *key, const u8 *iv, const u8 *input,
			       u16 input_length, u8 *output, u16 *output_length,
			       u32 crypt_flag)
{
	u32 block_num, i, j, rk[32];
	u16 remainder;
	u8 block_in[16], block_out[16], temp_iv[16], k;

	*output_length = 0;
	remainder = input_length & 0x0f;
	block_num = input_length >> 4;
	if (remainder != 0)
		block_num++;
	else
		remainder = 16;

	for (k = 0; k < 16; k++)
		temp_iv[k] = iv[15 - k];

	memcpy(block_in, temp_iv, 16);

	host_sms4_key_ext(key, rk, crypt_flag);

	for (i = 0; i < block_num - 1; i++) {
		host_sms4_crypt(block_in, block_out, rk);
		host_sms4_xor_block(&output[i * 16], &input[i * 16], block_out);
		memcpy(block_in, block_out, 16);
	}

	*output_length = i * 16;

	host_sms4_crypt(block_in, block_out, rk);

	for (j = 0; j < remainder; j++)
		output[i * 16 + j] = input[i * 16 + j] ^ block_out[j];
	*output_length += remainder;
}

#ifndef WAPI_SMS4_OFB_L1_REF
void host_wapi_sms4_encryption(const u8 *key, const u8 *iv, const u8 *input,
			       u16 input_length, u8 *output, u16 *output_length)
{
	wapi_sms4_cryption(key, iv, input, input_length, output, output_length,
			   ENCRYPT);
}

void host_wapi_sms4_decryption(const u8 *key, const u8 *iv, const u8 *input,
			       u16 input_length, u8 *output, u16 *output_length)
{
	wapi_sms4_cryption(key, iv, input, input_length, output, output_length,
			   ENCRYPT);
}
#else
void WapiSMS4Cryption(u8 *key, u8 *iv, u8 *input, u16 input_length, u8 *output,
		      u16 *output_length, u32 crypt_flag)
{
	wapi_sms4_cryption(key, iv, input, input_length, output, output_length,
			   crypt_flag);
}

void WapiSMS4Encryption(u8 *key, u8 *iv, u8 *input, u16 input_length,
			u8 *output, u16 *output_length)
{
	WapiSMS4Cryption(key, iv, input, input_length, output, output_length,
			 ENCRYPT);
}

void WapiSMS4Decryption(u8 *key, u8 *iv, u8 *input, u16 input_length,
			u8 *output, u16 *output_length)
{
	WapiSMS4Cryption(key, iv, input, input_length, output, output_length,
			 ENCRYPT);
}
#endif
