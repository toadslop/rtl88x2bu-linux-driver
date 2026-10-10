// SPDX-License-Identifier: GPL-2.0
//! W3-137: WiFi-only BTC coexistence stubs — Rust port of `core/rtw_btcoex_wifionly.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    unreachable_pub
)]

use core::ffi::c_void;

type Adapter = c_void;

extern "C" {
    fn hal_btcoex_wifionly_switchband_notify(padapter: *mut Adapter);
    fn hal_btcoex_wifionly_scan_notify(padapter: *mut Adapter);
    fn hal_btcoex_wifionly_connect_notify(padapter: *mut Adapter);
    fn hal_btcoex_wifionly_hw_config(padapter: *mut Adapter);
    fn hal_btcoex_wifionly_initlizevariables(padapter: *mut Adapter);
    fn hal_btcoex_wifionly_AntInfoSetting(padapter: *mut Adapter);
}

#[no_mangle]
pub extern "C" fn rtw_btcoex_wifionly_switchband_notify(padapter: *mut Adapter) {
    unsafe {
        hal_btcoex_wifionly_switchband_notify(padapter);
    }
}

#[no_mangle]
pub extern "C" fn rtw_btcoex_wifionly_scan_notify(padapter: *mut Adapter) {
    unsafe {
        hal_btcoex_wifionly_scan_notify(padapter);
    }
}

#[no_mangle]
pub extern "C" fn rtw_btcoex_wifionly_connect_notify(padapter: *mut Adapter) {
    unsafe {
        hal_btcoex_wifionly_connect_notify(padapter);
    }
}

#[no_mangle]
pub extern "C" fn rtw_btcoex_wifionly_hw_config(padapter: *mut Adapter) {
    unsafe {
        hal_btcoex_wifionly_hw_config(padapter);
    }
}

#[no_mangle]
pub extern "C" fn rtw_btcoex_wifionly_initialize(padapter: *mut Adapter) {
    unsafe {
        hal_btcoex_wifionly_initlizevariables(padapter);
    }
}

#[no_mangle]
pub extern "C" fn rtw_btcoex_wifionly_AntInfoSetting(padapter: *mut Adapter) {
    unsafe {
        hal_btcoex_wifionly_AntInfoSetting(padapter);
    }
}
