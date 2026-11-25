// SPDX-License-Identifier: GPL-2.0
// RCR check/add/clear leaf helpers (W4-127).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const SUCCESS: U8 = 1;
/* include/hal_com_reg.h BIT(2) */
const RCR_AM: U32 = 4;

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
    let mut rcr: U32 = 0;
    let hw_var = unsafe { rtw_rust_hal_com_hw_var_rcr() as U32 };
    // SAFETY: C shim reads HW_VAR_RCR into a stack u32; adapter is a valid host fixture.
    unsafe { rtw_rust_hal_com_get_hwreg(adapter, hw_var, &mut rcr as *mut U32 as *mut U8) };
    rcr |= add;
    let after_get = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    if rcr != after_get {
        // SAFETY: &mut rcr is a properly aligned u32 backing store, matching C (u8 *)&rcr.
        return unsafe {
            rtw_rust_hal_com_set_hwreg_u8(adapter, hw_var, &mut rcr as *mut U32 as *mut U8)
        };
    }
    SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_clear(adapter: *mut c_void, clear: U32) -> U8 {
    let mut rcr: U32 = 0;
    let hw_var = unsafe { rtw_rust_hal_com_hw_var_rcr() as U32 };
    // SAFETY: C shim reads HW_VAR_RCR into a stack u32; adapter is a valid host fixture.
    unsafe { rtw_rust_hal_com_get_hwreg(adapter, hw_var, &mut rcr as *mut U32 as *mut U8) };
    rcr &= !clear;
    let after_get = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    if rcr != after_get {
        // SAFETY: &mut rcr is a properly aligned u32 backing store, matching C (u8 *)&rcr.
        return unsafe {
            rtw_rust_hal_com_set_hwreg_u8(adapter, hw_var, &mut rcr as *mut U32 as *mut U8)
        };
    }
    SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_rust_hw_var_set_rcr_am(adapter: *mut c_void, enable: U8) {
    if enable != 0 {
        rtw_hal_rcr_add(adapter, RCR_AM);
    } else {
        rtw_hal_rcr_clear(adapter, RCR_AM);
    }
}