// SPDX-License-Identifier: GPL-2.0
// W3-220: monitor / simple_config / xmit_block debug proc leaves (was core/rtw_debug_rest.c).

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
    fn rtw_rust_debug_proc_emit_monitor(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_monitor(
        adapter: *mut c_void,
        dev: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_simple_config(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_simple_config(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_xmit_block(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_xmit_block(
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

#[no_mangle]
pub extern "C" fn proc_get_monitor(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_monitor(m, adapter) };
    }
    0
}

#[no_mangle]
pub extern "C" fn proc_set_monitor(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_monitor(adapter, data, buffer, count) }
}

#[cfg(any(rtw_simple_config, host_rtw_debug_proc_monitor_test))]
#[no_mangle]
pub extern "C" fn proc_get_simple_config(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_simple_config(m, adapter) };
    }
    0
}

#[cfg(any(rtw_simple_config, host_rtw_debug_proc_monitor_test))]
#[no_mangle]
pub extern "C" fn proc_set_simple_config(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_simple_config(adapter, buffer, count) }
}

#[cfg(any(dbg_xmit_block, host_rtw_debug_proc_monitor_test))]
#[no_mangle]
pub extern "C" fn proc_get_xmit_block(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_xmit_block(m, adapter) };
    }
    0
}

#[cfg(any(dbg_xmit_block, host_rtw_debug_proc_monitor_test))]
#[no_mangle]
pub extern "C" fn proc_set_xmit_block(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_xmit_block(adapter, buffer, count) }
}
