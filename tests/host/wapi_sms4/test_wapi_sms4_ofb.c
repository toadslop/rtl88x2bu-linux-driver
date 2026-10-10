// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for WAPI SMS4 OFB payload crypt (W3-139 / T17).
 * Expected outputs match core/rtw_wapi_sms4_rest.c (WAPI_LITTLE_ENDIAN SMS4 core).
 */
#include <stdio.h>
#include <string.h>

#include "host_wapi_sms4_ofb_oracle.h"

static const u8 k_key[16] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
};

static const u8 k_iv[16] = {
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
};

struct ofb_case {
	const char *name;
	u16 input_len;
	const u8 *expect;
	u16 expect_len;
	int roundtrip;
};

static u8 k_plain32[32];
static u8 k_cipher16[16] = {
	0x87, 0x04, 0x1f, 0x13, 0x18, 0x36, 0x32, 0x87,
	0x2f, 0x3d, 0xf6, 0xb4, 0x44, 0xf0, 0x55, 0x42,
};
static u8 k_cipher32[32] = {
	0x87, 0x04, 0x1f, 0x13, 0x18, 0x36, 0x32, 0x87,
	0x2f, 0x3d, 0xf6, 0xb4, 0x44, 0xf0, 0x55, 0x42,
	0x43, 0x74, 0xac, 0xd9, 0x7b, 0x6a, 0xbc, 0x17,
	0xcd, 0x66, 0x56, 0x7d, 0xfe, 0xb3, 0x92, 0x9c,
};
static u8 k_cipher17[17] = {
	0x87, 0x04, 0x1f, 0x13, 0x18, 0x36, 0x32, 0x87,
	0x2f, 0x3d, 0xf6, 0xb4, 0x44, 0xf0, 0x55, 0x42,
	0x43,
};
static u8 k_cipher31[31] = {
	0x87, 0x04, 0x1f, 0x13, 0x18, 0x36, 0x32, 0x87,
	0x2f, 0x3d, 0xf6, 0xb4, 0x44, 0xf0, 0x55, 0x42,
	0x43, 0x74, 0xac, 0xd9, 0x7b, 0x6a, 0xbc, 0x17,
	0xcd, 0x66, 0x56, 0x7d, 0xfe, 0xb3, 0x92,
};

static int run_case(const struct ofb_case *c)
{
	u8 out[64], dec[64];
	u16 out_len, dec_len;

	host_wapi_sms4_encryption(k_key, k_iv, k_plain32, c->input_len, out, &out_len);
	if (out_len != c->expect_len || memcmp(out, c->expect, c->expect_len) != 0) {
		fprintf(stderr, "%s: encrypt mismatch (len %u)\n", c->name, out_len);
		return -1;
	}
	if (!c->roundtrip)
		return 0;

	host_wapi_sms4_decryption(k_key, k_iv, out, out_len, dec, &dec_len);
	if (dec_len != c->input_len || memcmp(dec, k_plain32, c->input_len) != 0) {
		fprintf(stderr, "%s: decrypt roundtrip mismatch\n", c->name);
		return -1;
	}
	return 0;
}

int main(void)
{
	static const struct ofb_case k_cases[] = {
		{ "ofb-one-block", 16, k_cipher16, 16, 1 },
		{ "ofb-two-blocks", 32, k_cipher32, 32, 1 },
		{ "ofb-partial-17", 17, k_cipher17, 17, 1 },
		{ "ofb-partial-31", 31, k_cipher31, 31, 0 },
	};
	int i;

	for (i = 0; i < 32; i++)
		k_plain32[i] = (u8)i;

	for (i = 0; i < (int)(sizeof(k_cases) / sizeof(k_cases[0])); i++) {
		if (run_case(&k_cases[i]) != 0)
			return 1;
	}

#ifdef RUST_WAPI_SMS4_ORACLE
	printf("all wapi_sms4_ofb vectors passed (oracle: rust/rtw_wapi_sms4.rs)\n");
#else
	printf("all wapi_sms4_ofb vectors passed (oracle: core/rtw_wapi_sms4_rest.c)\n");
#endif
	return 0;
}
