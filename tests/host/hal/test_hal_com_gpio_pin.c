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

static void run_output_at(u8 pin, u8 outputval)
{
	struct _adapter *a = host_gpio_pin_adapter();
	u32 base;
	u8 idx_bit;
	u8 mod_before;
	u8 sel_before;
	u8 out_before;
	char tag[32];

	if (pin <= 7) {
		base = REG_GPIO_PIN_CTRL;
		idx_bit = pin;
	} else if (pin <= 15) {
		base = REG_GPIO_PIN_CTRL_2;
		idx_bit = pin - 8;
	} else {
		return;
	}

	host_gpio_pin_reset();
	mod_before = host_gpio_pin_reg_get(base, 3);
	sel_before = host_gpio_pin_reg_get(base, 2);
	out_before = host_gpio_pin_reg_get(base, 1);
	rtw_hal_set_output_gpio(a, pin, outputval);

	snprintf(tag, sizeof(tag), "out%u_count", pin);
	expect_u32(tag, host_gpio_pin_mmio_count(), 3);
	expect_mmio(0, base + 3, mod_before & ~BIT(idx_bit));
	expect_mmio(1, base + 2, sel_before | BIT(idx_bit));
	if (outputval)
		expect_mmio(2, base + 1, out_before | BIT(idx_bit));
	else
		expect_mmio(2, base + 1, out_before & ~BIT(idx_bit));
}

static void run_input_at(u8 pin)
{
	struct _adapter *a = host_gpio_pin_adapter();
	u32 base;
	u8 idx_bit;
	u8 mod_before;
	u8 sel_before;
	char tag[32];

	if (pin <= 7) {
		base = REG_GPIO_PIN_CTRL;
		idx_bit = pin;
	} else if (pin <= 15) {
		base = REG_GPIO_PIN_CTRL_2;
		idx_bit = pin - 8;
	} else {
		return;
	}

	host_gpio_pin_reset();
	mod_before = host_gpio_pin_reg_get(base, 3);
	sel_before = host_gpio_pin_reg_get(base, 2);
	rtw_hal_set_input_gpio(a, pin);

	snprintf(tag, sizeof(tag), "in%u_count", pin);
	expect_u32(tag, host_gpio_pin_mmio_count(), 2);
	expect_mmio(0, base + 3, mod_before & ~BIT(idx_bit));
	expect_mmio(1, base + 2, sel_before & ~BIT(idx_bit));
}

static void run_invalid(void)
{
	struct _adapter *a = host_gpio_pin_adapter();

	host_gpio_pin_reset();
	rtw_hal_set_output_gpio(a, 16, 1);
	rtw_hal_set_input_gpio(a, 16);
	rtw_hal_set_output_gpio(a, 20, 1);
	rtw_hal_set_input_gpio(a, 21);

	expect_u32("invalid_mmio", host_gpio_pin_mmio_count(), 0);
	expect_u32("invalid_out_log", host_gpio_pin_trace.invalid_output_logs, 2);
	expect_u32("invalid_in_log", host_gpio_pin_trace.invalid_input_logs, 2);
}

int main(void)
{
	fail_line = 0;

	run_output_at(3, 1);
	run_output_at(5, 0);
	run_output_at(7, 1);
	run_output_at(8, 0);
	run_output_at(10, 0);
	run_output_at(10, 1);
	run_output_at(15, 1);
	run_input_at(4);
	run_input_at(7);
	run_input_at(8);
	run_input_at(12);
	run_input_at(15);
	run_invalid();

	if (fail_line) {
		fprintf(stderr, "hal_com_gpio_pin L2 failed\n");
		return 1;
	}
	printf("OK hal_com_gpio_pin L2\n");
	return 0;
}