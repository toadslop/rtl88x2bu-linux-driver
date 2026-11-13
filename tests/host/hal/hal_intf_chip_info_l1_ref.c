// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_hal_chip_configure + rtw_hal_read_chip_info (W4-59). */

typedef struct _adapter _adapter;

void rtw_hal_chip_configure(_adapter *padapter)
{
	(void)padapter;
}

unsigned char rtw_hal_read_chip_info(_adapter *padapter)
{
	(void)padapter;
	return 1;
}
