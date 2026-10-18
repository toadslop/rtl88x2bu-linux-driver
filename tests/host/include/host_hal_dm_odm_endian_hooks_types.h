// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_ODM_ENDIAN_HOOKS_TYPES_H
#define HOST_HAL_DM_ODM_ENDIAN_HOOKS_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t s32;

struct dm_struct { int _unused; };

#define ODM_CMNINFO_BAND 54u
#define ODM_CMNINFO_SEC_MODE 56u
#define ODM_CMNINFO_BW 57u
#define ODM_CMNINFO_NET_CLOSED 71u

struct host_hal_dm_odm_endian_fixture {
	u8 band;
	u32 sec_mode;
	u8 bw;
	s32 net_closed;
};

struct host_hal_dm_odm_endian_hook_trace {
	void *band, *sec_mode, *bw, *net_closed;
};

extern struct host_hal_dm_odm_endian_hook_trace host_hal_dm_odm_endian_trace;

void host_hal_dm_odm_endian_reset(void);
void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value);
void odm_cmn_info_hook(struct dm_struct *dm, u32 cmn_info, void *ptr);

void rtw_rust_hal_dm_init_odm_endian_hooks_le(struct dm_struct *pdm_odm, u8 *band,
	u32 *sec_mode, u8 *bw, s32 *net_closed);
void rtw_rust_hal_dm_init_odm_endian_hooks_be(struct dm_struct *pdm_odm, u8 *band,
	u32 *sec_mode, u8 *bw, s32 *net_closed);

#endif
