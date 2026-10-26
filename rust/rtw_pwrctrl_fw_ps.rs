// SPDX-License-Identifier: GPL-2.0
//! W3-172 — `rtw_fw_ps_state` from `core/rtw_pwrctrl.c` (8822B `BIT_LPS_STATUS` path).

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

const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;
const BIT_LPS_STATUS: u16 = 1 << 3;

type Padapter = *mut c_void;

extern "C" {
    fn rtw_rust_fw_ps_check_enabled(padapter: Padapter) -> u8;
    fn rtw_rust_fw_ps_enter_32k_lock(padapter: Padapter);
    fn rtw_rust_fw_ps_exit_32k_lock(padapter: Padapter);
    fn rtw_rust_fw_ps_cannot_run(padapter: Padapter) -> u8;
    fn rtw_rust_fw_ps_hal_get_state(padapter: Padapter, fw_ps_state: *mut u16);
    fn rtw_rust_fw_ps_inc_poll_fail_cnt(padapter: Padapter);
}

#[no_mangle]
pub extern "C" fn rtw_fw_ps_state(padapter: Padapter) -> i32 {
    if padapter.is_null() {
        return _FAIL;
    }
    if unsafe { rtw_rust_fw_ps_check_enabled(padapter) } == 0 {
        return _SUCCESS;
    }

    let mut ret = _FAIL;
    unsafe { rtw_rust_fw_ps_enter_32k_lock(padapter) };

    if unsafe { rtw_rust_fw_ps_cannot_run(padapter) } != 0 {
        unsafe { rtw_rust_fw_ps_exit_32k_lock(padapter) };
        return ret;
    }

    let mut fw_ps_state: u16 = 0;
    unsafe {
        rtw_rust_fw_ps_hal_get_state(padapter, &mut fw_ps_state);
        if (fw_ps_state & BIT_LPS_STATUS) == 0 {
            ret = _SUCCESS;
        } else {
            rtw_rust_fw_ps_inc_poll_fail_cnt(padapter);
        }
    }

    unsafe { rtw_rust_fw_ps_exit_32k_lock(padapter) };
    ret
}
