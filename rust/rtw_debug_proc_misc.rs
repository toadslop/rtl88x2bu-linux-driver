// SPDX-License-Identifier: GPL-2.0
// proc malloc cnt + best channel (W3-214). Logic in core/rtw_debug.c.

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
    fn rtw_rust_debug_proc_emit_malloc_cnt(sel: *mut c_void);
    fn rtw_rust_debug_proc_emit_best_channel(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_best_channel(
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

#[cfg(any(all(dbg_memory_leak, config_proc_debug), host_rtw_debug_proc_misc_test))]
#[no_mangle]
pub extern "C" fn proc_get_malloc_cnt(m: *mut c_void, _v: *mut c_void) -> c_int {
    unsafe { rtw_rust_debug_proc_emit_malloc_cnt(m) };
    0
}

#[cfg(any(
    all(config_find_best_channel, config_proc_debug),
    host_rtw_debug_proc_misc_test
))]
#[no_mangle]
pub extern "C" fn proc_get_best_channel(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_best_channel(m, adapter) };
    }
    0
}

#[cfg(any(
    all(config_find_best_channel, config_proc_debug),
    host_rtw_debug_proc_misc_test
))]
#[no_mangle]
pub extern "C" fn proc_set_best_channel(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_best_channel(adapter, buffer, count) }
}
