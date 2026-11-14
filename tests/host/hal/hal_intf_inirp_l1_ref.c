// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_hal_inirp_init / rtw_hal_inirp_deinit (W4-72). */

typedef unsigned int u32;
typedef struct _adapter _adapter;

u32 rtw_hal_inirp_init(_adapter *padapter)
{
	(void)padapter;
	return 1;
}

u32 rtw_hal_inirp_deinit(_adapter *padapter)
{
	(void)padapter;
	return 1;
}