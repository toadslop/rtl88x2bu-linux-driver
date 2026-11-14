// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_ODM_IFACE_TYPES_H
#define HOST_HAL_DM_ODM_IFACE_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

struct dm_struct {
	void *adapter;
};

struct hal_data_host {
	struct dm_struct odmpriv;
	u8 phydm_op_mode;
	u32 version_id;
	u16 CustomerID;
	u8 rf_type;
};

struct registry_host {
	u8 wifi_spec;
	u8 adv_ota;
};

typedef struct {
	struct hal_data_host HalData;
	struct registry_host registrypriv;
	u8 host_intf_type;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

#define ODM_CE 0x04u
#define ODM_ITRF_SDIO 0x04u
#define RTW_GSPI 8u
#define PHYDM_PERFORMANCE_MODE 0u

#define ODM_CMNINFO_PLATFORM 0u
#define ODM_CMNINFO_INTERFACE 2u
#define ODM_CMNINFO_MP_TEST_CHIP 3u
#define ODM_CMNINFO_RF_TYPE 9u
#define ODM_CMNINFO_PATCH_ID 25u
#define ODM_CMNINFO_BWIFI_TEST 27u
#define ODM_CMNINFO_ADVANCE_OTA 44u
#define HAL_ODM_REGULATION 3

#define NORMAL_CHIP 1u

struct host_hal_dm_odm_iface_trace {
	u64 platform;
	u64 interface;
	u64 mp_test_chip;
	u64 patch_id;
	u64 wifi_test;
	u64 advance_ota;
	u64 rf_type;
	u8 phydm_op_mode;
	void *phydm_adapter;
	u8 ic_type_init;
	u8 regulation_set;
};

extern struct host_hal_dm_odm_iface_trace host_hal_dm_odm_iface_trace;

void host_hal_dm_odm_iface_reset(void);
void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value);

void rtw_phydm_priv_init(_adapter *adapter);
void rtw_rust_hal_dm_init_odm_cominfo_head(_adapter *adapter);
void rtw_rust_hal_dm_init_odm_cominfo_regulation_dfs(_adapter *adapter);

#endif