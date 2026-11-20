// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-123 rtw_hal_switch_gpio_wl_ctrl. */

#include "host_hal_com_gpio_wakeup_types.h"
#include <stdio.h>

void host_gpio_wl_reset(void);
struct _adapter *host_gpio_wl_adapter(void);
void host_gpio_wl_set_ic(u8 ic_type);
extern struct host_gpio_wl_trace host_gpio_wl_trace;

#ifdef RUST_HAL_COM_GPIO_WAKEUP_ORACLE
void rtw_hal_switch_gpio_wl_ctrl(struct _adapter *padapter, u8 index, u8 enable);
#else
#define rtw_hal_switch_gpio_wl_ctrl rtw_hal_switch_gpio_wl_ctrl_c_oracle
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

int main(void)
{
	host_gpio_wl_reset();

	/* Non-supported series: no hwreg unless GPIO 13/14. */
	expect_eq("8188e_5", call_oracle(CHIP_8188E, 5, 1), 0);
	expect_eq("8188e_13", call_oracle(CHIP_8188E, 13, 1), 1);

	/* Supported series programs via series check; GPIO 13/14 may add second call. */
	expect_eq("8822b_5", call_oracle(CHIP_8822B, 5, 0), 1);
	expect_eq("8822b_13", call_oracle(CHIP_8822B, 13, 1), 2);

	expect_eq("pair_8822b_14", call_under_test(CHIP_8822B, 14, 0),
		  call_oracle(CHIP_8822B, 14, 0));
	expect_eq("pair_8723d", call_under_test(CHIP_8723D, 8, 1),
		  call_oracle(CHIP_8723D, 8, 1));
	expect_eq("pair_8192f_13", call_under_test(CHIP_8192F, 13, 1),
		  call_oracle(CHIP_8192F, 13, 1));

	if (failures)
		return 1;
	puts("OK hal_com_gpio_wakeup L2");
	return 0;
}