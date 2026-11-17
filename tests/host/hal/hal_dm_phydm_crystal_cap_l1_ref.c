/* SPDX-License-Identifier: GPL-2.0 */
/* L1 reference leaf: crystal cap setter (W4-104). */

typedef unsigned char u8;
typedef u8 boolean;
typedef struct _adapter _adapter;

boolean rtw_phydm_set_crystal_cap(_adapter *adapter, u8 crystal_cap)
{
	(void)adapter;
	(void)crystal_cap;
	return 1;
}