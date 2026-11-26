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

static void setup_pin_output_low(u8 pin, u8 out_bit)
{
	u32 base = (pin <= 7) ? REG_GPIO_PIN_CTRL : REG_GPIO_PIN_CTRL_2;
	u8 bit = (pin <= 7) ? pin : (u8)(pin - 8);
	u8 en = host_gpio_api_reg_get(base, 2);

	host_gpio_api_reg_set(base, 2, en | BIT(bit));
	host_gpio_api_reg_set(base, 1, out_bit ? (u8)BIT(bit) : 0);
}

static void setup_pin_input(u8 pin, u8 in_bit)
{
	u32 base = (pin <= 7) ? REG_GPIO_PIN_CTRL : REG_GPIO_PIN_CTRL_2;
	u8 bit = (pin <= 7) ? pin : (u8)(pin - 8);
	u8 en = host_gpio_api_reg_get(base, 2);

	host_gpio_api_reg_set(base, 2, en & (u8)~BIT(bit));
	host_gpio_api_reg_set(base, 0, in_bit ? (u8)BIT(bit) : 0);
}

static void test_func_check_early_get(void)
{
	struct _adapter *a = host_gpio_api_adapter();

	host_gpio_api_reset();
	host_gpio_api_set_func_check_fail(3, 1);
	expect_u8("fc_get", DUT_GET(a, 3), 0);
	expect_u32("fc_get_deny", host_gpio_api_trace.ps_deny_count, 0);
	expect_u32("fc_get_leave", host_gpio_api_trace.leave_ps_count, 0);
}

static void test_func_check_early_set(void)
{
	struct _adapter *a = host_gpio_api_adapter();

	host_gpio_api_reset();
	setup_pin_output_low(3, 1);
	host_gpio_api_set_func_check_fail(3, 1);
	expect_i32("fc_set", DUT_SET(a, 3, 1), -1);
	expect_u32("fc_set_deny", host_gpio_api_trace.ps_deny_count, 0);
}

static void test_get_gpio3_output(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_output_low(3, 1);
	c = rtw_hal_get_gpio_c_oracle(a, 3);
	host_gpio_api_reset();
	setup_pin_output_low(3, 1);
	r = DUT_GET(a, 3);
	expect_u8("get3_out", r, c);
	expect_u8("get3_val", r, 1);
	expect_u32("get3_deny", host_gpio_api_trace.ps_deny_count, 1);
	expect_u32("get3_leave", host_gpio_api_trace.leave_ps_count, 1);
}

static void test_get_gpio10_input(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 c, r;

	host_gpio_api_reset();
	setup_pin_input(10, 1);
	c = rtw_hal_get_gpio_c_oracle(a, 10);
	host_gpio_api_reset();
	setup_pin_input(10, 1);
	r = DUT_GET(a, 10);
	expect_u8("get10_in", r, c);
	expect_u8("get10_val", r, 1);
}

static void test_set_gpio3_output_high(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	int c, r;

	host_gpio_api_reset();
	setup_pin_output_low(3, 0);
	c = rtw_hal_set_gpio_output_value_c_oracle(a, 3, 1);
	host_gpio_api_reset();
	setup_pin_output_low(3, 0);
	r = DUT_SET(a, 3, 1);
	expect_i32("set3_out_res", r, c);
	expect_i32("set3_out_res0", r, 0);
	expect_u8("set3_reg", host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 1) & BIT(3), BIT(3));
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
}

static void test_config_gpio10_output(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 before, after_oracle, after_dut;

	host_gpio_api_reset();
	before = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);
	expect_u32("cfg10_reset_mfr", host_gpio_api_trace.multi_func_reset_count, 0);
	(void)rtw_hal_config_gpio_c_oracle(a, 10, 1);
	after_oracle = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL_2, 2, before);
	(void)DUT_CFG(a, 10, 1);
	after_dut = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL_2, 2);

	expect_u8("cfg10_en", after_dut, after_oracle);
	expect_u8("cfg10_bit", after_dut & (u8)BIT(2), (u8)BIT(2));
	expect_u32("cfg10_mfr", host_gpio_api_trace.multi_func_reset_count, 1);
}

static void test_config_gpio3_input(void)
{
	struct _adapter *a = host_gpio_api_adapter();
	u8 after_oracle, after_dut;

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL, 2, 0xff);
	(void)rtw_hal_config_gpio_c_oracle(a, 3, 0);
	after_oracle = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 2);

	host_gpio_api_reset();
	host_gpio_api_reg_set(REG_GPIO_PIN_CTRL, 2, 0xff);
	(void)DUT_CFG(a, 3, 0);
	after_dut = host_gpio_api_reg_get(REG_GPIO_PIN_CTRL, 2);

	expect_u8("cfg3_en", after_dut, after_oracle);
	expect_u8("cfg3_bit_clear", after_dut & BIT(3), 0);
	expect_u32("cfg3_mfr", host_gpio_api_trace.multi_func_reset_count, 1);
}

int main(void)
{
	failures = 0;

	test_func_check_early_get();
	test_func_check_early_set();
	test_get_gpio3_output();
	test_get_gpio10_input();
	test_set_gpio3_output_high();
	test_set_gpio3_input_dir_returns_255();
	test_config_gpio10_output();
	test_config_gpio3_input();

	if (failures) {
		fprintf(stderr, "hal_com_gpio_api L2 failed (%d)\n", failures);
		return 1;
	}
	printf("OK hal_com_gpio_api L2\n");
	return 0;
}