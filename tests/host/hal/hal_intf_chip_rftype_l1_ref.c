// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_chip_rftype_to_hal_rftype (W4-64). */

typedef struct _adapter _adapter;
typedef unsigned char u8;

enum rf_type {
	RF_1T1R = 0,
	RF_TYPE_MAX = 16,
};

enum rf_type rtw_chip_rftype_to_hal_rftype(_adapter *adapter, u8 limit)
{
	(void)adapter;
	(void)limit;
	return RF_1T1R;
}
