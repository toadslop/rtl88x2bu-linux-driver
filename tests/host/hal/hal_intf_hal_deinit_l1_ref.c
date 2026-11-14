// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_hal_deinit (W4-75). */

typedef struct _adapter _adapter;

unsigned int rtw_hal_deinit(_adapter *padapter)
{
	(void)padapter;
	return 1;
}