// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for WAPI PN + IV extension helpers (W3-136).
 */
#include <stdio.h>
#include <string.h>

#include "host_wapi_sms4_pn_iv_oracle.h"

#ifdef RUST_WAPI_PN_IV_ORACLE
extern u8 host_wapi_increase_pn(u8 *pn, u8 add_count);
extern u8 host_wapi_sms4_fill_extension(struct host_wapi_extension *ext,
					u8 key_idx, u8 *pn, u8 add_count);
#endif

static int test_increase_simple(void)
{
	u8 pn[16] = { 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	u8 ov;

	ov = host_wapi_increase_pn(pn, 2);
	if (ov != 0 || pn[0] != 0x03) {
		fprintf(stderr, "simple increment: ov=%u pn[0]=%02x\n", ov, pn[0]);
		return -1;
	}
	return 0;
}

static int test_increase_carry(void)
{
	u8 pn[16] = { 0xff, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	u8 ov;

	ov = host_wapi_increase_pn(pn, 1);
	if (ov != 0 || pn[0] != 0x00 || pn[1] != 0x01) {
		fprintf(stderr, "carry increment failed\n");
		return -1;
	}
	return 0;
}

static int test_increase_overflow(void)
{
	u8 pn[16];
	u8 ov;
	int i;

	for (i = 0; i < 16; i++)
		pn[i] = 0xff;
	ov = host_wapi_increase_pn(pn, 1);
	if (ov != 1) {
		fprintf(stderr, "expected overflow flag\n");
		return -1;
	}
	return 0;
}

static int test_increase_null(void)
{
	if (host_wapi_increase_pn(NULL, 1) != 1)
		return -1;
	return 0;
}

static int test_fill_extension(void)
{
	struct host_wapi_extension ext;
	u8 pn[16] = { 0x10, 0x11, 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	u8 ov;

	memset(&ext, 0xaa, sizeof(ext));
	ov = host_wapi_sms4_fill_extension(&ext, 1, pn, 1);
	if (ov != 0 || ext.key_idx != 1 || ext.reserved != 0)
		return -1;
	if (ext.pn[0] != 0x11 || ext.pn[1] != 0x11 || ext.pn[2] != 0x12)
		return -1;
	if (pn[0] != 0x11)
		return -1;
	return 0;
}

static int test_fill_extension_null(void)
{
	struct host_wapi_extension ext;

	if (host_wapi_sms4_fill_extension(NULL, 0, NULL, 1) != 1)
		return -1;
	if (host_wapi_sms4_fill_extension(&ext, 0, NULL, 1) != 1)
		return -1;
	return 0;
}

int main(void)
{
	if (test_increase_null() || test_increase_simple() || test_increase_carry() ||
	    test_increase_overflow() || test_fill_extension() ||
	    test_fill_extension_null()) {
		fprintf(stderr, "wapi_sms4_pn_iv tests failed\n");
		return 1;
	}
#ifdef RUST_WAPI_PN_IV_ORACLE
	printf("all wapi_sms4_pn_iv vectors passed (oracle: rust/rtw_wapi_sms4.rs)\n");
#else
	printf("all wapi_sms4_pn_iv vectors passed (oracle: wapi_sms4_pn_iv_c_oracle.c)\n");
#endif
	return 0;
}
