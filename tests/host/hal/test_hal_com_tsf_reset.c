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
unsigned host_tsf_reset_reg0_read_count(void);
unsigned host_tsf_reset_reg1_read_count(void);
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

struct tsf_reset_expect {
	s32 rc;
	unsigned sleep;
	int h2c_count;
	u8 h2c_cmd;
	unsigned h2c_len;
	u8 h2c_b0;
	u8 h2c_b1;
	unsigned reg0_reads;
	unsigned reg1_reads;
};

static void expect_u32_tag(const char *tag, const char *name, unsigned int got,
			   unsigned int expect)
{
	if (got != expect)
		fprintf(stderr, "FAIL %s %s got=%u expect=%u\n", tag, name, got, expect),
			failures++;
}

static void expect_case(const char *tag, s32 dut_rc, unsigned dut_sleep, int dut_h2c,
			u8 dut_cmd, unsigned dut_len, u8 dut_b0, u8 dut_b1,
			unsigned dut_reg0, unsigned dut_reg1,
			const struct tsf_reset_expect *exp)
{
	expect_u32_tag(tag, "rc", (unsigned)dut_rc, (unsigned)exp->rc);
	expect_u32_tag(tag, "msleep", dut_sleep, exp->sleep);
	expect_u32_tag(tag, "h2c_count", (unsigned)dut_h2c, (unsigned)exp->h2c_count);
	expect_u32_tag(tag, "reg0_reads", dut_reg0, exp->reg0_reads);
	expect_u32_tag(tag, "reg1_reads", dut_reg1, exp->reg1_reads);
	if (exp->h2c_count > 0) {
		expect_u32_tag(tag, "h2c_cmd", dut_cmd, exp->h2c_cmd);
		expect_u32_tag(tag, "h2c_len", dut_len, exp->h2c_len);
		expect_u32_tag(tag, "h2c_b0", dut_b0, exp->h2c_b0);
		expect_u32_tag(tag, "h2c_b1", dut_b1, exp->h2c_b1);
	}
}

static void capture_after_reset(u8 port, s32 fill_ret, const u8 *cnt_reads,
				unsigned read_len, s32 *rc, unsigned *sleep, int *h2c,
				u8 *cmd, unsigned *len, u8 *b0, u8 *b1,
				unsigned *reg0, unsigned *reg1)
{
	const u8 *buf;

	host_tsf_reset_reset();
	host_tsf_reset_set_fill_ret(fill_ret);
	if (port == HW_PORT0)
		host_tsf_reset_set_cnt0_reads(cnt_reads, read_len);
	else
		host_tsf_reset_set_cnt1_reads(cnt_reads, read_len);

	*rc = rtw_hal_reset_tsf(host_tsf_reset_adapter(), port);
	*sleep = host_tsf_reset_msleep_count();
	*h2c = host_tsf_reset_h2c_count();
	*reg0 = host_tsf_reset_reg0_read_count();
	*reg1 = host_tsf_reset_reg1_read_count();
	if (*h2c > 0) {
		buf = host_tsf_reset_h2c_buf(0);
		*cmd = host_tsf_reset_h2c_cmd(0);
		*len = host_tsf_reset_h2c_len(0);
		*b0 = buf[0];
		*b1 = buf[1];
	} else {
		*cmd = 0;
		*len = 0;
		*b0 = *b1 = 0;
	}
}

static void run_reset_case(u8 port, s32 fill_ret, const u8 *cnt_reads, unsigned read_len,
			   const char *tag, const struct tsf_reset_expect *exp)
{
	s32 dut_rc, oracle_rc;
	unsigned dut_sleep, oracle_sleep;
	unsigned dut_reg0, dut_reg1, oracle_reg0, oracle_reg1;
	int dut_h2c, oracle_h2c;
	u8 dut_cmd, dut_b0, dut_b1;
	u8 oracle_cmd, oracle_b0, oracle_b1;
	unsigned dut_len, oracle_len;

	capture_after_reset(port, fill_ret, cnt_reads, read_len, &dut_rc, &dut_sleep,
			    &dut_h2c, &dut_cmd, &dut_len, &dut_b0, &dut_b1, &dut_reg0,
			    &dut_reg1);
	expect_case(tag, dut_rc, dut_sleep, dut_h2c, dut_cmd, dut_len, dut_b0, dut_b1,
		    dut_reg0, dut_reg1, exp);

	capture_after_reset(port, fill_ret, cnt_reads, read_len, &oracle_rc, &oracle_sleep,
			    &oracle_h2c, &oracle_cmd, &oracle_len, &oracle_b0, &oracle_b1,
			    &oracle_reg0, &oracle_reg1);

	expect_u32_tag(tag, "oracle_rc", (unsigned)oracle_rc, (unsigned)exp->rc);
	expect_u32_tag(tag, "dut_vs_oracle_rc", (unsigned)dut_rc, (unsigned)oracle_rc);
	expect_u32_tag(tag, "dut_vs_oracle_sleep", dut_sleep, oracle_sleep);
	expect_u32_tag(tag, "dut_vs_oracle_h2c", (unsigned)dut_h2c, (unsigned)oracle_h2c);
	if (dut_h2c > 0) {
		expect_u32_tag(tag, "dut_vs_oracle_cmd", dut_cmd, oracle_cmd);
		expect_u32_tag(tag, "dut_vs_oracle_b0", dut_b0, oracle_b0);
		expect_u32_tag(tag, "dut_vs_oracle_b1", dut_b1, oracle_b1);
	}
}

int main(void)
{
	static const u8 early_change[] = {5, 6};
	static const u8 late_ok[] = {5, 5, 5, 5, 5, 6};
	static const u8 stall_ten[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
	static const u8 change_on_tenth[] = {
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,
	};
	static const struct tsf_reset_expect port0_early = {
		.rc = _SUCCESS, .sleep = 1, .h2c_count = 1,
		.h2c_cmd = H2C_RESET_TSF, .h2c_len = 2, .h2c_b0 = 1, .h2c_b1 = 0,
		.reg0_reads = 2, .reg1_reads = 0,
	};
	static const struct tsf_reset_expect port2_early = {
		.rc = _SUCCESS, .sleep = 1, .h2c_count = 1,
		.h2c_cmd = H2C_RESET_TSF, .h2c_len = 2, .h2c_b0 = 0, .h2c_b1 = 1,
		.reg0_reads = 0, .reg1_reads = 2,
	};
	static const struct tsf_reset_expect h2c_fail = {
		.rc = _FAIL, .sleep = 0, .h2c_count = 1,
		.h2c_cmd = H2C_RESET_TSF, .h2c_len = 2, .h2c_b0 = 0, .h2c_b1 = 1,
		.reg0_reads = 0, .reg1_reads = 1,
	};
	static const struct tsf_reset_expect late_ok_exp = {
		.rc = _SUCCESS, .sleep = 5, .h2c_count = 1,
		.h2c_cmd = H2C_RESET_TSF, .h2c_len = 2, .h2c_b0 = 1, .h2c_b1 = 0,
		.reg0_reads = 6, .reg1_reads = 0,
	};
	static const struct tsf_reset_expect timeout_exp = {
		.rc = _FAIL, .sleep = 10, .h2c_count = 1,
		.h2c_cmd = H2C_RESET_TSF, .h2c_len = 2, .h2c_b0 = 1, .h2c_b1 = 0,
		.reg0_reads = 11, .reg1_reads = 0,
	};

	failures = 0;
	host_tsf_reset_reset();

	run_reset_case(HW_PORT0, _SUCCESS, early_change, 2, "port0_success", &port0_early);
	run_reset_case(2, _SUCCESS, early_change, 2, "port2_success", &port2_early);
	run_reset_case(2, _FAIL, early_change, 1, "h2c_fail_no_poll", &h2c_fail);

	host_tsf_reset_reset();
	host_tsf_reset_set_fill_ret(_SUCCESS);
	host_tsf_reset_set_cnt0_reads(early_change, 2);
	expect_u32_tag("buddy", "buddy", host_tsf_reset_buddy_abort_count(), 0);
	(void)rtw_hal_reset_tsf(host_tsf_reset_adapter(), HW_PORT0);
	expect_u32_tag("buddy", "buddy_after", host_tsf_reset_buddy_abort_count(), 1);

	run_reset_case(HW_PORT0, _SUCCESS, late_ok, 6, "mid_loop_success", &late_ok_exp);
	run_reset_case(HW_PORT0, _SUCCESS, stall_ten, 11, "timeout_fail", &timeout_exp);
	run_reset_case(HW_PORT0, _SUCCESS, change_on_tenth, 11, "change_at_loop10_fail",
		       &timeout_exp);

	if (failures)
		return fprintf(stderr, "tests failed (%d)\n", failures), 1;
	puts("OK hal_com_tsf_reset L2");
	return 0;
}