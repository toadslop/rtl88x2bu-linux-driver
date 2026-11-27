// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-142 CONFIG_GPIO_API (body in rust/hal_com_gpio_api.rs). */

struct _adapter;

unsigned char rtw_hal_get_gpio(struct _adapter *adapter, unsigned char gpio_num)
{
	(void)adapter;
	(void)gpio_num;
	return 0;
}

int rtw_hal_set_gpio_output_value(struct _adapter *adapter, unsigned char gpio_num,
				  unsigned char is_high)
{
	(void)adapter;
	(void)gpio_num;
	(void)is_high;
	return 0;
}

int rtw_hal_config_gpio(struct _adapter *adapter, unsigned char gpio_num,
			unsigned char is_output)
{
	(void)adapter;
	(void)gpio_num;
	(void)is_output;
	return 0;
}