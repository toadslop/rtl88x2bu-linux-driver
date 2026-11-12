// SPDX-License-Identifier: GPL-2.0
//! W3-167 — IPS enter/leave helpers from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_ips_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_ips_test))]
use core::ffi::c_void;

pub(crate) const _TRUE: u8 = 1;
pub(crate) const _FALSE: u8 = 0;
const _SUCCESS: i32 = 1;
pub(crate) const RF_OFF: u8 = 2;
pub(crate) const RF_ON: u8 = 0;
pub(crate) const IPS_LEVEL_2: u8 = 2;
pub(crate) const IPS_NONE: u8 = 0;

pub(crate) const F_RF_PWRSTATE: u32 = 0;
pub(crate) const F_CHANGE_RFPWRSTATE: u32 = 1;
pub(crate) const F_BIPS_PROCESSING: u32 = 2;
pub(crate) const F_IPS_MODE: u32 = 3;
pub(crate) const F_IPS_MODE_REQ: u32 = 4;
pub(crate) const F_BPOWER_SAVING: u32 = 5;
pub(crate) const F_BKEEPFWALIVE: u32 = 6;

pub(crate) type Padapter = *mut c_void;

#[cfg(host_pwrctrl_ips_test)]
#[path = "rtw_pwrctrl_ips_host.rs"]
mod host;

extern "C" {
    fn rtw_rust_pwrctrl_enter_lock(padapter: Padapter);
    fn rtw_rust_pwrctrl_exit_lock(padapter: Padapter);
    fn rtw_rust_ips_pwrpriv(padapter: Padapter) -> *mut c_void;
    fn rtw_rust_ips_get_u8(pwr: *mut c_void, field: u32) -> u8;
    fn rtw_rust_ips_set_u8(pwr: *mut c_void, field: u32, v: u8);
    fn rtw_rust_ips_inc_enter_cnt(pwr: *mut c_void);
    fn rtw_rust_ips_pwr_down(padapter: Padapter);
    fn rtw_rust_ips_btcoex_notify(padapter: Padapter, mode: u8);
    fn rtw_rust_ips_pci_aspm_ps(padapter: Padapter);
    fn rtw_rust_ips_inc_leave_cnt(pwr: *mut c_void);
    fn rtw_rust_ips_pwr_up(padapter: Padapter) -> i32;
    fn rtw_rust_ips_is_primary(padapter: Padapter) -> u8;
    fn rtw_rust_ips_pci_aspm_perf(padapter: Padapter);
    fn rtw_rust_ips_odm_reset(padapter: Padapter);
    fn rtw_rust_ips_dbg_leave_fail(padapter: Padapter);
    fn rtw_rust_ips_llstats_on_enter(pwr: *mut c_void);
    fn rtw_rust_ips_llstats_on_leave(pwr: *mut c_void);
}

pub(crate) fn ips_enter_core(padapter: Padapter) {
    let pwr = unsafe { rtw_rust_ips_pwrpriv(padapter) };
    if pwr.is_null() {
        return;
    }
    unsafe {
        rtw_rust_ips_set_u8(pwr, F_BIPS_PROCESSING, _TRUE);
        let req = rtw_rust_ips_get_u8(pwr, F_IPS_MODE_REQ);
        rtw_rust_ips_set_u8(pwr, F_IPS_MODE, req);
        rtw_rust_ips_inc_enter_cnt(pwr);
        if rtw_rust_ips_get_u8(pwr, F_CHANGE_RFPWRSTATE) == RF_OFF {
            rtw_rust_ips_set_u8(pwr, F_BPOWER_SAVING, _TRUE);
            if rtw_rust_ips_get_u8(pwr, F_IPS_MODE) == IPS_LEVEL_2 {
                rtw_rust_ips_set_u8(pwr, F_BKEEPFWALIVE, _TRUE);
            }
            rtw_rust_ips_llstats_on_enter(pwr);
            rtw_rust_ips_pwr_down(padapter);
            rtw_rust_ips_set_u8(pwr, F_RF_PWRSTATE, RF_OFF);
        }
        rtw_rust_ips_set_u8(pwr, F_BIPS_PROCESSING, _FALSE);
    }
}

#[no_mangle]
pub extern "C" fn _ips_enter(padapter: Padapter) {
    if !padapter.is_null() {
        ips_enter_core(padapter);
    }
}

#[no_mangle]
pub extern "C" fn ips_enter(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    let pwr = unsafe { rtw_rust_ips_pwrpriv(padapter) };
    let mode_req = unsafe { rtw_rust_ips_get_u8(pwr, F_IPS_MODE_REQ) };
    unsafe {
        rtw_rust_ips_btcoex_notify(padapter, mode_req);
        rtw_rust_pwrctrl_enter_lock(padapter);
    }
    ips_enter_core(padapter);
    unsafe {
        rtw_rust_pwrctrl_exit_lock(padapter);
        rtw_rust_ips_pci_aspm_ps(padapter);
    }
}

pub(crate) fn ips_leave_core(padapter: Padapter) -> i32 {
    let pwr = unsafe { rtw_rust_ips_pwrpriv(padapter) };
    if pwr.is_null() {
        return _SUCCESS;
    }
    unsafe {
        if rtw_rust_ips_get_u8(pwr, F_RF_PWRSTATE) == RF_OFF
            && rtw_rust_ips_get_u8(pwr, F_BIPS_PROCESSING) == 0
        {
            rtw_rust_ips_set_u8(pwr, F_BIPS_PROCESSING, _TRUE);
            rtw_rust_ips_set_u8(pwr, F_CHANGE_RFPWRSTATE, RF_ON);
            rtw_rust_ips_inc_leave_cnt(pwr);
            let result = rtw_rust_ips_pwr_up(padapter);
            if result == _SUCCESS {
                rtw_rust_ips_set_u8(pwr, F_RF_PWRSTATE, RF_ON);
            }
            rtw_rust_ips_llstats_on_leave(pwr);
            rtw_rust_ips_set_u8(pwr, F_BIPS_PROCESSING, _FALSE);
            rtw_rust_ips_set_u8(pwr, F_BKEEPFWALIVE, _FALSE);
            rtw_rust_ips_set_u8(pwr, F_BPOWER_SAVING, _FALSE);
            result
        } else {
            _SUCCESS
        }
    }
}

#[no_mangle]
pub extern "C" fn _ips_leave(padapter: Padapter) -> i32 {
    if padapter.is_null() {
        return _SUCCESS;
    }
    ips_leave_core(padapter)
}

#[no_mangle]
pub extern "C" fn ips_leave(padapter: Padapter) -> i32 {
    if padapter.is_null() {
        return _SUCCESS;
    }
    if unsafe { rtw_rust_ips_is_primary(padapter) } == 0 {
        return _SUCCESS;
    }
    unsafe { rtw_rust_pwrctrl_enter_lock(padapter) };
    let ret = ips_leave_core(padapter);
    unsafe {
        rtw_rust_ips_dbg_leave_fail(padapter);
        rtw_rust_pwrctrl_exit_lock(padapter);
        rtw_rust_ips_pci_aspm_perf(padapter);
    }
    if ret == _SUCCESS {
        unsafe {
            rtw_rust_ips_odm_reset(padapter);
            rtw_rust_ips_btcoex_notify(padapter, IPS_NONE);
        }
    }
    ret
}

#[cfg(host_pwrctrl_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ips_reset_trace() {
    host::reset_trace();
}

#[cfg(host_pwrctrl_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ips_set_pwr_up_result(r: i32) {
    host::set_pwr_up_result(r);
}

#[cfg(host_pwrctrl_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ips_get_trace() -> *mut host::HostIpsTrace {
    host::trace_ptr()
}
