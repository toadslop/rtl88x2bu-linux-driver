// W4-79 — `rtw_hal_update_iqk_fw_offload_cap` and param-init FW offload caps from `hal/hal_dm.c`.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U16 = u16;

const FALSE: U8 = 0;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_reg_iqk_fw_offload(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_iqk_sctx_ptr(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_dm_mi_linked(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_lps_leave_iqk_switch(adapter: *mut c_void);
    fn rtw_rust_hal_dm_log_iqk_fw_offload(enable: U8);
    fn rtw_rust_hal_dm_log_param_init_fw_offload(enable: U8);
    fn rtw_rust_hal_dm_phydm_rf_iqk_offload() -> U16;
    fn rtw_rust_hal_dm_phydm_phy_param_offload() -> U16;
    fn phydm_fwoffload_ability_init(dm: *mut DmStruct, cap: U16);
    fn phydm_fwoffload_ability_clear(dm: *mut DmStruct, cap: U16);
    fn halrf_iqk_trigger(dm_void: *mut c_void, is_recovery: U8);
    fn rtw_sctx_init(sctx: *mut c_void, timeout_ms: i32);
}

#[cfg(config_fw_offload_param_init)]
unsafe extern "C" {
    fn rtw_rust_hal_dm_fw_param_init(adapter: *mut c_void) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_hal_update_iqk_fw_offload_cap(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let dm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if dm.is_null() {
            return;
        }
        let iqk_offload = rtw_rust_hal_dm_reg_iqk_fw_offload(adapter);
        let cap = rtw_rust_hal_dm_phydm_rf_iqk_offload();
        if iqk_offload != 0 {
            let sctx = rtw_rust_hal_dm_iqk_sctx_ptr(adapter);
            if !sctx.is_null() {
                rtw_sctx_init(sctx, 0);
            }
            phydm_fwoffload_ability_init(dm, cap);
        } else {
            phydm_fwoffload_ability_clear(dm, cap);
        }
        rtw_rust_hal_dm_log_iqk_fw_offload(iqk_offload);
        if rtw_rust_hal_dm_mi_linked(adapter) != 0 {
            rtw_rust_hal_dm_lps_leave_iqk_switch(adapter);
            halrf_iqk_trigger(dm.cast(), FALSE);
        }
    }
}

#[cfg(config_fw_offload_param_init)]
#[no_mangle]
pub extern "C" fn rtw_hal_update_param_init_fw_offload_cap(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let dm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if dm.is_null() {
            return;
        }
        let fw_param = rtw_rust_hal_dm_fw_param_init(adapter);
        let cap = rtw_rust_hal_dm_phydm_phy_param_offload();
        if fw_param != 0 {
            phydm_fwoffload_ability_init(dm, cap);
        } else {
            phydm_fwoffload_ability_clear(dm, cap);
        }
        rtw_rust_hal_dm_log_param_init_fw_offload(fw_param);
    }
}