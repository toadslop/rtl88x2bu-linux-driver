// SPDX-License-Identifier: GPL-2.0
//! W3-195 — `_rtw_pwr_wakeup`, `rtw_set_ips_deny` from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_wakeup_test)]
use std::ffi::{c_char, c_int, c_void};

#[cfg(not(host_pwrctrl_wakeup_test))]
use core::ffi::{c_char, c_int, c_ulong, c_void};

#[cfg(not(host_pwrctrl_wakeup_test))]
pub(crate) type Systime = c_ulong;

#[cfg(host_pwrctrl_wakeup_test)]
pub(crate) type Systime = u32;

pub(crate) type Padapter = *mut c_void;

pub(crate) const _TRUE: u8 = 1;
pub(crate) const _FALSE: u8 = 0;
pub(crate) const _SUCCESS: c_int = 1;
pub(crate) const _FAIL: c_int = 0;
const RF_OFF: u8 = 2;
const WIFI_ASOC_STATE: u32 = 0x0000_0001;

#[cfg(host_pwrctrl_wakeup_test)]
#[path = "rtw_pwrctrl_wakeup_host.rs"]
mod host;

#[cfg(host_pwrctrl_wakeup_test)]
use host::{
    check_fwstate, get_b_in_suspend, get_bup, get_current_time, get_ips_deny_time,
    get_passing_time_ms, get_ps_processing, get_rf_pwrstate, ips_leave, is_do_late_resume,
    is_drv_stopped, is_hw_init_completed, leave_all_ps, ms_to_systime, msleep_os, primary_adapter,
    pwrpriv, set_ips_deny_time, time_after,
};

#[cfg(not(host_pwrctrl_wakeup_test))]
extern "C" {
    fn rtw_rust_wakeup_leave_all_ps(adapter: Padapter);
    fn rtw_rust_wakeup_get_primary(adapter: Padapter) -> Padapter;
    fn rtw_rust_wakeup_get_current_time() -> Systime;
    fn rtw_rust_wakeup_ms_to_systime(ms: u32) -> Systime;
    fn rtw_rust_wakeup_time_after(a: Systime, b: Systime) -> u8;
    fn rtw_rust_wakeup_get_passing_time_ms(start: Systime) -> u32;
    fn rtw_rust_wakeup_msleep_os(ms: u32);
    fn rtw_rust_wakeup_pwrpriv(adapter: Padapter) -> *mut c_void;
    fn rtw_rust_wakeup_get_ps_processing(pwr: *mut c_void) -> u8;
    fn rtw_rust_wakeup_get_b_in_suspend(pwr: *mut c_void) -> u8;
    fn rtw_rust_wakeup_is_do_late_resume(pwr: *mut c_void) -> u8;
    fn rtw_rust_wakeup_get_ips_deny_time(pwr: *mut c_void) -> Systime;
    fn rtw_rust_wakeup_set_ips_deny_time(pwr: *mut c_void, t: Systime);
    fn rtw_rust_wakeup_check_fwstate(adapter: Padapter, state: u32) -> u8;
    fn rtw_rust_wakeup_get_rf_pwrstate(pwr: *mut c_void) -> u8;
    fn rtw_rust_wakeup_ips_leave(adapter: Padapter) -> c_int;
    fn rtw_rust_wakeup_is_drv_stopped(adapter: Padapter) -> u8;
    fn rtw_rust_wakeup_get_bup(adapter: Padapter) -> u8;
    fn rtw_rust_wakeup_is_hw_init_completed(adapter: Padapter) -> u8;
}

#[cfg(host_pwrctrl_wakeup_test)]
pub(crate) type PwrctrlPrivPtr = *mut c_void;

#[cfg(not(host_pwrctrl_wakeup_test))]
pub(crate) type PwrctrlPrivPtr = *mut c_void;

#[cfg(not(host_pwrctrl_wakeup_test))]
fn leave_all_ps(adapter: Padapter) {
    unsafe {
        rtw_rust_wakeup_leave_all_ps(adapter);
    }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn primary_adapter(adapter: Padapter) -> Padapter {
    unsafe { rtw_rust_wakeup_get_primary(adapter) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn pwrpriv(adapter: Padapter) -> PwrctrlPrivPtr {
    unsafe { rtw_rust_wakeup_pwrpriv(adapter) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn get_current_time() -> Systime {
    unsafe { rtw_rust_wakeup_get_current_time() }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn ms_to_systime(ms: u32) -> Systime {
    unsafe { rtw_rust_wakeup_ms_to_systime(ms) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn time_after(a: Systime, b: Systime) -> bool {
    unsafe { rtw_rust_wakeup_time_after(a, b) != 0 }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn get_passing_time_ms(start: Systime) -> u32 {
    unsafe { rtw_rust_wakeup_get_passing_time_ms(start) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn msleep_os(ms: u32) {
    unsafe {
        rtw_rust_wakeup_msleep_os(ms);
    }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn is_do_late_resume(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe { rtw_rust_wakeup_is_do_late_resume(pwr) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn check_fwstate(adapter: Padapter, state: u32) -> u8 {
    unsafe { rtw_rust_wakeup_check_fwstate(adapter, state) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn is_drv_stopped(adapter: Padapter) -> u8 {
    unsafe { rtw_rust_wakeup_is_drv_stopped(adapter) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn is_hw_init_completed(adapter: Padapter) -> u8 {
    unsafe { rtw_rust_wakeup_is_hw_init_completed(adapter) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn get_bup(adapter: Padapter) -> u8 {
    unsafe { rtw_rust_wakeup_get_bup(adapter) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn ips_leave(adapter: Padapter) -> c_int {
    unsafe { rtw_rust_wakeup_ips_leave(adapter) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn get_ps_processing(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe { rtw_rust_wakeup_get_ps_processing(pwr) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn get_b_in_suspend(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe { rtw_rust_wakeup_get_b_in_suspend(pwr) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn get_rf_pwrstate(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe { rtw_rust_wakeup_get_rf_pwrstate(pwr) }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn set_ips_deny_time(pwr: PwrctrlPrivPtr, t: Systime) {
    unsafe {
        rtw_rust_wakeup_set_ips_deny_time(pwr, t);
    }
}

#[cfg(not(host_pwrctrl_wakeup_test))]
fn get_ips_deny_time(pwr: PwrctrlPrivPtr) -> Systime {
    unsafe { rtw_rust_wakeup_get_ips_deny_time(pwr) }
}

fn bump_ips_deny(pwr: PwrctrlPrivPtr, ips_deffer_ms: u32) {
    let now = get_current_time();
    let candidate = now + ms_to_systime(ips_deffer_ms);
    if time_after(candidate, get_ips_deny_time(pwr)) {
        set_ips_deny_time(pwr, candidate);
    }
}

fn rtw_set_ips_deny_impl(padapter: Padapter, ms: u32) {
    if padapter.is_null() {
        return;
    }
    let pwr = pwrpriv(padapter);
    let t = get_current_time() + ms_to_systime(ms);
    set_ips_deny_time(pwr, t);
}

fn rtw_pwr_wakeup_impl(padapter: Padapter, ips_deffer_ms: u32, _caller: *const c_char) -> c_int {
    if padapter.is_null() {
        return _FAIL;
    }
    let pwr = pwrpriv(padapter);
    let start = get_current_time();
    let mut ret = _SUCCESS;

    leave_all_ps(padapter);
    let padapter = primary_adapter(padapter);
    bump_ips_deny(pwr, ips_deffer_ms);

    if get_ps_processing(pwr) == _TRUE {
        while get_ps_processing(pwr) == _TRUE && get_passing_time_ms(start) <= 3000 {
            msleep_os(10);
        }
    }

    if get_b_in_suspend(pwr) == _TRUE {
        while get_b_in_suspend(pwr) == _TRUE
            && ((get_passing_time_ms(start) <= 3000 && is_do_late_resume(pwr) == _FALSE)
                || (get_passing_time_ms(start) <= 500 && is_do_late_resume(pwr) == _TRUE))
        {
            msleep_os(10);
        }
    }

    if get_b_in_suspend(pwr) == _TRUE {
        ret = _FAIL;
    } else if check_fwstate(padapter, WIFI_ASOC_STATE) == _TRUE {
        ret = _SUCCESS;
    } else if get_rf_pwrstate(pwr) == RF_OFF {
        if ips_leave(padapter) == _FAIL {
            ret = _FAIL;
        }
        bump_ips_deny(pwr, ips_deffer_ms);
        return ret;
    } else if is_drv_stopped(padapter) != 0
        || get_bup(padapter) == 0
        || is_hw_init_completed(padapter) == 0
    {
        ret = 0;
    }

    bump_ips_deny(pwr, ips_deffer_ms);
    ret
}

#[no_mangle]
pub extern "C" fn rtw_set_ips_deny(padapter: Padapter, ms: u32) {
    rtw_set_ips_deny_impl(padapter, ms);
}

#[no_mangle]
pub extern "C" fn _rtw_pwr_wakeup(
    padapter: Padapter,
    ips_deffer_ms: u32,
    caller: *const c_char,
) -> c_int {
    rtw_pwr_wakeup_impl(padapter, ips_deffer_ms, caller)
}
