// W4-106 — `rtw_hal_iqk_test`, `rtw_hal_lck_test` under CONFIG_DBG_RF_CAL.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;
type U64 = u64;

const PS_DENY_IOCTL: U32 = 5;
const HALRF_CMNINFO_ABILITY: U32 = 0;
const HAL_RF_IQK: U64 = 1 << 1;
const HAL_RF_LCK: U64 = 1 << 2;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_ps_deny(adapter: *mut c_void, reason: U32);
    fn rtw_ps_deny_cancel(adapter: *mut c_void, reason: U32);
    fn LeaveAllPowerSaveModeDirect(adapter: *mut c_void);
    fn rtw_phydm_ability_backup(adapter: *mut c_void);
    fn rtw_phydm_func_disable_all(adapter: *mut c_void);
    fn rtw_phydm_ability_restore(adapter: *mut c_void);
    fn halrf_cmn_info_set(dm_void: *mut c_void, cmn_info: U32, value: U64);
    fn rtw_phydm_iqk_trigger_dbg(adapter: *mut c_void, recovery: U8, clear: U8, segment: U8);
    fn rtw_phydm_lck_trigger(adapter: *mut c_void);
}

#[no_mangle]
pub extern "C" fn rtw_hal_iqk_test(
    adapter: *mut c_void,
    recovery: U8,
    clear: U8,
    segment: U8,
) {
    unsafe {
        let p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);

        rtw_ps_deny(adapter, PS_DENY_IOCTL);
        LeaveAllPowerSaveModeDirect(adapter);
        rtw_phydm_ability_backup(adapter);
        rtw_phydm_func_disable_all(adapter);
        halrf_cmn_info_set(p_dm_odm.cast(), HALRF_CMNINFO_ABILITY, HAL_RF_IQK);
        rtw_phydm_iqk_trigger_dbg(adapter, recovery, clear, segment);
        rtw_phydm_ability_restore(adapter);
        rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_lck_test(adapter: *mut c_void) {
    unsafe {
        let p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);

        rtw_ps_deny(adapter, PS_DENY_IOCTL);
        LeaveAllPowerSaveModeDirect(adapter);
        rtw_phydm_ability_backup(adapter);
        rtw_phydm_func_disable_all(adapter);
        halrf_cmn_info_set(p_dm_odm.cast(), HALRF_CMNINFO_ABILITY, HAL_RF_LCK);
        rtw_phydm_lck_trigger(adapter);
        rtw_phydm_ability_restore(adapter);
        rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
    }
}