// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_BOARD_TYPE_TYPES_H
#define HOST_HAL_DM_BOARD_TYPE_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

struct dm_struct {
	int _unused;
};

/* Mirrors enum odm_board_type in hal/phydm/phydm_pre_define.h */
#define ODM_BOARD_DEFAULT 0u
#define ODM_BOARD_BT (1u << 2)
#define ODM_BOARD_EXT_PA (1u << 3)
#define ODM_BOARD_EXT_LNA (1u << 4)
#define ODM_BOARD_EXT_PA_5G (1u << 6)
#define ODM_BOARD_EXT_LNA_5G (1u << 7)

/* Mirrors enum odm_cmninfo in hal/phydm/phydm.h (subset). */
#define ODM_CMNINFO_BOARD_TYPE 12u
#define ODM_CMNINFO_EXT_LNA 14u
#define ODM_CMNINFO_5G_EXT_LNA 15u
#define ODM_CMNINFO_EXT_PA 16u
#define ODM_CMNINFO_5G_EXT_PA 17u

struct host_hal_dm_board_trace {
	u8 board_type;
	u8 ext_lna_2g;
	u8 ext_lna_5g;
	u8 ext_pa_2g;
	u8 ext_pa_5g;
};

extern struct host_hal_dm_board_trace host_hal_dm_board_trace;

void host_hal_dm_board_reset(void);
void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value);

void rtw_rust_hal_dm_init_odm_board_type(
	struct dm_struct *pdm_odm,
	u8 external_lna_2g,
	u8 external_lna_5g,
	u8 external_pa_2g,
	u8 external_pa_5g,
	u8 eeprom_bt_coexist);

#endif
