// SPDX-License-Identifier: GPL-2.0
//! W3-144 `dump_adapters_status` from `core/rtw_debug_rest.c`.

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

use core::ffi::c_void;

extern "C" {
    fn dump_mi_status(sel: *mut c_void, dvobj: *mut c_void);
    fn rtw_rust_debug_adapters_dump_body(sel: *mut c_void, dvobj: *mut c_void);
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn dump_adapters_status(sel: *mut c_void, dvobj: *mut c_void) {
    if sel.is_null() || dvobj.is_null() {
        return;
    }
    unsafe {
        dump_mi_status(sel, dvobj);
        rtw_rust_debug_adapters_dump_body(sel, dvobj);
    }
}
