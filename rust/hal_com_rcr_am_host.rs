// SPDX-License-Identifier: GPL-2.0
// Host staticlib: W4-137 RCR_AM hw_var delegate only (calls traced C add/clear).

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

type U8 = u8;
type U32 = u32;

const RCR_AM: U32 = 4;

extern "C" {
    fn rtw_hal_rcr_add(adapter: *mut c_void, add: U32) -> U8;
    fn rtw_hal_rcr_clear(adapter: *mut c_void, clear: U32) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_rust_hw_var_set_rcr_am(adapter: *mut c_void, enable: U8) {
    if enable != 0 {
        unsafe { rtw_hal_rcr_add(adapter, RCR_AM) };
    } else {
        unsafe { rtw_hal_rcr_clear(adapter, RCR_AM) };
    }
}