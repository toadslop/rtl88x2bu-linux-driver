// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_hal_fw_dl (W4-71). */

typedef int s32;
typedef unsigned char u8;
typedef struct _adapter _adapter;

s32 rtw_hal_fw_dl(_adapter *padapter, u8 wowlan)
{
	(void)padapter;
	(void)wowlan;
	return 0;
}