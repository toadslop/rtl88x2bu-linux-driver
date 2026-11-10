// SPDX-License-Identifier: GPL-2.0
// proc fw_offload get/set (W3-228). Logic in core/rtw_debug.c.

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
    fn rtw_rust_debug_proc_emit_fw_offload(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_fw_offload(
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
    config_rtw_debug,
    config_proc_debug,
    host_rtw_debug_proc_fw_offload_test
))]
#[no_mangle]
pub extern "C" fn proc_get_fw_offload(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_fw_offload(m, adapter) };
    }
    0
}

#[cfg(any(
    config_rtw_debug,
    config_proc_debug,
    host_rtw_debug_proc_fw_offload_test
))]
#[no_mangle]
pub extern "C" fn proc_set_fw_offload(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_fw_offload(adapter, buffer, count) }
}
