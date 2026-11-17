// W4-105 — `rtw_phydm_iqk_trigger_dbg` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn halrf_segment_iqk_trigger(dm_void: *mut c_void, clear: U8, segment: U8);
    fn halrf_iqk_trigger(dm_void: *mut c_void, is_recovery: U8);
}

#[no_mangle]
pub extern "C" fn rtw_phydm_iqk_trigger_dbg(
    adapter: *mut c_void,
    _recovery: U8,
    clear: U8,
    segment: U8,
) {
    unsafe {
        let p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        #[cfg(any(
            CONFIG_RTL8822B,
            CONFIG_RTL8821C,
            CONFIG_RTL8814B,
            CONFIG_RTL8822C,
        ))]
        {
            halrf_segment_iqk_trigger(p_dm_odm.cast(), clear, segment);
        }
        #[cfg(not(any(
            CONFIG_RTL8822B,
            CONFIG_RTL8821C,
            CONFIG_RTL8814B,
            CONFIG_RTL8822C,
        )))]
        {
            halrf_iqk_trigger(p_dm_odm.cast(), _recovery);
        }
    }
}