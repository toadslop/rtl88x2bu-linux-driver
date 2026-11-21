// SPDX-License-Identifier: GPL-2.0
// RCR check/add/clear leaf helpers (W4-127).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const SUCCESS: U8 = 1;

extern "C" {
    fn rtw_rust_hal_com_hw_var_rcr() -> i32;
    fn rtw_rust_hal_com_receive_config(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_com_get_hwreg(adapter: *mut c_void, variable: U32, val: *mut U8);
    fn rtw_rust_hal_com_set_hwreg_u8(adapter: *mut c_void, variable: U32, val: *mut U8) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_check(adapter: *mut c_void, check_bit: U32) -> U8 {
    let rcr = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    if (rcr & check_bit) == check_bit {
        1
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_add(adapter: *mut c_void, add: U32) -> U8 {
    let cached = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    let mut rcr = cached;
    let hw_var = unsafe { rtw_rust_hal_com_hw_var_rcr() as U32 };
    unsafe { rtw_rust_hal_com_get_hwreg(adapter, hw_var, &mut rcr as *mut U32 as *mut U8) };
    rcr |= add;
    if rcr != cached {
        let mut bytes = rcr.to_ne_bytes();
        return unsafe { rtw_rust_hal_com_set_hwreg_u8(adapter, hw_var, bytes.as_mut_ptr()) };
    }
    SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_clear(adapter: *mut c_void, clear: U32) -> U8 {
    let cached = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    let mut rcr = cached;
    let hw_var = unsafe { rtw_rust_hal_com_hw_var_rcr() as U32 };
    unsafe { rtw_rust_hal_com_get_hwreg(adapter, hw_var, &mut rcr as *mut U32 as *mut U8) };
    rcr &= !clear;
    if rcr != cached {
        let mut bytes = rcr.to_ne_bytes();
        return unsafe { rtw_rust_hal_com_set_hwreg_u8(adapter, hw_var, bytes.as_mut_ptr()) };
    }
    SUCCESS
}