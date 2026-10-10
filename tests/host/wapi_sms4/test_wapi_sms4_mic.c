// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for WAPI SMS4 MIC (W3-140 / T17).
 * Expected outputs match core/rtw_wapi_sms4_rest.c (WAPI_LITTLE_ENDIAN SMS4 core).
 */
#include <stdio.h>
#include <string.h>

#include "host_wapi_sms4_mic_oracle.h"

static const u8 k_key[16] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
};

static const u8 k_iv[16] = {
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
};

static const u8 k_mic_iv_only[16] = {
	0x87, 0x05, 0x1d, 0x10, 0x1c, 0x33, 0x34, 0x80,
	0x27, 0x34, 0xfc, 0xbf, 0x48, 0xfd, 0x5b, 0x4d,
};

static const u8 k_mic_in1_16[16] = {
	0x74, 0xa0, 0x39, 0x08, 0x30, 0x4f, 0x38, 0x7d,
	0xf8, 0x30, 0x88, 0x76, 0xf7, 0x1e, 0xa3, 0x3d,
};

static const u8 k_mic_in1_17[16] = {
	0x46, 0x19, 0x71, 0x8a, 0xb6, 0xc3, 0xb3, 0x00,
	0x42, 0x55, 0xc4, 0x35, 0x18, 0x16, 0xd6, 0xaa,
};

static const u8 k_mic_in1_32[16] = {
	0xeb, 0xe8, 0x9b, 0x48, 0xed, 0xbd, 0x40, 0x37,
	0x07, 0x0f, 0xe6, 0x5b, 0x66, 0xf4, 0x8c, 0x8e,
};

static const u8 k_mic_in2_16[16] = {
	0x14, 0xcb, 0xdb, 0x85, 0x50, 0xe8, 0x03, 0x81,
	0xf6, 0x61, 0xaa, 0x9a, 0x43, 0x20, 0xb3, 0x6b,
};

static const u8 k_mic_in1_10_in2_20[16] = {
	0xb7, 0x5b, 0xcf, 0xa1, 0xd0, 0xee, 0xbc, 0xf3,
	0x7d, 0x20, 0xf8, 0x98, 0xda, 0x71, 0x28, 0x4d,
};

static const u8 k_mic_in1_31_in2_17[16] = {
	0x2a, 0x0b, 0xa7, 0x69, 0x2f, 0x17, 0x7e, 0xd9,
	0x48, 0x2a, 0x2e, 0x91, 0xc3, 0x7a, 0xf8, 0xe5,
};

struct mic_case {
	const char *name;
	u8 input1_len;
	u16 input2_len;
	const u8 *expect;
};

static u8 k_in1_buf[32];
static u8 k_in2_buf[32];

static int run_case(const struct mic_case *c)
{
	u8 out[16], out_len;

	host_wapi_sms4_calculate_mic(k_key, k_iv, k_in1_buf, c->input1_len,
				     k_in2_buf, c->input2_len, out, &out_len);
	if (out_len != 16) {
		fprintf(stderr, "%s: bad output length %u\n", c->name, out_len);
		return -1;
	}
	if (memcmp(out, c->expect, 16) != 0) {
		fprintf(stderr, "%s: MIC mismatch\n", c->name);
		return -1;
	}
	return 0;
}

int main(void)
{
	static const struct mic_case k_cases[] = {
		{ "iv-only", 0, 0, k_mic_iv_only },
		{ "in1-one-block", 16, 0, k_mic_in1_16 },
		{ "in1-partial-17", 17, 0, k_mic_in1_17 },
		{ "in1-two-blocks", 32, 0, k_mic_in1_32 },
		{ "in2-one-block", 0, 16, k_mic_in2_16 },
		{ "in1-partial-in2-multi", 10, 20, k_mic_in1_10_in2_20 },
		{ "in1-partial-in2-partial", 31, 17, k_mic_in1_31_in2_17 },
	};
	int i;

	for (i = 0; i < 32; i++)
		k_in1_buf[i] = (u8)i;
	for (i = 0; i < 32; i++)
		k_in2_buf[i] = (u8)(0xa0 + i);

	for (i = 0; i < (int)(sizeof(k_cases) / sizeof(k_cases[0])); i++) {
		if (run_case(&k_cases[i]) != 0)
			return 1;
	}

#ifdef RUST_WAPI_SMS4_ORACLE
	printf("all wapi_sms4_mic vectors passed (oracle: rust/rtw_wapi_sms4.rs)\n");
#else
	printf("all wapi_sms4_mic vectors passed (oracle: core/rtw_wapi_sms4_rest.c)\n");
#endif
	return 0;
}
