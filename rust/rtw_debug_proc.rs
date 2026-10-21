// SPDX-License-Identifier: GPL-2.0
//! W3-145/W3-146/W3-147 proc seq_file leaf handlers from `core/rtw_debug_rest.c`.

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
    fn rtw_rust_debug_proc_emit_fwstate(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_mlmext_state(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_qos_option(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_ht_option(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_rf_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_sec_info(sel: *mut c_void, adapter: *mut c_void);
}

#[cfg(config_layer2_roaming)]
extern "C" {
    fn rtw_rust_debug_proc_emit_roam_flags(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_roam_param(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_set_roam_flags(
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

macro_rules! export_proc {
    ($name:ident, $emit:ident) => {
        #[cfg(any(config_rtw_debug, config_proc_debug))]
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

export_proc!(proc_get_fwstate, rtw_rust_debug_proc_emit_fwstate);
export_proc!(proc_get_mlmext_state, rtw_rust_debug_proc_emit_mlmext_state);
export_proc!(proc_get_qos_option, rtw_rust_debug_proc_emit_qos_option);
export_proc!(proc_get_ht_option, rtw_rust_debug_proc_emit_ht_option);
export_proc!(proc_get_rf_info, rtw_rust_debug_proc_emit_rf_info);
export_proc!(proc_get_sec_info, rtw_rust_debug_proc_emit_sec_info);

#[cfg(config_layer2_roaming)]
export_proc!(proc_get_roam_flags, rtw_rust_debug_proc_emit_roam_flags);
#[cfg(config_layer2_roaming)]
export_proc!(proc_get_roam_param, rtw_rust_debug_proc_emit_roam_param);

#[cfg(all(config_layer2_roaming, any(config_rtw_debug, config_proc_debug)))]
#[no_mangle]
pub extern "C" fn proc_set_roam_flags(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_roam_flags(adapter, buffer, count) }
}
