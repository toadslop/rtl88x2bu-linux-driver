// SPDX-License-Identifier: GPL-2.0
// W3-217: WoWLAN registry/GPIO debug proc leaf handlers (was core/rtw_debug_rest.c).
// W3-218: WoWLAN wake-pattern `/proc` leaf (`wow_pattern_info` get/set).

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
    fn rtw_rust_debug_proc_emit_wow_enable(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_wow_enable(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_wakeup_event(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_wakeup_event(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_wakeup_reason(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_pattern_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_pattern_info(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(config_gpio_wakeup)]
extern "C" {
    fn rtw_rust_debug_proc_emit_wowlan_gpio_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_wowlan_gpio_info(
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

#[cfg(config_wowlan)]
#[no_mangle]
pub extern "C" fn proc_get_wow_enable(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_wow_enable(m, adapter) };
    }
    0
}

#[cfg(config_wowlan)]
#[no_mangle]
pub extern "C" fn proc_set_wow_enable(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_wow_enable(adapter, buffer, count) }
}

#[cfg(config_wowlan)]
#[no_mangle]
pub extern "C" fn proc_get_wakeup_event(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_wakeup_event(m, adapter) };
    }
    0
}

#[cfg(config_wowlan)]
#[no_mangle]
pub extern "C" fn proc_set_wakeup_event(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_wakeup_event(adapter, buffer, count) }
}

#[cfg(config_wowlan)]
#[no_mangle]
pub extern "C" fn proc_get_wakeup_reason(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_wakeup_reason(m, adapter) };
    }
    0
}

#[cfg(config_wowlan)]
#[no_mangle]
pub extern "C" fn proc_get_pattern_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_pattern_info(m, adapter) };
    }
    0
}

#[cfg(config_wowlan)]
#[no_mangle]
pub extern "C" fn proc_set_pattern_info(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_pattern_info(adapter, buffer, count) }
}

#[cfg(config_gpio_wakeup)]
#[no_mangle]
pub extern "C" fn proc_get_wowlan_gpio_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_wowlan_gpio_info(m, adapter) };
    }
    0
}

#[cfg(config_gpio_wakeup)]
#[no_mangle]
pub extern "C" fn proc_set_wowlan_gpio_info(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_wowlan_gpio_info(adapter, buffer, count) }
}
