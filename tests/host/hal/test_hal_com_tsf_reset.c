// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-143 TSF reset H2C offload. */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_tsf_reset_types.h"

struct _adapter;

void host_tsf_reset_reset(void);
void host_tsf_reset_set_fill_ret(s32 v);
void host_tsf_reset_set_cnt_defaults(u8 cnt0, u8 cnt1);
void host_tsf_reset_set_cnt0_reads(const u8 *vals, unsigned len);
void host_tsf_reset_set_cnt1_reads(const u8 *vals, unsigned len);
unsigned host_tsf_reset_msleep_count(void);
unsigned host_tsf_reset_buddy_abort_count(void);
int host_tsf_reset_h2c_count(void);
u8 host_tsf_reset_h2c_cmd(int idx);
unsigned int host_tsf_reset_h2c_len(int idx);
const u8 *host_tsf_reset_h2c_buf(int idx);
struct _adapter *host_tsf_reset_adapter(void);

#ifdef RUST_HAL_COM_TSF_RESET_ORACLE
s32 rtw_hal_reset_tsf(struct _adapter *adapter, u8 reset_port);
#else
#define rtw_hal_reset_tsf rtw_hal_reset_tsf_c_oracle
#endif

s32 rtw_hal_reset_tsf_c_oracle(struct _adapter *adapter, u8 reset_port);

static int failures;

static void expect_u32(const char *name, unsigned int got, unsigned int expect)
{
	if (got != expect) {
		fprintf(stderr, "FAIL %s got=%u expect=%u\n", name, got, expect);
		failures++;
	}
}

static void expect_rc_and_h2c(const char *tag, s32 dut_rc, s32 oracle_rc,
			      int dut_h2c, int oracle_h2c, u8 dut_b0, u8 dut_b1,
			      u8 oracle_b0, u8 oracle_b1, unsigned dut_sleep,
			      unsigned oracle_sleep)
{
	(void)tag;
	expect_u32("rc", (unsigned)dut_rc, (unsigned)oracle_rc);
	expect_u32("h2c_count", (unsigned)dut_h2c, (unsigned)oracle_h2c);
	expect_u32("msleep", dut_sleep, oracle_sleep);
	if (dut_h2c > 0) {
		expect_u32("h2c_cmd", host_tsf_reset_h2c_cmd(0), H2C_RESET_TSF);
		expect_u32("h2c_len", host_tsf_reset_h2c_len(0), 2);
		expect_u32("h2c_b0", dut_b0, oracle_b0);
		expect_u32("h2c_b1", dut_b1, oracle_b1);
	}
}

static void run_reset_case(u8 port, s32 fill_ret, const u8 *cnt_reads, unsigned read_len,
			   const char *tag)
{
	s32 dut_rc, oracle_rc;
	u8 dut_b0, dut_b1, oracle_b0, oracle_b1;
	const u8 *buf;
	unsigned dut_sleep, oracle_sleep;
	int dut_h2c, oracle_h2c;
	struct _adapter *adap = host_tsf_reset_adapter();

	host_tsf_reset_reset();
	host_tsf_reset_set_fill_ret(fill_ret);
	if (port == HW_PORT0)
		host_tsf_reset_set_cnt0_reads(cnt_reads, read_len);
	else
		host_tsf_reset_set_cnt1_reads(cnt_reads, read_len);
	dut_rc = rtw_hal_reset_tsf(adap, port);
	dut_sleep = host_tsf_reset_msleep_count();
	dut_h2c = host_tsf_reset_h2c_count();
	if (dut_h2c > 0) {
		buf = host_tsf_reset_h2c_buf(0);
		dut_b0 = buf[0];
		dut_b1 = buf[1];
	} else {
		dut_b0 = dut_b1 = 0;
	}

	host_tsf_reset_reset();
	host_tsf_reset_set_fill_ret(fill_ret);
	if (port == HW_PORT0)
		host_tsf_reset_set_cnt0_reads(cnt_reads, read_len);
	else
		host_tsf_reset_set_cnt1_reads(cnt_reads, read_len);
	oracle_rc = rtw_hal_reset_tsf_c_oracle(adap, port);
	oracle_sleep = host_tsf_reset_msleep_count();
	oracle_h2c = host_tsf_reset_h2c_count();
	if (oracle_h2c > 0) {
		buf = host_tsf_reset_h2c_buf(0);
		oracle_b0 = buf[0];
		oracle_b1 = buf[1];
	} else {
		oracle_b0 = oracle_b1 = 0;
	}

	expect_rc_and_h2c(tag, dut_rc, oracle_rc, dut_h2c, oracle_h2c, dut_b0, dut_b1,
			  oracle_b0, oracle_b1, dut_sleep, oracle_sleep);
}

int main(void)
{
	static const u8 early_change[] = {5, 6};
	static const u8 late_ok[] = {5, 5, 5, 5, 5, 6};
	static const u8 stall_ten[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
	static const u8 change_on_tenth[] = {
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,
	};

	failures = 0;
	host_tsf_reset_reset();

	run_reset_case(HW_PORT0, _SUCCESS, early_change, 2, "port0_success");
	host_tsf_reset_reset();
	host_tsf_reset_set_fill_ret(_SUCCESS);
	host_tsf_reset_set_cnt0_reads(early_change, 2);
	(void)rtw_hal_reset_tsf(host_tsf_reset_adapter(), HW_PORT0);
	expect_u32("port0_h2c_b0", host_tsf_reset_h2c_buf(0)[0], 1);
	expect_u32("port0_h2c_b1", host_tsf_reset_h2c_buf(0)[1], 0);

	run_reset_case(2, _SUCCESS, early_change, 2, "port2_success");
	host_tsf_reset_reset();
	host_tsf_reset_set_fill_ret(_SUCCESS);
	host_tsf_reset_set_cnt1_reads(early_change, 2);
	(void)rtw_hal_reset_tsf(host_tsf_reset_adapter(), 2);
	expect_u32("port2_h2c_b0", host_tsf_reset_h2c_buf(0)[0], 0);
	expect_u32("port2_h2c_b1", host_tsf_reset_h2c_buf(0)[1], 1);
	run_reset_case(2, 0, early_change, 1, "h2c_fail_no_poll");

	host_tsf_reset_reset();
	host_tsf_reset_set_fill_ret(_SUCCESS);
	host_tsf_reset_set_cnt0_reads(early_change, 2);
	expect_u32("buddy", host_tsf_reset_buddy_abort_count(), 0);
	(void)rtw_hal_reset_tsf(host_tsf_reset_adapter(), HW_PORT0);
	expect_u32("buddy_after", host_tsf_reset_buddy_abort_count(), 1);

	run_reset_case(HW_PORT0, _SUCCESS, late_ok, 6, "mid_loop_success");
	run_reset_case(HW_PORT0, _SUCCESS, stall_ten, 11, "timeout_fail");
	run_reset_case(HW_PORT0, _SUCCESS, change_on_tenth, 11, "change_at_loop10_fail");

	if (failures)
		return fprintf(stderr, "tests failed (%d)\n", failures), 1;
	puts("OK hal_com_tsf_reset L2");
	return 0;
}