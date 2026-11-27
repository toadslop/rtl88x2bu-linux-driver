// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-142 CONFIG_GPIO_API helpers (hal_com_rest.c layout). */

#include "host_hal_com_gpio_api_types.h"

u8 rtw_hal_get_gpio_c_oracle(struct _adapter *adapter, u8 gpio_num)
{
	u8 value = 0;
	u8 direction = 0;
	u32 gpio_pin_input_val = REG_GPIO_PIN_CTRL;
	u32 gpio_pin_output_val = REG_GPIO_PIN_CTRL + 1;
	u32 gpio_pin_output_en = REG_GPIO_PIN_CTRL + 2;
	u8 gpio_num_to_set = gpio_num;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(adapter);

	if (rtw_hal_gpio_func_check(adapter, gpio_num) == _FAIL)
		return value;

	rtw_ps_deny(adapter, PS_DENY_IOCTL);

	host_gpio_api_note_rf_pwrstate(pwrpriv->rf_pwrstate);
	LeaveAllPowerSaveModeDirect(adapter);

	if (gpio_num > 7) {
		gpio_pin_input_val = REG_GPIO_PIN_CTRL_2;
		gpio_pin_output_val = REG_GPIO_PIN_CTRL_2 + 1;
		gpio_pin_output_en = REG_GPIO_PIN_CTRL_2 + 2;
		gpio_num_to_set = gpio_num - 8;
	}

	direction = (rtw_read8(adapter, gpio_pin_output_en) & BIT(gpio_num_to_set)) >>
		    gpio_num_to_set;

	if (direction)
		value = (rtw_read8(adapter, gpio_pin_output_val) & BIT(gpio_num_to_set)) >>
			gpio_num_to_set;
	else
		value = (rtw_read8(adapter, gpio_pin_input_val) & BIT(gpio_num_to_set)) >>
			gpio_num_to_set;

	rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);

	return value;
}

int rtw_hal_set_gpio_output_value_c_oracle(struct _adapter *adapter, u8 gpio_num,
					   u8 isHigh)
{
	u8 direction = 0;
	u8 res = (u8)-1;
	u32 gpio_pin_output_val = REG_GPIO_PIN_CTRL + 1;
	u32 gpio_pin_output_en = REG_GPIO_PIN_CTRL + 2;
	u8 gpio_num_to_set = gpio_num;

	if (rtw_hal_gpio_func_check(adapter, gpio_num) == _FAIL)
		return -1;

	rtw_ps_deny(adapter, PS_DENY_IOCTL);

	LeaveAllPowerSaveModeDirect(adapter);

	if (gpio_num > 7) {
		gpio_pin_output_val = REG_GPIO_PIN_CTRL_2 + 1;
		gpio_pin_output_en = REG_GPIO_PIN_CTRL_2 + 2;
		gpio_num_to_set = gpio_num - 8;
	}

	direction = (rtw_read8(adapter, gpio_pin_output_en) & BIT(gpio_num_to_set)) >>
		    gpio_num_to_set;

	if (direction) {
		if (isHigh)
			rtw_write8(adapter, gpio_pin_output_val,
				   rtw_read8(adapter, gpio_pin_output_val) |
					   BIT(gpio_num_to_set));
		else
			rtw_write8(adapter, gpio_pin_output_val,
				   rtw_read8(adapter, gpio_pin_output_val) &
					   ~BIT(gpio_num_to_set));
		res = 0;
	}

	rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
	return (int)res;
}

int rtw_hal_config_gpio_c_oracle(struct _adapter *adapter, u8 gpio_num, u8 isOutput)
{
	u32 gpio_ctrl_reg_to_set = REG_GPIO_PIN_CTRL + 2;
	u8 gpio_num_to_set = gpio_num;

	if (rtw_hal_gpio_func_check(adapter, gpio_num) == _FAIL)
		return -1;

	rtw_ps_deny(adapter, PS_DENY_IOCTL);

	LeaveAllPowerSaveModeDirect(adapter);

	rtw_hal_gpio_multi_func_reset(adapter, gpio_num);

	if (gpio_num > 7) {
		gpio_ctrl_reg_to_set = REG_GPIO_PIN_CTRL_2 + 2;
		gpio_num_to_set = gpio_num - 8;
	}

	if (isOutput)
		rtw_write8(adapter, gpio_ctrl_reg_to_set,
			   rtw_read8(adapter, gpio_ctrl_reg_to_set) | BIT(gpio_num_to_set));
	else
		rtw_write8(adapter, gpio_ctrl_reg_to_set,
			   rtw_read8(adapter, gpio_ctrl_reg_to_set) & ~BIT(gpio_num_to_set));

	rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);

	return 0;
}

#ifndef RUST_HAL_COM_GPIO_API_ORACLE
u8 rtw_hal_get_gpio(struct _adapter *adapter, u8 gpio_num)
{
	return rtw_hal_get_gpio_c_oracle(adapter, gpio_num);
}

int rtw_hal_set_gpio_output_value(struct _adapter *adapter, u8 gpio_num, u8 isHigh)
{
	return rtw_hal_set_gpio_output_value_c_oracle(adapter, gpio_num, isHigh);
}

int rtw_hal_config_gpio(struct _adapter *adapter, u8 gpio_num, u8 isOutput)
{
	return rtw_hal_config_gpio_c_oracle(adapter, gpio_num, isOutput);
}
#endif