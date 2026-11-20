/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_GPIO_WAKEUP_TYPES_H
#define HOST_HAL_COM_GPIO_WAKEUP_TYPES_H

#define _TRUE 1
#define _FALSE 0
#define TRUE _TRUE
#define FALSE _FALSE

typedef unsigned char u8;
typedef unsigned int u32;

/* HAL_IC_TYPE_E — subset for L2 fixtures */
enum {
	CHIP_8188E = 5,
	CHIP_8821 = 7,
	CHIP_8822B = 13,
	CHIP_8723D = 14,
	CHIP_8821C = 15,
	CHIP_8192F = 17,
	CHIP_8822C = 19,
};

/* Matches rtw_led.h host stub values when CONFIG_RTL8821A + CONFIG_SDIO_HCI. */
#define HOST_HW_LED 1
#define HOST_NO_LED 0

typedef struct {
	u8 ICType;
} HAL_VERSION;

struct hal_data {
	HAL_VERSION version_id;
};

struct _adapter {
	struct hal_data hal;
};

/* Matches hal_intf.h ordering for host trace (GPIO block only). */
#define HW_SET_GPIO_WL_CTRL 200

#define GET_HAL_DATA(adapter) (&((adapter)->hal))
#define IS_8723D_SERIES(version) \
	(((version).ICType == CHIP_8723D) ? _TRUE : _FALSE)
#define IS_8192F_SERIES(version) \
	(((version).ICType == CHIP_8192F) ? _TRUE : _FALSE)
#define IS_8822B_SERIES(version) \
	(((version).ICType == CHIP_8822B) ? _TRUE : _FALSE)
#define IS_8821C_SERIES(version) \
	(((version).ICType == CHIP_8821C) ? _TRUE : _FALSE)
#define IS_8822C_SERIES(version) \
	(((version).ICType == CHIP_8822C) ? _TRUE : _FALSE)

struct host_gpio_wl_trace {
	unsigned int set_hwreg_count;
	u8 last_enable;
};

#endif