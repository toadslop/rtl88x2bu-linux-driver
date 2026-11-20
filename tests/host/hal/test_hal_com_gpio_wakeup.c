// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-123 rtw_hal_switch_gpio_wl_ctrl. */

#include "host_hal_com_gpio_wakeup_types.h"
#include <stdio.h>

void host_gpio_wl_reset(void);
struct _adapter *host_gpio_wl_adapter(void);
void host_gpio_wl_set_ic(u8 ic_type);
#if defined(CONFIG_RTL8821A) && defined(CONFIG_SDIO_HCI)
void host_gpio_wl_set_led_strategy(u8 strategy);
#endif
extern struct host_gpio_wl_trace host_gpio_wl_trace;

#ifdef RUST_HAL_COM_GPIO_WAKEUP_ORACLE
void rtw_hal_switch_gpio_wl_ctrl(struct _adapter *padapter, u8 index, u8 enable);
#else
void rtw_hal_switch_gpio_wl_ctrl(struct _adapter *padapter, u8 index, u8 enable);
#endif

void rtw_hal_switch_gpio_wl_ctrl_c_oracle(struct _adapter *padapter, u8 index, u8 enable);

static int failures;

static void reset_trace(void)
{
	host_gpio_wl_trace.set_hwreg_count = 0;
	host_gpio_wl_trace.last_enable = 0;
}

static unsigned call_under_test(u8 ic, u8 index, u8 enable)
{
	reset_trace();
	host_gpio_wl_set_ic(ic);
	rtw_hal_switch_gpio_wl_ctrl(host_gpio_wl_adapter(), index, enable);
	return host_gpio_wl_trace.set_hwreg_count;
}

static unsigned call_oracle(u8 ic, u8 index, u8 enable)
{
	reset_trace();
	host_gpio_wl_set_ic(ic);
	rtw_hal_switch_gpio_wl_ctrl_c_oracle(host_gpio_wl_adapter(), index, enable);
	return host_gpio_wl_trace.set_hwreg_count;
}

static void expect_eq(const char *tag, unsigned a, unsigned b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void expect_count_enable(const char *tag, u8 ic, u8 index, u8 enable,
				 unsigned want_count, u8 want_enable)
{
	unsigned got = call_under_test(ic, index, enable);

	expect_eq(tag, got, want_count);
	if (want_count)
		expect_eq(tag, host_gpio_wl_trace.last_enable, want_enable);
}

int main(void)
{
	host_gpio_wl_reset();

	/* Non-supported series: no hwreg unless GPIO 13/14. */
	expect_count_enable("8188e_5", CHIP_8188E, 5, 1, 0, 0);
	expect_count_enable("8188e_12", CHIP_8188E, 12, 1, 0, 0);
	expect_count_enable("8188e_15", CHIP_8188E, 15, 0, 0, 0);
	expect_count_enable("8188e_13_e1", CHIP_8188E, 13, 1, 1, 1);
	expect_count_enable("8188e_14_e0", CHIP_8188E, 14, 0, 1, 0);

	expect_count_enable("8723d_8_e1", CHIP_8723D, 8, 1, 1, 1);
	expect_count_enable("8192f_8_e0", CHIP_8192F, 8, 0, 1, 0);
	expect_count_enable("8821c_8_e1", CHIP_8821C, 8, 1, 1, 1);
	expect_count_enable("8822c_8_e1", CHIP_8822C, 8, 1, 1, 1);

	expect_count_enable("8822b_5_e0", CHIP_8822B, 5, 0, 1, 0);
	expect_count_enable("8822b_12_e1", CHIP_8822B, 12, 1, 1, 1);
	expect_count_enable("8822b_13_e1", CHIP_8822B, 13, 1, 2, 1);
	expect_count_enable("8822b_14_e0", CHIP_8822B, 14, 0, 2, 0);

	expect_eq("pair_8822b_14", call_under_test(CHIP_8822B, 14, 0),
		  call_oracle(CHIP_8822B, 14, 0));
	expect_eq("pair_8723d", call_under_test(CHIP_8723D, 8, 1),
		  call_oracle(CHIP_8723D, 8, 1));
	expect_eq("pair_8192f_13", call_under_test(CHIP_8192F, 13, 1),
		  call_oracle(CHIP_8192F, 13, 1));
	expect_eq("pair_8821c", call_under_test(CHIP_8821C, 8, 1),
		  call_oracle(CHIP_8821C, 8, 1));
	expect_eq("pair_8822c", call_under_test(CHIP_8822C, 8, 1),
		  call_oracle(CHIP_8822C, 8, 1));
	expect_eq("pair_8188e_13", call_under_test(CHIP_8188E, 13, 1),
		  call_oracle(CHIP_8188E, 13, 1));

#if defined(CONFIG_RTL8821A) && defined(CONFIG_SDIO_HCI)
	host_gpio_wl_set_ic(CHIP_8821);
	host_gpio_wl_set_led_strategy(HOST_HW_LED);
	expect_count_enable("8821as_hwled_13_dis", CHIP_8821, 13, 0, 0, 0);
	expect_count_enable("8821as_hwled_14_en", CHIP_8821, 14, 1, 1, 1);
#endif

	if (failures)
		return 1;
	puts("OK hal_com_gpio_wakeup L2");
	return 0;
}