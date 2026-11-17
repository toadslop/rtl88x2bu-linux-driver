/* SPDX-License-Identifier: GPL-2.0 */
/* L1 reference leaf: IQK debug trigger wrapper (W4-105). */

typedef unsigned char u8;
typedef u8 boolean;
typedef struct _adapter _adapter;

void rtw_phydm_iqk_trigger_dbg(_adapter *adapter, boolean recovery,
			       boolean clear, boolean segment)
{
	(void)adapter;
	(void)recovery;
	(void)clear;
	(void)segment;
}