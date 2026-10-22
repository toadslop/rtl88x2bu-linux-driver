// SPDX-License-Identifier: GPL-2.0
//! W3-145..150 proc seq_file handlers from `core/rtw_debug_rest.c`.

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

type U8 = u8;
type U16 = u16;
type U32 = u32;

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_emit_fwstate(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_mlmext_state(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_qos_option(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_ht_option(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_rf_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_sec_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_defs_param(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_defs_param(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_write_reg(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_read_reg(
        buffer: *const c_void,
        count: usize,
        addr_out: *mut U32,
        len_out: *mut U32,
        parsed_ok: *mut c_int,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_read_reg_uninit(sel: *mut c_void);
    fn rtw_rust_debug_proc_emit_read_reg(sel: *mut c_void, addr: U32, len: U32, val: U32);
    fn rtw_rust_debug_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_debug_read16(adapter: *mut c_void, addr: U32) -> U16;
    fn rtw_rust_debug_read32(adapter: *mut c_void, addr: U32) -> U32;
    fn rtw_rust_debug_proc_emit_rx_stat(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_tx_stat(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_scan_param(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_scan_param(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_emit_scan_abort(sel: *mut c_void, adapter: *mut c_void);
}

#[cfg(config_layer2_roaming)]
extern "C" {
    fn rtw_rust_debug_proc_emit_roam_flags(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_emit_roam_param(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_roam_flags(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_roam_param(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_roam_tgt_addr(
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
export_proc!(proc_get_defs_param, rtw_rust_debug_proc_emit_defs_param);
export_proc!(proc_get_rx_stat, rtw_rust_debug_proc_emit_rx_stat);
export_proc!(proc_get_tx_stat, rtw_rust_debug_proc_emit_tx_stat);
export_proc!(proc_get_scan_param, rtw_rust_debug_proc_emit_scan_param);
export_proc!(proc_get_scan_abort, rtw_rust_debug_proc_emit_scan_abort);

static mut PROC_GET_READ_ADDR: U32 = 0xeeeeeeee;
static mut PROC_GET_READ_LEN: U32 = 0x4;

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_read_reg(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if adapter.is_null() {
        return 0;
    }
    let addr = unsafe { PROC_GET_READ_ADDR };
    if addr == 0xeeeeeeee {
        unsafe { rtw_rust_debug_proc_emit_read_reg_uninit(m) };
        return 0;
    }
    let len = unsafe { PROC_GET_READ_LEN };
    let val = unsafe {
        match len {
            1 => rtw_rust_debug_read8(adapter, addr) as U32,
            2 => rtw_rust_debug_read16(adapter, addr) as U32,
            4 => rtw_rust_debug_read32(adapter, addr),
            _ => {
                rtw_rust_debug_proc_emit_read_reg(m, addr, len, 0);
                return 0;
            }
        }
    };
    unsafe { rtw_rust_debug_proc_emit_read_reg(m, addr, len, val) };
    0
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_scan_param(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_scan_param(adapter, buffer, count) }
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_defs_param(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_defs_param(adapter, buffer, count) }
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_write_reg(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_write_reg(adapter, buffer, count) }
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_read_reg(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    _data: *mut c_void,
) -> c_long {
    let mut addr = 0u32;
    let mut len = 0u32;
    let mut parsed_ok: c_int = 0;
    let ret = unsafe {
        rtw_rust_debug_proc_set_read_reg(buffer, count, &mut addr, &mut len, &mut parsed_ok)
    };
    if ret >= 0 && parsed_ok != 0 {
        unsafe {
            PROC_GET_READ_ADDR = addr;
            PROC_GET_READ_LEN = len;
        }
    }
    ret
}

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

#[cfg(all(config_layer2_roaming, any(config_rtw_debug, config_proc_debug)))]
#[no_mangle]
pub extern "C" fn proc_set_roam_param(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_roam_param(adapter, buffer, count) }
}

#[cfg(all(config_layer2_roaming, any(config_rtw_debug, config_proc_debug)))]
#[no_mangle]
pub extern "C" fn proc_set_roam_tgt_addr(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_roam_tgt_addr(adapter, buffer, count) }
}
