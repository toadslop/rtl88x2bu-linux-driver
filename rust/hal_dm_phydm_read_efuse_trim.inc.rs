// W4-112 — `rtw_phydm_read_efuse` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn phydm_get_thermal_trim_offset(dm_void: *mut c_void);
    fn phydm_get_power_trim_offset(dm_void: *mut c_void);
}

#[no_mangle]
pub extern "C" fn rtw_phydm_read_efuse(adapter: *mut c_void) {
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_get_thermal_trim_offset(phydm.cast());
        phydm_get_power_trim_offset(phydm.cast());
    }
}