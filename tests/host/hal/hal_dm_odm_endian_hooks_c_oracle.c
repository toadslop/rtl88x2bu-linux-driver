// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-38 Init_ODM_ComInfo endian hook block (hal/hal_dm.c). */

#include "host_hal_dm_odm_endian_hooks_types.h"

static u8 *convert_to_big_endian(void *value, int size)
{
	return (u8 *)value + size - 1;
}

void rtw_rust_hal_dm_init_odm_endian_hooks_le(
	struct dm_struct *pdm_odm,
	u8 *band,
	u32 *sec_mode,
	u8 *bw,
	s32 *net_closed)
{
	if (!pdm_odm)
		return;

	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BAND, band);
	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_SEC_MODE, sec_mode);
	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BW, bw);
	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_NET_CLOSED, net_closed);
}

void rtw_rust_hal_dm_init_odm_endian_hooks_be(
	struct dm_struct *pdm_odm,
	u8 *band,
	u32 *sec_mode,
	u8 *bw,
	s32 *net_closed)
{
	if (!pdm_odm)
		return;

	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BAND,
		convert_to_big_endian(band, sizeof(u8)));
	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_SEC_MODE,
		convert_to_big_endian(sec_mode, sizeof(u32)));
	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BW,
		convert_to_big_endian(bw, sizeof(u8)));
	odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_NET_CLOSED,
		convert_to_big_endian(net_closed, sizeof(s32)));
}
