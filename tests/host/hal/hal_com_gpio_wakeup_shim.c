// SPDX-License-Identifier: GPL-2.0
/* Host fixture for W4-123 GPIO WL ctrl L2. */

#include "host_hal_com_gpio_wakeup_types.h"
#include <string.h>

static struct _adapter g_adapter;
struct host_gpio_wl_trace host_gpio_wl_trace; /* shared with L2 oracle */

void host_gpio_wl_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&host_gpio_wl_trace, 0, sizeof(host_gpio_wl_trace));
}

struct _adapter *host_gpio_wl_adapter(void)
{
	return &g_adapter;
}

void host_gpio_wl_set_ic(u8 ic_type)
{
	g_adapter.hal.version_id.ICType = ic_type;
}

void rtw_hal_set_hwreg(struct _adapter *padapter, u32 variable, u8 *val)
{
	(void)padapter;
	if (variable != HW_SET_GPIO_WL_CTRL)
		return;
	host_gpio_wl_trace.set_hwreg_count++;
	if (val)
		host_gpio_wl_trace.last_enable = *val;
}

void rtw_rust_hal_com_set_hwreg(void *adapter, u32 variable, u8 *val)
{
	rtw_hal_set_hwreg(adapter, variable, val);
}

int rtw_rust_hal_com_hw_set_gpio_wl_ctrl_var(void)
{
	return HW_SET_GPIO_WL_CTRL;
}

u8 rtw_rust_hal_com_gpio_wl_series_match(void *adapter)
{
	struct _adapter *padapter = adapter;
	struct hal_data *hal = GET_HAL_DATA(padapter);
	HAL_VERSION ver = hal->version_id;

	if (IS_8723D_SERIES(ver) || IS_8192F_SERIES(ver) || IS_8822B_SERIES(ver) ||
	    IS_8821C_SERIES(ver) || IS_8822C_SERIES(ver))
		return _TRUE;
	return _FALSE;
}

#if defined(CONFIG_RTL8821A) && defined(CONFIG_SDIO_HCI)
u8 rtw_rust_hal_com_gpio_wl_led_allow_disable(void *adapter, u8 enable)
{
	(void)adapter;
	(void)enable;
	return _TRUE;
}
#else
u8 rtw_rust_hal_com_gpio_wl_led_allow_disable(void *adapter, u8 enable)
{
	(void)adapter;
	(void)enable;
	return _TRUE;
}
#endif