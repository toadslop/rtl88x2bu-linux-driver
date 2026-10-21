// SPDX-License-Identifier: GPL-2.0
// reqtxrpt hal op + MI MAC addr push (W4-51).

use core::ffi::c_void;

type U8 = u8;

extern "C" {
    fn rtw_rust_hal_com_call_reqtxrpt(adapter: *mut c_void, macid: U8);
    fn rtw_rust_hal_com_hw_var_mac_addr() -> i32;
    fn rtw_rust_hal_com_set_hwreg(adapter: *mut c_void, variable: i32, val: *mut U8);
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_dvobj_iface_nums(dvobj: *mut c_void) -> U8;
    fn rtw_rust_hal_com_dvobj_adapter_at(dvobj: *mut c_void, idx: i32) -> *mut c_void;
    fn rtw_rust_hal_com_adapter_mac_addr(adapter: *mut c_void) -> *mut U8;
    #[cfg(config_mi_with_mbssid_cam)]
    fn rtw_mi_set_mbid_cam(adapter: *mut c_void);
}

#[no_mangle]
pub extern "C" fn rtw_hal_reqtxrpt(adapter: *mut c_void, macid: U8) {
    unsafe { rtw_rust_hal_com_call_reqtxrpt(adapter, macid) };
}

#[no_mangle]
pub extern "C" fn rtw_mi_set_mac_addr(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    #[cfg(config_mi_with_mbssid_cam)]
    unsafe {
        rtw_mi_set_mbid_cam(adapter);
    }
    #[cfg(not(config_mi_with_mbssid_cam))]
    {
        let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
        if dvobj.is_null() {
            return;
        }
        let n = unsafe { rtw_rust_hal_com_dvobj_iface_nums(dvobj) };
        let hw_var = unsafe { rtw_rust_hal_com_hw_var_mac_addr() };
        for i in 0..n {
            let iface = unsafe { rtw_rust_hal_com_dvobj_adapter_at(dvobj, i as i32) };
            if !iface.is_null() {
                let mac = unsafe { rtw_rust_hal_com_adapter_mac_addr(iface) };
                unsafe { rtw_rust_hal_com_set_hwreg(iface, hw_var, mac) };
            }
        }
    }
}

include!("hal_com_sec_cam_rw.rs");
