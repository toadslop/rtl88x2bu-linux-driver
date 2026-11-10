// SPDX-License-Identifier: GPL-2.0
// proc rf4ce_state + sreset get/set (W3-216). Logic in core/rtw_debug.c.

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
    fn rtw_rust_debug_proc_emit_rf4ce_state(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_rf4ce_state(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_sreset(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_sreset(
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

#[cfg(any(
    all(config_rf4ce_coexist, config_proc_debug),
    host_rtw_debug_proc_sreset_test
))]
#[no_mangle]
pub extern "C" fn proc_get_rf4ce_state(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_rf4ce_state(m, adapter) };
    }
    0
}

#[cfg(any(
    all(config_rf4ce_coexist, config_proc_debug),
    host_rtw_debug_proc_sreset_test
))]
#[no_mangle]
pub extern "C" fn proc_set_rf4ce_state(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_rf4ce_state(adapter, buffer, count) }
}

#[cfg(any(
    all(dbg_config_error_detect, config_proc_debug),
    host_rtw_debug_proc_sreset_test
))]
#[no_mangle]
pub extern "C" fn proc_get_sreset(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_sreset(m, adapter) };
    }
    0
}

#[cfg(any(
    all(dbg_config_error_detect, config_proc_debug),
    host_rtw_debug_proc_sreset_test
))]
#[no_mangle]
pub extern "C" fn proc_set_sreset(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_sreset(adapter, buffer, count) }
}
