// SPDX-License-Identifier: GPL-2.0
// W3-230: proc IQK/LCK cal test handlers (was core/rtw_debug_rest.c).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

use core::ffi::{c_int, c_long, c_void};

const EFAULT: c_long = -14;

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
}

#[cfg(any(config_dbg_rf_cal, host_rtw_debug_proc_cal_test))]
extern "C" {
    fn rtw_rust_debug_proc_set_iqk(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_lck(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(any(config_dbg_rf_cal, host_rtw_debug_proc_cal_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_cal_test))]
#[no_mangle]
pub extern "C" fn proc_get_iqk_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let _adapter = unsafe { rtw_rust_debug_proc_adapter(m) };
    0
}

#[cfg(any(config_dbg_rf_cal, host_rtw_debug_proc_cal_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_cal_test))]
#[no_mangle]
pub extern "C" fn proc_set_iqk(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    if adapter.is_null() {
        return EFAULT;
    }
    unsafe { rtw_rust_debug_proc_set_iqk(adapter, buffer, count) }
}

#[cfg(any(config_dbg_rf_cal, host_rtw_debug_proc_cal_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_cal_test))]
#[no_mangle]
pub extern "C" fn proc_get_lck_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let _adapter = unsafe { rtw_rust_debug_proc_adapter(m) };
    0
}

#[cfg(any(config_dbg_rf_cal, host_rtw_debug_proc_cal_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_cal_test))]
#[no_mangle]
pub extern "C" fn proc_set_lck(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    if adapter.is_null() {
        return EFAULT;
    }
    unsafe { rtw_rust_debug_proc_set_lck(adapter, buffer, count) }
}
