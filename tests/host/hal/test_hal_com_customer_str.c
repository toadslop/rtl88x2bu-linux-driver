// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-86 customer string sync API. */

#include <stdio.h>
#include <string.h>

struct _adapter;

#define _SUCCESS 1
#define _FAIL 0

void host_customer_str_reset(void);
void host_customer_str_set_auto_complete(int on);
void host_customer_str_set_req_cmd_result(unsigned char ret);
void host_customer_str_set_write_cmd_result(unsigned char ret);
void host_customer_str_fill_buf(const unsigned char *src);
int host_customer_str_h2c_count(void);
unsigned char host_customer_str_h2c_cmd(int idx);
struct _adapter *host_customer_str_adapter(void);

#ifdef RUST_HAL_COM_CUSTOMER_STR_ORACLE
long rtw_hal_customer_str_read(struct _adapter *adapter, unsigned char *cs);
long rtw_hal_h2c_customer_str_write(struct _adapter *adapter, const unsigned char *cs);
long rtw_hal_customer_str_write(struct _adapter *adapter, const unsigned char *cs);
#else
#define rtw_hal_customer_str_read rtw_hal_customer_str_read_c_oracle
#define rtw_hal_h2c_customer_str_write rtw_hal_h2c_customer_str_write_c_oracle
#define rtw_hal_customer_str_write rtw_hal_customer_str_write_c_oracle
#endif

long rtw_hal_customer_str_read_c_oracle(struct _adapter *adapter, unsigned char *cs);
long rtw_hal_h2c_customer_str_write_c_oracle(struct _adapter *adapter, const unsigned char *cs);
long rtw_hal_customer_str_write_c_oracle(struct _adapter *adapter, const unsigned char *cs);

static int fail_line;

static void expect_eq_long(const char *name, long got, long expect)
{
	if (got != expect) {
		fprintf(stderr, "FAIL %s got=%ld expect=%ld\n", name, got, expect);
		fail_line = __LINE__;
	}
}

static void test_h2c_packing(void)
{
	static const unsigned char cs[16] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x11, 0x12,
		0x13, 0x14, 0x15, 0x16, 0x21, 0x22, 0x23, 0x24,
	};
	long rc, rc_o;

	host_customer_str_reset();
	rc = rtw_hal_h2c_customer_str_write(host_customer_str_adapter(), cs);
	expect_eq_long("h2c_write_c", rc, _SUCCESS);
	if (host_customer_str_h2c_count() != 3)
		fprintf(stderr, "FAIL h2c_count=%d expect 3\n", host_customer_str_h2c_count());

	host_customer_str_reset();
	rc_o = rtw_hal_h2c_customer_str_write_c_oracle(host_customer_str_adapter(), cs);
	expect_eq_long("h2c_write_o", rc_o, _SUCCESS);
	if (host_customer_str_h2c_cmd(0) != 0xC6 || host_customer_str_h2c_cmd(1) != 0xC7 ||
	    host_customer_str_h2c_cmd(2) != 0xC8)
		fprintf(stderr, "FAIL h2c cmd ids\n");
}

static void test_read_success(void)
{
	static const unsigned char expect[16] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
	unsigned char out[16];
	unsigned char out_o[16];
	long rc, rc_o;

	host_customer_str_reset();
	host_customer_str_set_auto_complete(1);
	host_customer_str_set_req_cmd_result(_SUCCESS);
	host_customer_str_fill_buf(expect);

	rc = rtw_hal_customer_str_read(host_customer_str_adapter(), out);
	rc_o = rtw_hal_customer_str_read_c_oracle(host_customer_str_adapter(), out_o);
	expect_eq_long("read_ret", rc, rc_o);
	if (memcmp(out, out_o, sizeof(out)) != 0)
		fprintf(stderr, "FAIL read payload mismatch\n");
}

int main(void)
{
	test_h2c_packing();
	test_read_success();
	if (fail_line)
		return 1;
	puts("OK hal_com_customer_str L2");
	return 0;
}