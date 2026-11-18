// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: dynamic SOML helpers (W4-111). */

typedef unsigned char u8;
typedef unsigned int u32;
typedef struct _adapter _adapter;

void rtw_dyn_soml_byte_update(_adapter *adapter, u8 data_rate, u32 size)
{
	(void)adapter;
	(void)data_rate;
	(void)size;
}

void rtw_dyn_soml_para_set(_adapter *adapter, u8 train_num, u8 intvl,
			   u8 period, u8 delay)
{
	(void)adapter;
	(void)train_num;
	(void)intvl;
	(void)period;
	(void)delay;
}

void rtw_dyn_soml_config(_adapter *adapter)
{
	(void)adapter;
}