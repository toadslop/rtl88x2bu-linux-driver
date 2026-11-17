// W4-101 — `rtw_hal_phydm_cal_trigger`.

use core::ffi::c_void;

type DmStruct = u8;

// `PS_DENY_IOCTL` in include/rtw_pwrctrl.h (enum ordinal).
const PS_DENY_IOCTL: u32 = 5;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_ps_deny(adapter: *mut c_void, reason: u32);
    fn rtw_ps_deny_cancel(adapter: *mut c_void, reason: u32);
    fn LeaveAllPowerSaveModeDirect(adapter: *mut c_void);
    fn rtw_phydm_iqk_trigger_all(adapter: *mut c_void);
}

#[no_mangle]
pub extern "C" fn rtw_hal_phydm_cal_trigger(adapter: *mut c_void) {
    unsafe {
        let _p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);

        rtw_ps_deny(adapter, PS_DENY_IOCTL);
        LeaveAllPowerSaveModeDirect(adapter);
        rtw_phydm_iqk_trigger_all(adapter);
        rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
    }
}