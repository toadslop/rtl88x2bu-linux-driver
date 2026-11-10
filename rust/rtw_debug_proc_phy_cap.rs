// SPDX-License-Identifier: GPL-2.0
// proc stbc/rx_stbc/ldpc/txbf registry get/set (W3-211). Logic in core/rtw_debug.c.

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
    fn rtw_rust_debug_proc_emit_stbc_cap(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_stbc_cap(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_rx_stbc(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_rx_stbc(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_ldpc_cap(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_ldpc_cap(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    #[cfg(config_beamforming)]
    fn rtw_rust_debug_proc_emit_txbf_cap(sel: *mut c_void, adapter: *mut c_void);
    #[cfg(config_beamforming)]
    fn rtw_rust_debug_proc_set_txbf_cap(
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

macro_rules! proc_get {
    ($name:ident, $emit:ident) => {
        #[cfg(any(config_proc_debug, host_rtw_debug_proc_phy_cap_registry_test))]
        #[no_mangle]
        pub extern "C" fn $name(m: *mut c_void, _v: *mut c_void) -> c_int {
            let adapter = adapter_from_seq(m);
            if !adapter.is_null() {
                unsafe { $emit(m, adapter) };
            }
            0
        }
    };
}

macro_rules! proc_set {
    ($name:ident, $set:ident) => {
        #[cfg(any(config_proc_debug, host_rtw_debug_proc_phy_cap_registry_test))]
        #[no_mangle]
        pub extern "C" fn $name(
            _file: *mut c_void,
            buffer: *const c_void,
            count: usize,
            _pos: *mut c_long,
            data: *mut c_void,
        ) -> c_long {
            let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
            unsafe { $set(adapter, buffer, count) }
        }
    };
}

proc_get!(proc_get_stbc_cap, rtw_rust_debug_proc_emit_stbc_cap);
proc_set!(proc_set_stbc_cap, rtw_rust_debug_proc_set_stbc_cap);
proc_get!(proc_get_rx_stbc, rtw_rust_debug_proc_emit_rx_stbc);
proc_set!(proc_set_rx_stbc, rtw_rust_debug_proc_set_rx_stbc);
proc_get!(proc_get_ldpc_cap, rtw_rust_debug_proc_emit_ldpc_cap);
proc_set!(proc_set_ldpc_cap, rtw_rust_debug_proc_set_ldpc_cap);

#[cfg(config_beamforming)]
proc_get!(proc_get_txbf_cap, rtw_rust_debug_proc_emit_txbf_cap);
#[cfg(config_beamforming)]
proc_set!(proc_set_txbf_cap, rtw_rust_debug_proc_set_txbf_cap);
