// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-48 rtw_hal_get_port / rtw_init_hal_com_default_value. */

struct _adapter;

unsigned char rtw_hal_get_port(struct _adapter *adapter)
{
	(void)adapter;
	return 0;
}

void rtw_init_hal_com_default_value(struct _adapter *adapter)
{
	(void)adapter;
}
