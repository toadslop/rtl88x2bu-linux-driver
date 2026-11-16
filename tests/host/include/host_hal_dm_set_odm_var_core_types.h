// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_SET_ODM_VAR_CORE_TYPES_H
#define HOST_HAL_DM_SET_ODM_VAR_CORE_TYPES_H

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t BOOLEAN;

#define STA_DM_CTRL_ACTIVE 1u /* BIT(0) from rtw_sta_info.h */

#define HAL_ODM_STA_INFO 0u
#define HAL_ODM_P2P_STATE 1u
#define HAL_ODM_WIFI_DISPLAY_STATE 2u
#define HAL_ODM_REGULATION 3u
#define HAL_ODM_INITIAL_GAIN 4u

/* Indices match enum odm_cmninfo in hal/phydm/phydm.h (ODM_CMNINFO_PLATFORM = 0). */
#define ODM_CMNINFO_WIFI_DIRECT 86u
#define ODM_CMNINFO_WIFI_DISPLAY 87u

#define PHYDM_ADAPINFO_DOMAIN_CODE_2G 4u
#define PHYDM_ADAPINFO_DOMAIN_CODE_5G 5u

#define PHYDM_PAUSE 1u
#define PHYDM_RESUME 3u
#define PHYDM_PAUSE_LEVEL_0 0u

struct rtw_sta_cmn_host {
	u8 mac_id;
	u8 dm_ctrl;
};

struct sta_info {
	struct rtw_sta_cmn_host cmn;
};

struct dm_struct {
	u8 _pad;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

typedef struct {
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_set_odm_var_core_trace {
	u8 sta_hook_macid;
	u8 sta_hook_null;
	u8 sta_log_set;
	u8 sta_log_clean;
	u32 cmn_update_last;
	u64 cmn_update_value;
	u32 adapt_update_last;
	u64 adapt_update_value;
	u8 adapt_call_count;
	u8 pause_type;
	u8 pause_level;
	u8 pause_igi;
};

extern struct host_hal_dm_set_odm_var_core_trace host_hal_dm_set_odm_var_core_trace;

void host_hal_dm_set_odm_var_core_reset(void);
void host_hal_dm_set_odm_var_core_sta_log_set(u8 mac_id);
void host_hal_dm_set_odm_var_core_sta_log_clean(u8 mac_id);

#define RTW_INFO(fmt, ...) \
	do { \
		if (!strcmp(fmt, "### Set STA_(%d) info ###\n")) \
			host_hal_dm_set_odm_var_core_sta_log_set((u8)(__VA_ARGS__)); \
		else if (!strcmp(fmt, "### Clean STA_(%d) info ###\n")) \
			host_hal_dm_set_odm_var_core_sta_log_clean((u8)(__VA_ARGS__)); \
	} while (0)

void phydm_cmn_sta_info_hook(struct dm_struct *dm, u8 mac_id, void *sta);
void odm_cmn_info_update(struct dm_struct *dm, u32 cmn_info, u64 value);
void phydm_adaptivity_info_update(void *dm_void, u32 cmn_info, u64 value);
void odm_pause_dig(void *dm_void, u8 pause_type, u8 pause_level, u8 igi);

void SetHalODMVar(_adapter *adapter, u32 eVariable, void *pValue1, BOOLEAN bSet);

#endif
