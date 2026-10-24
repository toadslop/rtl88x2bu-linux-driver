// SPDX-License-Identifier: GPL-2.0
//! W3-167 PR1 — IPS enter helpers from `core/rtw_pwrctrl.c`.

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

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const RF_OFF: u8 = 2;
const IPS_LEVEL_2: u8 = 2;

const F_RF_PWRSTATE: u32 = 0;
const F_CHANGE_RFPWRSTATE: u32 = 1;
const F_BIPS_PROCESSING: u32 = 2;
const F_IPS_MODE: u32 = 3;
const F_IPS_MODE_REQ: u32 = 4;
const F_BPOWER_SAVING: u32 = 5;
const F_BKEEPFWALIVE: u32 = 6;

type Padapter = *mut c_void;

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
}

fn ips_enter_core(padapter: Padapter) {
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
