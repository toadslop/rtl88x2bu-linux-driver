// W4-84 — `rtw_phydm_priv_init` + early `Init_ODM_ComInfo` iface block from `hal/hal_dm.c`.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;
type U64 = u64;

const ODM_CE: U32 = 0x04;
const ODM_ITRF_SDIO: U64 = 0x04;
const ODM_CMNINFO_PLATFORM: U32 = 0;
const ODM_CMNINFO_INTERFACE: U32 = 2;
const ODM_CMNINFO_MP_TEST_CHIP: U32 = 3;
const ODM_CMNINFO_RF_TYPE: U32 = 9;
const ODM_CMNINFO_PATCH_ID: U32 = 25;
const ODM_CMNINFO_BWIFI_TEST: U32 = 27;
const ODM_CMNINFO_ADVANCE_OTA: U32 = 44;
const PHYDM_PERFORMANCE_MODE: U8 = 0;
const RTW_GSPI: U8 = 8;
const HAL_ODM_REGULATION: i32 = 3;
const _TRUE: U8 = 1;

unsafe extern "C" {
    fn odm_cmn_info_init(dm: *mut DmStruct, cmn_info: U32, value: U64);
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_phydm_set_adapter(phydm: *mut DmStruct, adapter: *mut c_void);
    fn rtw_rust_hal_dm_set_phydm_op_mode(adapter: *mut c_void, mode: U8);
    fn rtw_rust_hal_dm_mp_test_chip_value(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_patch_id(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_dm_wifi_spec(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_rf_type(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_init_advance_ota_if_enabled(adapter: *mut c_void, phydm: *mut DmStruct);
    fn rtw_rust_hal_dm_get_intf_type(adapter: *mut c_void) -> U8;
    fn rtw_odm_init_ic_type(adapter: *mut c_void);
    fn rtw_hal_set_odm_var(adapter: *mut c_void, variable: i32, pvalue1: *mut c_void, bset: U8);
    fn rtw_rust_hal_dm_hook_dfs_master(adapter: *mut c_void, phydm: *mut DmStruct);
}

#[no_mangle]
pub extern "C" fn rtw_phydm_priv_init(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if phydm.is_null() {
            return;
        }
        rtw_rust_hal_dm_phydm_set_adapter(phydm, adapter);
        odm_cmn_info_init(phydm, ODM_CMNINFO_PLATFORM, ODM_CE as U64);
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_dm_init_odm_cominfo_head(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if phydm.is_null() {
            return;
        }
        rtw_rust_hal_dm_set_phydm_op_mode(adapter, PHYDM_PERFORMANCE_MODE);
        rtw_odm_init_ic_type(adapter);
        let intf = rtw_rust_hal_dm_get_intf_type(adapter);
        let odm_intf = if intf == RTW_GSPI {
            ODM_ITRF_SDIO
        } else {
            intf as U64
        };
        odm_cmn_info_init(phydm, ODM_CMNINFO_INTERFACE, odm_intf);
        let mp_test = rtw_rust_hal_dm_mp_test_chip_value(adapter);
        odm_cmn_info_init(phydm, ODM_CMNINFO_MP_TEST_CHIP, mp_test as U64);
        let patch = rtw_rust_hal_dm_patch_id(adapter);
        odm_cmn_info_init(phydm, ODM_CMNINFO_PATCH_ID, patch as U64);
        let wifi_spec = rtw_rust_hal_dm_wifi_spec(adapter);
        odm_cmn_info_init(phydm, ODM_CMNINFO_BWIFI_TEST, wifi_spec as U64);
        rtw_rust_hal_dm_init_advance_ota_if_enabled(adapter, phydm);
        let rf_type = rtw_rust_hal_dm_rf_type(adapter);
        odm_cmn_info_init(phydm, ODM_CMNINFO_RF_TYPE, rf_type as U64);
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_dm_init_odm_cominfo_regulation_dfs(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if phydm.is_null() {
            return;
        }
        rtw_hal_set_odm_var(
            adapter,
            HAL_ODM_REGULATION,
            core::ptr::null_mut(),
            _TRUE,
        );
        rtw_rust_hal_dm_hook_dfs_master(adapter, phydm);
    }
}