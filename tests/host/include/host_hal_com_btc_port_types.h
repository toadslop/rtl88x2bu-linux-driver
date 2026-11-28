/* SPDX-License-Identifier: GPL-2.0 */
/* Host types for W4-144 BT WLAN port H2C L2 harness. */

#ifndef HOST_HAL_COM_BTC_PORT_TYPES_H
#define HOST_HAL_COM_BTC_PORT_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct _adapter;

#define H2C_BTC_WL_PORT_ID 0x71
#define H2C_BTC_WL_PORT_ID_LEN 1
#define _SUCCESS 1

s32 rtw_hal_set_wifi_btc_port_id_cmd(struct _adapter *adapter);

#endif