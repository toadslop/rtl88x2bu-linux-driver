// SPDX-License-Identifier: GPL-2.0
/* L1 reference stubs for W4-89 (bodies in rust/hal_dm.rs). */

#include <linux/types.h>

#ifndef u8
typedef unsigned char u8;
#endif
#ifndef u64
typedef unsigned long long u64;
#endif

struct dm_struct;
struct cmn_sta_info;

void rtw_phydm_ops_func_init(struct dm_struct *p_phydm)
{
	(void)p_phydm;
}

void record_ra_info(void *p_dm_void, u8 macid, struct cmn_sta_info *p_sta,
		    u64 ra_mask)
{
	(void)p_dm_void;
	(void)macid;
	(void)p_sta;
	(void)ra_mask;
}