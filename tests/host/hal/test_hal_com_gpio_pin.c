// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-140 GPIO pin MMIO. */

#include <stdio.h>
#include "host_hal_com_gpio_pin_types.h"

static int fail_line;

static void expect_u32(const char *name, unsigned int got, unsigned int exp)
{
	if (got != exp) {
		fprintf(stderr, "FAIL %s got=%u expect=%u\n", name, got, exp);
		fail_line = __LINE__;
	}
}

static void expect_u8(const char *name, unsigned int got, unsigned int exp)
{
	expect_u32(name, got, exp);
}

static void expect_mmio(unsigned int idx, u32 addr, u8 val)
{
	char tag[48];

	if (idx >= host_gpio_pin_mmio_count()) {
		fprintf(stderr, "FAIL mmio idx %u missing\n", idx);
		fail_line = __LINE__;
		return;
	}
	snprintf(tag, sizeof(tag), "mmio%u_addr", idx);
	expect_u32(tag, host_gpio_pin_mmio_addr(idx), addr);
	snprintf(tag, sizeof(tag), "mmio%u_val", idx);
	expect_u8(tag, host_gpio_pin_mmio_val(idx), val);
}

static void run_output_low(u8 index, u8 outputval)
{
	struct _adapter *a = host_gpio_pin_adapter();
	u8 idx_bit = index;
	u8 mod_before;
	u8 sel_before;
	u8 out_before;

	host_gpio_pin_reset();
	mod_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL, 3);
	sel_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL, 2);
	out_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL, 1);
	rtw_hal_set_output_gpio(a, index, outputval);

	expect_u32("out_low_count", host_gpio_pin_mmio_count(), 3);
	expect_mmio(0, REG_GPIO_PIN_CTRL + 3, mod_before & ~BIT(idx_bit));
	expect_mmio(1, REG_GPIO_PIN_CTRL + 2, sel_before | BIT(idx_bit));
	if (outputval)
		expect_mmio(2, REG_GPIO_PIN_CTRL + 1, out_before | BIT(idx_bit));
	else
		expect_mmio(2, REG_GPIO_PIN_CTRL + 1, out_before & ~BIT(idx_bit));
}

static void run_output_high_bank(void)
{
	struct _adapter *a = host_gpio_pin_adapter();
	u8 pin = 10;
	u8 adj = pin - 8;
	u8 mod_before;
	u8 sel_before;
	u8 out_before;

	host_gpio_pin_reset();
	mod_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL_2, 3);
	sel_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL_2, 2);
	out_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL_2, 1);
	rtw_hal_set_output_gpio(a, pin, 0);

	expect_u32("out_high_count", host_gpio_pin_mmio_count(), 3);
	expect_mmio(0, REG_GPIO_PIN_CTRL_2 + 3, mod_before & ~BIT(adj));
	expect_mmio(1, REG_GPIO_PIN_CTRL_2 + 2, sel_before | BIT(adj));
	expect_mmio(2, REG_GPIO_PIN_CTRL_2 + 1, out_before & ~BIT(adj));
}

static void run_input_low(u8 index)
{
	struct _adapter *a = host_gpio_pin_adapter();
	u8 mod_before;
	u8 sel_before;

	host_gpio_pin_reset();
	mod_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL, 3);
	sel_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL, 2);
	rtw_hal_set_input_gpio(a, index);

	expect_u32("in_low_count", host_gpio_pin_mmio_count(), 2);
	expect_mmio(0, REG_GPIO_PIN_CTRL + 3, mod_before & ~BIT(index));
	expect_mmio(1, REG_GPIO_PIN_CTRL + 2, sel_before & ~BIT(index));
}

static void run_input_high_bank(void)
{
	struct _adapter *a = host_gpio_pin_adapter();
	u8 pin = 12;
	u8 adj = pin - 8;
	u8 mod_before;
	u8 sel_before;

	host_gpio_pin_reset();
	mod_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL_2, 3);
	sel_before = host_gpio_pin_reg_get(REG_GPIO_PIN_CTRL_2, 2);
	rtw_hal_set_input_gpio(a, pin);

	expect_u32("in_high_count", host_gpio_pin_mmio_count(), 2);
	expect_mmio(0, REG_GPIO_PIN_CTRL_2 + 3, mod_before & ~BIT(adj));
	expect_mmio(1, REG_GPIO_PIN_CTRL_2 + 2, sel_before & ~BIT(adj));
}

static void run_invalid(void)
{
	struct _adapter *a = host_gpio_pin_adapter();

	host_gpio_pin_reset();
	rtw_hal_set_output_gpio(a, 20, 1);
	rtw_hal_set_input_gpio(a, 21);

	expect_u32("invalid_mmio", host_gpio_pin_mmio_count(), 0);
	expect_u32("invalid_out_log", host_gpio_pin_trace.invalid_output_logs, 1);
	expect_u32("invalid_in_log", host_gpio_pin_trace.invalid_input_logs, 1);
}

int main(void)
{
	fail_line = 0;

	run_output_low(3, 1);
	run_output_low(5, 0);
	run_output_high_bank();
	run_input_low(4);
	run_input_high_bank();
	run_invalid();

	if (fail_line) {
		fprintf(stderr, "hal_com_gpio_pin L2 failed\n");
		return 1;
	}
	printf("OK hal_com_gpio_pin L2\n");
	return 0;
}