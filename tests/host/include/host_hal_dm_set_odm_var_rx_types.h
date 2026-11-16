// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_SET_ODM_VAR_RX_TYPES_H
#define HOST_HAL_DM_SET_ODM_VAR_RX_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint32_t u32;
typedef uint8_t BOOLEAN;

#define HAL_ODM_RX_INFO_DUMP 5u
#define HAL_ODM_RX_DFRAME_INFO 6u

#define FA_OFDM 0u
#define FA_CCK 1u
#define FA_TOTAL 2u

struct dm_struct {
	u8 is_linked;
	u8 rx_rate;
	u8 rssi_a;
	u8 rssi_b;
	u8 rssi_c;
	u8 rssi_d;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

typedef struct {
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_print_capture {
	char buf[4096];
	size_t len;
};

void host_hal_dm_print_capture_reset(struct host_hal_dm_print_capture *cap);
const char *host_hal_dm_print_capture_text(const struct host_hal_dm_print_capture *cap);

void host_hal_dm_set_odm_var_rx_fixture(
	u8 is_linked, u8 rx_rate, u8 rssi_a, u8 rssi_b,
	u8 cur_igi, s8 phydm_min_rssi, s8 dm_min_rssi,
	u32 fa_cck, u32 fa_ofdm, u32 fa_total);

void SetHalODMVar(_adapter *adapter, u32 eVariable, void *pValue1, BOOLEAN bSet);

u8 rtw_phydm_get_cur_igi(_adapter *adapter);
s8 rtw_phydm_get_min_rssi(_adapter *adapter);
s8 rtw_dm_get_min_rssi(_adapter *adapter);
u32 rtw_phydm_get_phy_cnt(_adapter *adapter, u32 cnt);
void _RTW_PRINT_SEL(void *sel, const char *fmt, ...);
const char *host_hdata_rate_str(u8 rate);
int host_is_hardware_type_8814a(_adapter *adapter);

#endif