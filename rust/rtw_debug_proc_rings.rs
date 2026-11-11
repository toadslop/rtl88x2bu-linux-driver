// SPDX-License-Identifier: GPL-2.0
// W3-233 PR1: TRX ring debug proc getters rx/tx (was core/rtw_debug_rest.c).

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

const EFAULT: c_long = -14;

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_rings_test))]
extern "C" {
    fn rtw_rust_debug_proc_emit_rx_ring(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_tx_ring(sel: *mut c_void, adapter: *mut c_void);
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_rings_test))]
#[cfg(any(dbg_txbd_desc_dump, host_rtw_debug_proc_rings_test))]
extern "C" {
    fn rtw_rust_debug_proc_emit_tx_ring_ext(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_tx_ring_ext(
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

#[cfg(any(config_pci_hci, host_rtw_debug_proc_rings_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_rings_test))]
#[no_mangle]
pub extern "C" fn proc_get_rx_ring(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_rx_ring(m, adapter) };
    }
    0
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_rings_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_rings_test))]
#[no_mangle]
pub extern "C" fn proc_get_tx_ring(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_tx_ring(m, adapter) };
    }
    0
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_rings_test))]
#[cfg(any(dbg_txbd_desc_dump, host_rtw_debug_proc_rings_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_rings_test))]
#[no_mangle]
pub extern "C" fn proc_get_tx_ring_ext(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_tx_ring_ext(m, adapter) };
    }
    0
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_rings_test))]
#[cfg(any(dbg_txbd_desc_dump, host_rtw_debug_proc_rings_test))]
#[cfg(any(config_proc_debug, host_rtw_debug_proc_rings_test))]
#[no_mangle]
pub extern "C" fn proc_set_tx_ring_ext(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    if count < 1 {
        return EFAULT;
    }
    if adapter.is_null() {
        return EFAULT;
    }
    unsafe { rtw_rust_debug_proc_set_tx_ring_ext(adapter, buffer, count) }
}
