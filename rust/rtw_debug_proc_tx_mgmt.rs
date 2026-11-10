// SPDX-License-Identifier: GPL-2.0
// W3-221: 11w TX mgmt inject test proc handlers (was core/rtw_debug_rest.c).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_imports,
    unused_unsafe
)]

use core::ffi::{c_int, c_long, c_void};

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_emit_tx_sa_query(sel: *mut c_void);
    fn rtw_rust_debug_proc_set_tx_sa_query(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_tx_deauth(sel: *mut c_void);
    fn rtw_rust_debug_proc_set_tx_deauth(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_tx_auth(sel: *mut c_void);
    fn rtw_rust_debug_proc_set_tx_auth(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

fn adapter_from_seq(m: *mut c_void) -> *mut c_void {
    if m.is_null() {
        return core::ptr::null_mut();
    }
    unsafe { rtw_rust_debug_proc_adapter(m) }
}

#[cfg(any(config_proc_debug, host_rtw_debug_proc_tx_mgmt_test))]
#[no_mangle]
pub extern "C" fn proc_get_tx_sa_query(m: *mut c_void, _v: *mut c_void) -> c_int {
    unsafe { rtw_rust_debug_proc_emit_tx_sa_query(m) };
    0
}

#[cfg(any(config_proc_debug, host_rtw_debug_proc_tx_mgmt_test))]
#[no_mangle]
pub extern "C" fn proc_set_tx_sa_query(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_tx_sa_query(adapter, buffer, count) }
}

#[cfg(any(config_proc_debug, host_rtw_debug_proc_tx_mgmt_test))]
#[no_mangle]
pub extern "C" fn proc_get_tx_deauth(m: *mut c_void, _v: *mut c_void) -> c_int {
    unsafe { rtw_rust_debug_proc_emit_tx_deauth(m) };
    0
}

#[cfg(any(config_proc_debug, host_rtw_debug_proc_tx_mgmt_test))]
#[no_mangle]
pub extern "C" fn proc_set_tx_deauth(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_tx_deauth(adapter, buffer, count) }
}

#[cfg(any(config_proc_debug, host_rtw_debug_proc_tx_mgmt_test))]
#[no_mangle]
pub extern "C" fn proc_get_tx_auth(m: *mut c_void, _v: *mut c_void) -> c_int {
    unsafe { rtw_rust_debug_proc_emit_tx_auth(m) };
    0
}

#[cfg(any(config_proc_debug, host_rtw_debug_proc_tx_mgmt_test))]
#[no_mangle]
pub extern "C" fn proc_set_tx_auth(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_tx_auth(adapter, buffer, count) }
}
