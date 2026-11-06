// SPDX-License-Identifier: GPL-2.0
// W3-219: proc ps_info / wmmps_info / new_bcn_max — logic from `core/rtw_debug_rest.c`.
// Emit/set helpers live in `core/rtw_debug.c` (`rtw_rust_debug_proc_*`).

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

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
}

#[cfg(config_power_saving)]
extern "C" {
    fn rtw_rust_debug_proc_emit_ps_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_ps_info(adapter: *mut c_void, buffer: *const c_void, count: usize)
        -> c_long;
}

#[cfg(all(config_power_saving, config_wmmps_sta))]
extern "C" {
    fn rtw_rust_debug_proc_emit_wmmps_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_wmmps_info(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(config_bcn_cnt_confirm_hdl)]
extern "C" {
    fn rtw_rust_debug_proc_emit_new_bcn_max(sel: *mut c_void);
    fn rtw_rust_debug_proc_set_new_bcn_max(buffer: *const c_void, count: usize) -> c_long;
}

fn adapter_from_seq(m: *mut c_void) -> *mut c_void {
    if m.is_null() {
        return core::ptr::null_mut();
    }
    unsafe { rtw_rust_debug_proc_adapter(m) }
}

#[cfg(config_power_saving)]
#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_ps_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_ps_info(m, adapter) };
    }
    0
}

#[cfg(config_power_saving)]
#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_ps_info(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_ps_info(adapter, buffer, count) }
}

#[cfg(all(config_power_saving, config_wmmps_sta))]
#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_wmmps_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_wmmps_info(m, adapter) };
    }
    0
}

#[cfg(all(config_power_saving, config_wmmps_sta))]
#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_wmmps_info(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_wmmps_info(adapter, buffer, count) }
}

#[cfg(config_bcn_cnt_confirm_hdl)]
#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_new_bcn_max(m: *mut c_void, _v: *mut c_void) -> c_int {
    unsafe { rtw_rust_debug_proc_emit_new_bcn_max(m) };
    0
}

#[cfg(config_bcn_cnt_confirm_hdl)]
#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_new_bcn_max(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    _data: *mut c_void,
) -> c_long {
    unsafe { rtw_rust_debug_proc_set_new_bcn_max(buffer, count) }
}
