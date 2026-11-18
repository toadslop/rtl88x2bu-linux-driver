// W4-117 — `rtw_hal_update_sta_ra_info`.

use core::ffi::c_void;

type Adapter = *mut c_void;
type StaInfo = *mut c_void;

unsafe extern "C" {
    fn rtw_hal_update_sta_mimo_type(adapter: Adapter, psta: StaInfo);
    fn rtw_hal_update_sta_smps_cap(adapter: Adapter, psta: StaInfo);
    fn rtw_hal_update_sta_rate_mask(adapter: Adapter, psta: StaInfo);
}

#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_ra_info(adapter: Adapter, psta: StaInfo) {
    unsafe {
        rtw_hal_update_sta_mimo_type(adapter, psta);
        rtw_hal_update_sta_smps_cap(adapter, psta);
        rtw_hal_update_sta_rate_mask(adapter, psta);
    }
}