// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-142 CONFIG_GPIO_API helpers. */

#include <stdio.h>
#include "host_hal_com_gpio_api_types.h"

u8 rtw_hal_get_gpio_c_oracle(struct _adapter *adapter, u8 gpio_num);
int rtw_hal_set_gpio_output_value_c_oracle(struct _adapter *adapter, u8 gpio_num,
					   u8 isHigh);
int rtw_hal_config_gpio_c_oracle(struct _adapter *adapter, u8 gpio_num, u8 isOutput);

#ifdef RUST_HAL_COM_GPIO_API_ORACLE
#define DUT_GET rtw_hal_get_gpio
#define DUT_SET rtw_hal_set_gpio_output_value
#define DUT_CFG rtw_hal_config_gpio
#else
#define DUT_GET rtw_hal_get_gpio_c_oracle
#define DUT_SET rtw_hal_set_gpio_output_value_c_oracle
#define DUT_CFG rtw_hal_config_gpio_c_oracle
#endif

static int failures;

static void expect_u32(const char *tag, unsigned int got, unsigned int exp)
{
	if (got != exp) {
		fprintf(stderr, "FAIL %s got=%u exp=%u\n", tag, got, exp);
		failures++;
	}
}

static void expect_u8(const char *tag, unsigned int got, unsigned int exp)
{
	expect_u32(tag, got, exp);
}

static void expect_i32(const char *tag, int got, int exp)
{
	if (got != exp) {
		fprintf(stderr, "FAIL %s got=%d exp=%d\n", tag, got, exp);
		failures++;
	}
}

static void expect_ps_trace_order_on_get(void)
{
	if (host_gpio_api_trace.ps_step_count < 4) {
		fprintf(stderr, "FAIL get_ps_steps too few\n");
		failures++;
		return;
	}
	expect_u8("get_ps0", host_gpio_api_trace.ps_steps[0],
		  HOST_GPIO_API_PS_STEP_DENY);
	expect_u8("get_ps1", host_gpio_api_trace.ps_steps[1],
		  HOST_GPIO_API_PS_STEP_RF_PWRSTATE_LOG);
	expect_u8("get_ps2", host_gpio_api_trace.ps_steps[2],
		  HOST_GPIO_API_PS_STEP_LEAVE_PS);
	expect_u8("get_ps_last", host_gpio_api_trace.ps_steps[host_gpio_api_trace.ps_step_count - 1],
		  HOST_GPIO_API_PS_STEP_DENY_CANCEL);
}

static void setup_pin_output(u8 pin, u8 out_bit)
{
	u32 base = (pin <= 7) ? REG_GPIO_PIN_CTRL : REG_GPIO_PIN_CTRL_2;
	u8 bit = (pin <= 7) ? pin : (u8)(pin - 8);
	u8 en = host_gpio_api_reg_get(base, 2);
	u8 out = host_gpio_api_reg_get(base, 1);

	host_gpio_api_reg_set(base, 2, en | BIT(bit));
	if (out_bit)
		host_gpio_api_reg_set(base, 1, out | (u8)BIT(bit));
	else
		host_gpio_api_reg_set(base, 1, out & (u8)~BIT(bit));
}

static void setup_pin_input(u8 pin, u8 in_bit)
{
	u32 base = (pin <= 7) ? REG_GPIO_PIN_CTRL : REG_GPIO_PIN_CTRL_2;
	u8 bit = (pin <= 7) ? pin : (u8)(pin - 8);
	u8 en = host_gpio_api_reg_get(base, 2);
	u8 in = host_gpio_api_reg_get(base, 0);

	host_gpio_api_reg_set(base, 2, en & (u8)~BIT(bit));
	if (in_bit)
		host_gpio_api_reg_set(base, 0, in | (u8)BIT(bit));
	else
		host_gpio_api_reg_set(base, 0, in & (u8)~BIT(bit));
}

static void test_func_check_early_get(void)
{
	struct _adapter *a = host_gpio_api_adapter();

	host_gpio_api_reset();
	host_gpio_api_set_func_check_fail(3, 1);
	expect_u8("fc_get", DUT_GET(a, 3), 0);
	expect_u32("fc_get_deny", host_gpio_api_trace.ps_deny_count, 0);
	expect_u32("fc_get_leave", host_gpio_api_trace.leave_ps_count, 0);
	expect_u32("fc_get_cancel", host_gpio_api_trace.ps_deny_cancel_count, 0);
	expect_u32("fc_get_rf_log", host_gpio_api_trace.rf_pwrstate_log_count, 0);
}

static void test_func_check_early_set(void)
{
	struct _adapter *a = host_gpio_api_adapter();

	host_gpio_api_reset();
	setup_pin_output(3, 1);
	host_gpio_api_set_func_check_fail(3, 1);
	expect_i32("fc_set", DUT_SET(a, 3, 1), -1);
	expect_u32("fc_set_deny", host_gpio_api_trace.ps_deny_count, 0);
	expect_u32("fc_set_leave", host_gpio_api_trace.leave_ps_count, 0);
	expect_u32("fc_set_cancel", host_gpio_api_trace.ps_deny_cancel_count, 0);
	expect_u32("fc_set_mmio_early", host_gpio_api_trace.mmio_before_leave_count, 0);
}

static void test_func_check_early_config(void)
{
	struct _adapter *a = host_gpio_api_adapter();

	host_gpio_api_reset();
	host_gpio_api_set_func_check_fail(10, 1);
	expect_i32("fc_cfg", DUT_CFG(a, 10, 1), -1);
	expect_u32("fc_cfg_deny", host_gpio_api_trace.ps_deny_count, 0);
	expect_u32("fc_cfg_mfr", host_gpio_api_trace.multi_func_reset_count, 0);
	expect_u32("fc_cfg_leave", host_gpio_api_trace.leave_ps_count, 0);
	expect_u32("fc_cfg_cancel", host_gpio_api_trace.ps_deny_cancel_count, 0);
	expect_u32("fc_cfg_mmio_early", host_gpio_api_trace.mmio_before_leave_count, 0);
}

static void expect_get_ps_success_trace(void)
{
	expect_u32("get_deny", host_gpio_api_trace.ps_deny_count, 1);
	expect_u32("get_leave", host_gpio_api_trace.leave_ps_count, 1);
	expect_u32("get_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("get_deny_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("get_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
	expect_u8("get_rf_logged", host_gpio_api_trace.rf_pwrstate_logged, 0x2a);
	expect_u32("get_rf_log", host_gpio_api_trace.rf_pwrstate_log_count, 1);
	expect_u32("get_mmio_early", host_gpio_api_trace.mmio_before_leave_count, 0);
	expect_u32("get_mmio_after_cancel", host_gpio_api_trace.mmio_after_deny_cancel_count, 0);
	expect_u8("get_first_rd_leave", host_gpio_api_trace.first_read_leave_snapshot, 1);
	expect_u8("get_first_rd_cancel", host_gpio_api_trace.first_read_cancel_snapshot, 0);
	expect_ps_trace_order_on_get();
}

static void test_get_gpio3_output_low(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_output(3, 0);
	c = rtw_hal_get_gpio_c_oracle(a, 3);
	host_gpio_api_reset();
	setup_pin_output(3, 0);
	r = DUT_GET(a, 3);
	expect_u8("get3_lo_out", r, c);
	expect_u8("get3_lo_val", r, 0);
	expect_get_ps_success_trace();
}

static void test_get_gpio3_output_high(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_output(3, 1);
	c = rtw_hal_get_gpio_c_oracle(a, 3);
	host_gpio_api_reset();
	setup_pin_output(3, 1);
	r = DUT_GET(a, 3);
	expect_u8("get3_hi_out", r, c);
	expect_u8("get3_hi_val", r, 1);
	expect_get_ps_success_trace();
}

static void test_get_gpio10_input_low(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_input(10, 0);
	c = rtw_hal_get_gpio_c_oracle(a, 10);
	host_gpio_api_reset();
	setup_pin_input(10, 0);
	r = DUT_GET(a, 10);
	expect_u8("get10_lo_in", r, c);
	expect_u8("get10_lo_val", r, 0);
	expect_get_ps_success_trace();
}

static void test_get_gpio10_input_high(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_input(10, 1);
	c = rtw_hal_get_gpio_c_oracle(a, 10);
	host_gpio_api_reset();
	setup_pin_input(10, 1);
	r = DUT_GET(a, 10);
	expect_u8("get10_hi_in", r, c);
	expect_u8("get10_hi_val", r, 1);
	expect_get_ps_success_trace();
}

static void test_get_gpio3_input_high(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_input(3, 1);
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL, 1, 0);
	c = rtw_hal_get_gpio_c_oracle(a, 3);
	host_gpio_api_reset();
	setup_pin_input(3, 1);
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL, 1, 0);
	r = DUT_GET(a, 3);
	expect_u8("get3_in_hi", r, c);
	expect_u8("get3_in_hi_val", r, 1);
	expect_get_ps_success_trace();
}

static void test_get_gpio10_output_high(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_output(10, 1);
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL_2, 0, 0);
	c = rtw_hal_get_gpio_c_oracle(a, 10);
	host_gpio_api_reset();
	setup_pin_output(10, 1);
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL_2, 0, 0);
	r = DUT_GET(a, 10);
	expect_u8("get10_out_hi", r, c);
	expect_u8("get10_out_hi_val", r, 1);
	expect_get_ps_success_trace();
}

static void test_set_gpio3_output_high(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	int c, r;
	u8 out_before, out_after_c, out_after_r;

	host_gpio_api_reset();
	setup_pin_output(3, 0);
	out_before = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 1);
	c = rtw_hal_set_gpio_output_value_c_oracle(a, 3, 1);
	out_after_c = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 1);

	host_gpio_api_reset();
	setup_pin_output(3, 0);
	r = DUT_SET(a, 3, 1);
	out_after_r = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 1);

	expect_i32("set3_hi_res", r, c);
	expect_i32("set3_hi_res0", r, 0);
	expect_u8("set3_hi_byte", out_after_r, out_after_c);
	expect_u8("set3_hi_rm", out_after_r, (u8)(out_before | BIT(3)));
	expect_u32("set3_hi_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("set3_hi_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("set3_hi_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
	expect_u32("set3_hi_leave", host_gpio_api_trace.leave_ps_count, 1);
	expect_u32("set3_hi_mmio_early", host_gpio_api_trace.mmio_before_leave_count, 0);
	expect_u32("set3_hi_cancel_early", host_gpio_api_trace.deny_cancel_before_leave_count, 0);
	expect_u32("set3_hi_mmio_after_cancel", host_gpio_api_trace.mmio_after_deny_cancel_count, 0);
	expect_u32("set3_hi_mmio_before_deny", host_gpio_api_trace.mmio_before_deny_count, 0);
	expect_u8("set3_hi_first_rd_leave", host_gpio_api_trace.first_read_leave_snapshot, 1);
	expect_u8("set3_hi_first_rd_cancel", host_gpio_api_trace.first_read_cancel_snapshot, 0);
	expect_u8("set3_hi_ps0", host_gpio_api_trace.ps_steps[0], HOST_GPIO_API_PS_STEP_DENY);
	expect_u8("set3_hi_ps1", host_gpio_api_trace.ps_steps[1], HOST_GPIO_API_PS_STEP_LEAVE_PS);
}

static void test_set_gpio3_output_low(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	int c, r;
	u8 out_after_c, out_after_r;

	host_gpio_api_reset();
	setup_pin_output(3, 1);
	c = rtw_hal_set_gpio_output_value_c_oracle(a, 3, 0);
	out_after_c = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 1);

	host_gpio_api_reset();
	setup_pin_output(3, 1);
	r = DUT_SET(a, 3, 0);
	out_after_r = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 1);

	expect_i32("set3_lo_res", r, c);
	expect_u8("set3_lo_byte", out_after_r, out_after_c);
	expect_u8("set3_lo_rm", out_after_r, 0xc1);
	expect_u32("set3_lo_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("set3_lo_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("set3_lo_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
	expect_u32("set3_lo_leave", host_gpio_api_trace.leave_ps_count, 1);
	expect_u32("set3_lo_mmio_early", host_gpio_api_trace.mmio_before_leave_count, 0);
	expect_u32("set3_lo_cancel_early", host_gpio_api_trace.deny_cancel_before_leave_count, 0);
	expect_u32("set3_lo_mmio_after_cancel", host_gpio_api_trace.mmio_after_deny_cancel_count, 0);
	expect_u32("set3_lo_mmio_before_deny", host_gpio_api_trace.mmio_before_deny_count, 0);
	expect_u8("set3_lo_first_rd_leave", host_gpio_api_trace.first_read_leave_snapshot, 1);
	expect_u8("set3_lo_first_rd_cancel", host_gpio_api_trace.first_read_cancel_snapshot, 0);
	expect_u8("set3_lo_ps0", host_gpio_api_trace.ps_steps[0], HOST_GPIO_API_PS_STEP_DENY);
	expect_u8("set3_lo_ps1", host_gpio_api_trace.ps_steps[1], HOST_GPIO_API_PS_STEP_LEAVE_PS);
}

static void test_set_gpio10_output_high(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	int c, r;
	u8 out_before, out_after_c, out_after_r;

	host_gpio_api_reset();
	setup_pin_output(10, 0);
	out_before = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 1);
	c = rtw_hal_set_gpio_output_value_c_oracle(a, 10, 1);
	out_after_c = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 1);

	host_gpio_api_reset();
	setup_pin_output(10, 0);
	r = DUT_SET(a, 10, 1);
	out_after_r = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 1);

	expect_i32("set10_hi_res", r, c);
	expect_u8("set10_hi_byte", out_after_r, out_after_c);
	expect_u8("set10_hi_rm", out_after_r, (u8)(out_before | BIT(2)));
	expect_u32("set10_hi_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("set10_hi_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("set10_hi_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
	expect_u32("set10_hi_leave", host_gpio_api_trace.leave_ps_count, 1);
	expect_u32("set10_hi_mmio_before_deny", host_gpio_api_trace.mmio_before_deny_count, 0);
	expect_u8("set10_hi_first_rd_leave", host_gpio_api_trace.first_read_leave_snapshot, 1);
	expect_u8("set10_hi_first_rd_cancel", host_gpio_api_trace.first_read_cancel_snapshot, 0);
}

static void test_set_gpio10_output_low(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	int c, r;
	u8 out_after_c, out_after_r;

	host_gpio_api_reset();
	setup_pin_output(10, 1);
	c = rtw_hal_set_gpio_output_value_c_oracle(a, 10, 0);
	out_after_c = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 1);

	host_gpio_api_reset();
	setup_pin_output(10, 1);
	r = DUT_SET(a, 10, 0);
	out_after_r = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 1);

	expect_i32("set10_lo_res", r, c);
	expect_u8("set10_lo_byte", out_after_r, out_after_c);
	expect_u8("set10_lo_rm", out_after_r, 0xd1);
	expect_u32("set10_lo_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("set10_lo_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("set10_lo_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
	expect_u32("set10_lo_leave", host_gpio_api_trace.leave_ps_count, 1);
	expect_u32("set10_lo_mmio_before_deny", host_gpio_api_trace.mmio_before_deny_count, 0);
	expect_u8("set10_lo_first_rd_leave", host_gpio_api_trace.first_read_leave_snapshot, 1);
	expect_u8("set10_lo_first_rd_cancel", host_gpio_api_trace.first_read_cancel_snapshot, 0);
	expect_u8("set10_lo_ps0", host_gpio_api_trace.ps_steps[0], HOST_GPIO_API_PS_STEP_DENY);
	expect_u8("set10_lo_ps1", host_gpio_api_trace.ps_steps[1], HOST_GPIO_API_PS_STEP_LEAVE_PS);
}

static void test_set_gpio3_input_dir_returns_255(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	int c, r;

	host_gpio_api_reset();
	setup_pin_input(3, 0);
	c = rtw_hal_set_gpio_output_value_c_oracle(a, 3, 1);
	host_gpio_api_reset();
	setup_pin_input(3, 0);
	r = DUT_SET(a, 3, 1);
	expect_i32("set3_in_res", r, c);
	expect_i32("set3_in_255", r, 255);
	expect_u32("set3_in_writes", host_gpio_api_mmio_write_count(), 0);
	expect_u32("set3_in_deny_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("set3_in_deny_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("set3_in_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
}

static void test_set_gpio10_input_dir_returns_255(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	int c, r;

	host_gpio_api_reset();
	setup_pin_input(10, 0);
	c = rtw_hal_set_gpio_output_value_c_oracle(a, 10, 1);
	host_gpio_api_reset();
	setup_pin_input(10, 0);
	r = DUT_SET(a, 10, 1);
	expect_i32("set10_in_res", r, c);
	expect_i32("set10_in_255", r, 255);
	expect_u32("set10_in_writes", host_gpio_api_mmio_write_count(), 0);
	expect_u32("set10_in_deny_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("set10_in_deny_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("set10_in_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
}

static void test_config_gpio10_output(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 before, after_oracle, after_dut;
	int rc, rd;

	host_gpio_api_reset();
	before = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);
	expect_u32("cfg10_reset_mfr", host_gpio_api_trace.multi_func_reset_count, 0);
	rc = rtw_hal_config_gpio_c_oracle(a, 10, 1);
	after_oracle = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL_2, 2, before);
	rd = DUT_CFG(a, 10, 1);
	after_dut = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);

	expect_i32("cfg10_ret", rd, rc);
	expect_i32("cfg10_ret0", rd, 0);
	expect_u8("cfg10_en", after_dut, after_oracle);
	expect_u8("cfg10_rm", after_dut, (u8)(before | BIT(2)));
	expect_u8("cfg10_bit", after_dut & (u8)BIT(2), (u8)BIT(2));
	expect_u32("cfg10_mfr", host_gpio_api_trace.multi_func_reset_count, 1);
	expect_u8("cfg10_mfr_pin", host_gpio_api_trace.multi_func_reset_last_gpio, 10);
	expect_u32("cfg10_mfr_pre_mmio",
		   host_gpio_api_trace.multi_func_reset_mmio_writes_at_call, 0);
	expect_u32("cfg10_mfr_before_leave",
		   host_gpio_api_trace.multi_func_reset_before_leave_count, 0);
	expect_u32("cfg10_deny_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("cfg10_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("cfg10_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
	expect_u32("cfg10_leave", host_gpio_api_trace.leave_ps_count, 1);
	expect_u32("cfg10_mmio_early", host_gpio_api_trace.mmio_before_leave_count, 0);
	expect_u32("cfg10_cancel_early", host_gpio_api_trace.deny_cancel_before_leave_count, 0);
	expect_u32("cfg10_mmio_after_cancel", host_gpio_api_trace.mmio_after_deny_cancel_count, 0);
	expect_u8("cfg10_ps0", host_gpio_api_trace.ps_steps[0], HOST_GPIO_API_PS_STEP_DENY);
	expect_u8("cfg10_ps1", host_gpio_api_trace.ps_steps[1], HOST_GPIO_API_PS_STEP_LEAVE_PS);
	expect_u8("cfg10_ps2", host_gpio_api_trace.ps_steps[2],
		  HOST_GPIO_API_PS_STEP_MULTI_FUNC_RESET);
	expect_u8("cfg10_first_wr_mfr", host_gpio_api_trace.first_write_mfr_snapshot, 1);
}

static void test_config_gpio3_output(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 before, after_oracle, after_dut;
	int rc, rd;

	host_gpio_api_reset();
	before = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 2);
	rc = rtw_hal_config_gpio_c_oracle(a, 3, 1);
	after_oracle = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 2);

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL, 2, before);
	rd = DUT_CFG(a, 3, 1);
	after_dut = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 2);

	expect_i32("cfg3_out_ret", rd, rc);
	expect_i32("cfg3_out_ret0", rd, 0);
	expect_u8("cfg3_out_en", after_dut, after_oracle);
	expect_u8("cfg3_out_rm", after_dut, (u8)(before | BIT(3)));
	expect_u32("cfg3_out_deny_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("cfg3_out_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
}

static void test_config_gpio10_input(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 after_oracle, after_dut;
	int rc, rd;

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL_2, 2, 0xd6);
	rc = rtw_hal_config_gpio_c_oracle(a, 10, 0);
	after_oracle = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL_2, 2, 0xd6);
	rd = DUT_CFG(a, 10, 0);
	after_dut = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);

	expect_i32("cfg10_in_ret", rd, rc);
	expect_i32("cfg10_in_ret0", rd, 0);
	expect_u8("cfg10_in_en", after_dut, after_oracle);
	expect_u8("cfg10_in_rm", after_dut, 0xd2);
	expect_u32("cfg10_in_deny_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("cfg10_in_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
}

static void test_config_gpio3_input(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 after_oracle, after_dut;
	int rc, rd;

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL, 2, 0xeb);
	rc = rtw_hal_config_gpio_c_oracle(a, 3, 0);
	after_oracle = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 2);

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL, 2, 0xeb);
	rd = DUT_CFG(a, 3, 0);
	after_dut = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 2);

	expect_i32("cfg3_ret", rd, rc);
	expect_i32("cfg3_ret0", rd, 0);
	expect_u8("cfg3_en", after_dut, after_oracle);
	expect_u8("cfg3_bit_clear", after_dut & BIT(3), 0);
	expect_u8("cfg3_rm", after_dut, 0xe3);
	expect_u32("cfg3_mfr", host_gpio_api_trace.multi_func_reset_count, 1);
	expect_u8("cfg3_mfr_pin", host_gpio_api_trace.multi_func_reset_last_gpio, 3);
	expect_u32("cfg3_cancel", host_gpio_api_trace.ps_deny_cancel_count, 1);
	expect_u32("cfg3_deny_reason", host_gpio_api_trace.ps_deny_reason, PS_DENY_IOCTL);
	expect_u32("cfg3_cancel_reason", host_gpio_api_trace.ps_deny_cancel_reason,
		   PS_DENY_IOCTL);
	expect_u32("cfg3_leave", host_gpio_api_trace.leave_ps_count, 1);
	expect_u32("cfg3_mfr_pre_mmio", host_gpio_api_trace.multi_func_reset_mmio_writes_at_call, 0);
	expect_u32("cfg3_mfr_before_leave",
		   host_gpio_api_trace.multi_func_reset_before_leave_count, 0);
	expect_u32("cfg3_mmio_early", host_gpio_api_trace.mmio_before_leave_count, 0);
	expect_u32("cfg3_cancel_early", host_gpio_api_trace.deny_cancel_before_leave_count, 0);
	expect_u32("cfg3_mmio_after_cancel", host_gpio_api_trace.mmio_after_deny_cancel_count, 0);
	expect_u8("cfg3_first_wr_mfr", host_gpio_api_trace.first_write_mfr_snapshot, 1);
	expect_u8("cfg3_ps0", host_gpio_api_trace.ps_steps[0], HOST_GPIO_API_PS_STEP_DENY);
	expect_u8("cfg3_ps1", host_gpio_api_trace.ps_steps[1], HOST_GPIO_API_PS_STEP_LEAVE_PS);
	expect_u8("cfg3_ps2", host_gpio_api_trace.ps_steps[2],
		  HOST_GPIO_API_PS_STEP_MULTI_FUNC_RESET);
}

int main(void)
{
	failures = 0;

	test_func_check_early_get();
	test_func_check_early_set();
	test_func_check_early_config();
	test_get_gpio3_output_low();
	test_get_gpio3_output_high();
	test_get_gpio3_input_high();
	test_get_gpio10_input_low();
	test_get_gpio10_input_high();
	test_get_gpio10_output_high();
	test_set_gpio3_output_high();
	test_set_gpio3_output_low();
	test_set_gpio10_output_high();
	test_set_gpio10_output_low();
	test_set_gpio3_input_dir_returns_255();
	test_set_gpio10_input_dir_returns_255();
	test_config_gpio10_output();
	test_config_gpio10_input();
	test_config_gpio3_output();
	test_config_gpio3_input();

	if (failures) {
		fprintf(stderr, "hal_com_gpio_api L2 failed (%d)\n", failures);
		return 1;
	}
	printf("OK hal_com_gpio_api L2\n");
	return 0;
}