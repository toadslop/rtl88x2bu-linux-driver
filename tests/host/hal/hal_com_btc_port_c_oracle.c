// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-144 BT WLAN port H2C (hal_com_rest.c). */

#include "host_hal_com_btc_port_types.h"

extern u8 rtw_hal_get_port(void *adapter);
extern s32 rtw_hal_fill_h2c_cmd(void *adapter, u8 id, u32 len, u8 *buf);

static void set_h2ccmd_btc_wl_port_id(u8 *h2c_buf, u8 hw_port)
{
	h2c_buf[0] = (h2c_buf[0] & 0xf0) | (hw_port & 0x0f);
}

s32 rtw_hal_set_wifi_btc_port_id_cmd_c_oracle(struct _adapter *adapter)
{
	u8 h2c_buf[H2C_BTC_WL_PORT_ID_LEN] = {0};
	u8 hw_port = rtw_hal_get_port(adapter);

	set_h2ccmd_btc_wl_port_id(h2c_buf, hw_port);
	return rtw_hal_fill_h2c_cmd(adapter, H2C_BTC_WL_PORT_ID, H2C_BTC_WL_PORT_ID_LEN,
				    h2c_buf);
}