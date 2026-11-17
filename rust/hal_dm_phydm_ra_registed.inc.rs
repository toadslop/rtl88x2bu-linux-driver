// W4-107 — `rtw_phydm_ra_registed` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;
type StaInfo = u8;
type U8 = u8;
type S8 = i8;

const RTW_DBGDUMP: usize = 0;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_dm_dvobj_to_macidctl(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_dm_macidctl_num(macid_ctl: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_sta_mac_id(psta: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_sta_rssi(sta: *mut StaInfo) -> S8;
    fn rtw_rust_hal_dm_err_ra_null_sta(adapter: *mut c_void);
    fn phydm_ra_registed(dm_void: *mut c_void, macid: U8, rssi_from_assoc: U8);
    fn dump_sta_info(sel: *mut c_void, psta: *mut c_void);
    fn rtw_rust_hal_dm_warn_on(cond: i32);
}

#[no_mangle]
pub extern "C" fn rtw_phydm_ra_registed(adapter: *mut c_void, psta: *mut c_void) {
    if psta.is_null() {
        // SAFETY: adapter may be null when sta is null; kernel logs adapter context.
        unsafe {
            rtw_rust_hal_dm_err_ra_null_sta(adapter);
            rtw_rust_hal_dm_warn_on(1);
        }
        return;
    }

    unsafe {
        let dvobj = rtw_rust_hal_dm_adapter_to_dvobj(adapter);
        let macid_ctl = rtw_rust_hal_dm_dvobj_to_macidctl(dvobj);
        let mac_id = rtw_rust_hal_dm_sta_mac_id(psta);
        if mac_id >= rtw_rust_hal_dm_macidctl_num(macid_ctl) {
            return;
        }
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        let rssi = rtw_rust_hal_dm_sta_rssi(psta.cast()) as U8;
        phydm_ra_registed(phydm.cast(), mac_id, rssi);
        dump_sta_info(RTW_DBGDUMP as *mut c_void, psta);
    }
}