// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for SMS4 block helpers (T17).
 * Vectors: GB/T 32907-2016 inputs; expected ciphertexts match
 * core/rtw_wapi_sms4.c (WAPI_LITTLE_ENDIAN).
 */
#include <stdio.h>
#include <string.h>

#include "host_wapi_sms4_oracle.h"

struct sms4_case {
	const char *name;
	const u8 key[16];
	const u8 plain[16];
	const u8 cipher[16];
	int roundtrip;
};

static const struct sms4_case k_cases[] = {
	{
		.name = "gbt32907-example1-ecb",
		.key = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 },
		.plain = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			   0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 },
		.cipher = { 0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
			    0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46 },
	},
	{
		.name = "gbt32907-example2-zero-key-block",
		.key = { 0 },
		.plain = { 0 },
		.cipher = { 0x9f, 0x1f, 0x7b, 0xff, 0x6f, 0x55, 0x11, 0x38,
			    0x4d, 0x94, 0x30, 0x53, 0x1e, 0x53, 0x8f, 0xd3 },
	},
	{
		.name = "gbt32907-example3-zero-key-plain",
		.key = { 0 },
		.plain = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
		.cipher = { 0xf2, 0xb5, 0x10, 0x7e, 0xff, 0xf1, 0x25, 0xcc,
			    0x01, 0x89, 0xf7, 0x6a, 0x8b, 0x63, 0xce, 0x04 },
		.roundtrip = 1,
	},
};

static int run_case(const struct sms4_case *c)
{
	u32 rk_enc[32], rk_dec[32];
	u8 out[16], dec[16];

	host_sms4_key_ext(c->key, rk_enc, HOST_SMS4_ENCRYPT);
	host_sms4_crypt(c->plain, out, rk_enc);
	if (memcmp(out, c->cipher, 16) != 0) {
		fprintf(stderr, "%s: ciphertext mismatch\n", c->name);
		return -1;
	}
	if (!c->roundtrip)
		return 0;
	host_sms4_key_ext(c->key, rk_dec, HOST_SMS4_DECRYPT);
	host_sms4_crypt(out, dec, rk_dec);
	if (memcmp(dec, c->plain, 16) != 0) {
		fprintf(stderr, "%s: decrypt roundtrip mismatch\n", c->name);
		return -1;
	}
	return 0;
}

static int run_xor_block(void)
{
	const u8 a[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	const u8 b[16] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
			   0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
	const u8 expect[16] = { 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
				0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0 };
	u8 out[16];

	host_sms4_xor_block(out, a, b);
	if (memcmp(out, expect, 16) != 0) {
		fprintf(stderr, "xor_block-leaf: mismatch\n");
		return -1;
	}
	return 0;
}

int main(void)
{
	size_t i;
	int failed = 0;

	for (i = 0; i < sizeof(k_cases) / sizeof(k_cases[0]); i++) {
		if (run_case(&k_cases[i]) != 0)
			failed++;
		else
			printf("ok %s\n", k_cases[i].name);
	}
	if (run_xor_block() != 0)
		failed++;
	else
		printf("ok xor_block-leaf\n");

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
#ifdef RUST_WAPI_SMS4_ORACLE
	printf("all wapi_sms4 vectors passed (oracle: rust/rtw_wapi_sms4.rs)\n");
#else
	printf("all wapi_sms4 vectors passed (oracle: core/rtw_wapi_sms4.c)\n");
#endif
	return 0;
}
