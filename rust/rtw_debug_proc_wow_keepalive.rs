// SPDX-License-Identifier: GPL-2.0
// W3-235: WoWLAN keep-alive pattern dump proc (was core/rtw_debug_rest.c).

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

use core::ffi::{c_int, c_void};

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
}

#[cfg(any(
    all(config_wowlan, config_wow_keep_alive_pattern, config_proc_debug),
    host_rtw_debug_proc_wow_keepalive_test
))]
extern "C" {
    fn rtw_rust_debug_proc_emit_wow_keep_alive_info(sel: *mut c_void, adapter: *mut c_void);
}

fn adapter_from_seq(m: *mut c_void) -> *mut c_void {
    if m.is_null() {
        return core::ptr::null_mut();
    }
    unsafe { rtw_rust_debug_proc_adapter(m) }
}

#[cfg(any(
    all(config_wowlan, config_wow_keep_alive_pattern, config_proc_debug),
    host_rtw_debug_proc_wow_keepalive_test
))]
#[no_mangle]
pub extern "C" fn proc_dump_wow_keep_alive_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_wow_keep_alive_info(m, adapter) };
    }
    0
}
