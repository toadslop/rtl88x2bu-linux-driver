// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-140 rtw_hal_set_{output,input}_gpio (hal_com_rest.c layout). */

#include "host_hal_com_gpio_pin_types.h"

#ifndef CONFIG_RTL8192F

void rtw_hal_set_output_gpio_c_oracle(struct _adapter *padapter, u8 index,
				      u8 outputval)
{
	if (index <= 7) {
		rtw_write8(padapter, REG_GPIO_PIN_CTRL + 3,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL + 3) & ~BIT(index));
		rtw_write8(padapter, REG_GPIO_PIN_CTRL + 2,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL + 2) | BIT(index));
		if (outputval)
			rtw_write8(padapter, REG_GPIO_PIN_CTRL + 1,
				   rtw_read8(padapter, REG_GPIO_PIN_CTRL + 1) |
					   BIT(index));
		else
			rtw_write8(padapter, REG_GPIO_PIN_CTRL + 1,
				   rtw_read8(padapter, REG_GPIO_PIN_CTRL + 1) &
					   ~BIT(index));
	} else if (index <= 15) {
		index -= 8;
		rtw_write8(padapter, REG_GPIO_PIN_CTRL_2 + 3,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL_2 + 3) &
				   ~BIT(index));
		rtw_write8(padapter, REG_GPIO_PIN_CTRL_2 + 2,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL_2 + 2) |
				   BIT(index));
		if (outputval)
			rtw_write8(padapter, REG_GPIO_PIN_CTRL_2 + 1,
				   rtw_read8(padapter, REG_GPIO_PIN_CTRL_2 + 1) |
					   BIT(index));
		else
			rtw_write8(padapter, REG_GPIO_PIN_CTRL_2 + 1,
				   rtw_read8(padapter, REG_GPIO_PIN_CTRL_2 + 1) &
					   ~BIT(index));
	} else {
		rtw_rust_hal_com_gpio_pin_invalid_output(padapter, index,
							 outputval);
	}
}

void rtw_hal_set_input_gpio_c_oracle(struct _adapter *padapter, u8 index)
{
	if (index <= 7) {
		rtw_write8(padapter, REG_GPIO_PIN_CTRL + 3,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL + 3) & ~BIT(index));
		rtw_write8(padapter, REG_GPIO_PIN_CTRL + 2,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL + 2) & ~BIT(index));
	} else if (index <= 15) {
		index -= 8;
		rtw_write8(padapter, REG_GPIO_PIN_CTRL_2 + 3,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL_2 + 3) &
				   ~BIT(index));
		rtw_write8(padapter, REG_GPIO_PIN_CTRL_2 + 2,
			   rtw_read8(padapter, REG_GPIO_PIN_CTRL_2 + 2) &
				   ~BIT(index));
	} else {
		rtw_rust_hal_com_gpio_pin_invalid_input(padapter, index);
	}
}

#ifndef RUST_HAL_COM_GPIO_PIN_ORACLE
void rtw_hal_set_output_gpio(struct _adapter *padapter, u8 index, u8 outputval)
{
	rtw_hal_set_output_gpio_c_oracle(padapter, index, outputval);
}

void rtw_hal_set_input_gpio(struct _adapter *padapter, u8 index)
{
	rtw_hal_set_input_gpio_c_oracle(padapter, index);
}
#endif

#endif /* !CONFIG_RTL8192F */