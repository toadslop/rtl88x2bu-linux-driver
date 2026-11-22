// SPDX-License-Identifier: GPL-2.0
// W4-129 — TSF iface enable/disable (hal/hal_com_rest.c).

use core::ffi::{c_ulong, c_void};

type U8 = u8;

extern "C" {
    fn rtw_rust_hal_com_tsf_set_pause_stime(adapter: *mut c_void, stime: c_ulong);
    fn rtw_rust_hal_com_tsf_set_update_required(adapter: *mut c_void, v: U8);
    fn rtw_rust_hal_com_tsf_set_en_hw_update(adapter: *mut c_void, v: U8);
    #[cfg(not(config_mi_with_mbssid_cam))]
    fn rtw_rust_hal_com_tsf_hal_set_tsf_update(adapter: *mut c_void, en: U8);
}

#[no_mangle]
pub extern "C" fn rtw_iface_enable_tsf_update(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        rtw_rust_hal_com_tsf_set_pause_stime(adapter, 0);
        rtw_rust_hal_com_tsf_set_update_required(adapter, 1);
    }
    #[cfg(not(config_mi_with_mbssid_cam))]
    unsafe {
        rtw_rust_hal_com_tsf_hal_set_tsf_update(adapter, 1);
    }
}

#[no_mangle]
pub extern "C" fn rtw_iface_disable_tsf_update(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        rtw_rust_hal_com_tsf_set_update_required(adapter, 0);
        rtw_rust_hal_com_tsf_set_pause_stime(adapter, 0);
        rtw_rust_hal_com_tsf_set_en_hw_update(adapter, 0);
    }
    #[cfg(not(config_mi_with_mbssid_cam))]
    unsafe {
        rtw_rust_hal_com_tsf_hal_set_tsf_update(adapter, 0);
    }
}

include!("hal_com_tsf_periodic.inc.rs");