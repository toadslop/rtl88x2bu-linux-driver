// SPDX-License-Identifier: GPL-2.0
// W3-222: MCC registry debug proc setters (was core/rtw_debug_rest.c).

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

use core::ffi::{c_long, c_void};

extern "C" {
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_set_mcc_enable(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_mcc_duration(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_mcc_single_tx_criteria(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(config_mcc_phydm_offload)]
extern "C" {
    fn rtw_rust_debug_proc_set_mcc_phydm_offload_enable(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(any(config_mcc_mode, host_rtw_debug_proc_mcc_test))]
#[no_mangle]
pub extern "C" fn proc_set_mcc_enable(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_mcc_enable(adapter, buffer, count) }
}

#[cfg(any(config_mcc_mode, host_rtw_debug_proc_mcc_test))]
#[no_mangle]
pub extern "C" fn proc_set_mcc_duration(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_mcc_duration(adapter, buffer, count) }
}

#[cfg(all(
    any(config_mcc_mode, host_rtw_debug_proc_mcc_test),
    config_mcc_phydm_offload
))]
#[no_mangle]
pub extern "C" fn proc_set_mcc_phydm_offload_enable(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_mcc_phydm_offload_enable(adapter, buffer, count) }
}

#[cfg(any(config_mcc_mode, host_rtw_debug_proc_mcc_test))]
#[no_mangle]
pub extern "C" fn proc_set_mcc_single_tx_criteria(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_mcc_single_tx_criteria(adapter, buffer, count) }
}
