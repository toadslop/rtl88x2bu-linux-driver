/* SPDX-License-Identifier: GPL-2.0 */
/* L1 reference leaf: RRSR control helpers (W4-102). */

typedef unsigned int u32;
typedef unsigned char u8;
typedef struct _adapter _adapter;

void rtw_phydm_set_rrsr(_adapter *adapter, u32 rrsr_value, u8 write_rrsr)
{
	(void)adapter;
	(void)rrsr_value;
	(void)write_rrsr;
}

void rtw_phydm_dyn_rrsr_en(_adapter *adapter, u8 en_rrsr)
{
	(void)adapter;
	(void)en_rrsr;
}