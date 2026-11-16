// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_GET_ODM_VAR_TYPES_H
#define HOST_HAL_DM_GET_ODM_VAR_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

struct phydm_fat_struct_host {
	u8 rx_idle_ant;
};

struct dm_struct {
	struct phydm_fat_struct_host dm_fat_table;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

typedef struct {
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

/* Mirror include/hal_intf.h HAL_ODM_VARIABLE (through RX_Dframe). */
#define HAL_ODM_STA_INFO 0
#define HAL_ODM_P2P_STATE 1
#define HAL_ODM_WIFI_DISPLAY_STATE 2
#define HAL_ODM_REGULATION 3
#define HAL_ODM_INITIAL_GAIN 4
#define HAL_ODM_RX_INFO_DUMP 5
#define HAL_ODM_RX_Dframe_INFO 6
#ifdef CONFIG_ANTENNA_DIVERSITY
#define HAL_ODM_ANTDIV_SELECT 7
#endif

#define PHYDM_INFO_CURR_IGI 20u

struct host_hal_dm_get_odm_var_trace {
	u32 phydm_cmn_query_last;
	u32 phydm_cmn_query_ret;
};

extern struct host_hal_dm_get_odm_var_trace host_hal_dm_get_odm_var_trace;

void host_hal_dm_get_odm_var_reset(void);

u32 phydm_cmn_info_query(struct dm_struct *dm, u32 info_type);
u8 rtw_phydm_get_cur_igi(_adapter *adapter);

void GetHalODMVar(_adapter *adapter, int e_variable, void *p_value1, void *p_value2);

#endif