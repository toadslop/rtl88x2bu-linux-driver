// SPDX-License-Identifier: GPL-2.0
//! Host L2 stubs for `rtw_pwrctrl_ips.rs` (shared IPS cores).

#![allow(non_snake_case, non_camel_case_types)]

use crate::{
    Padapter, F_BIPS_PROCESSING, F_BKEEPFWALIVE, F_BPOWER_SAVING, F_CHANGE_RFPWRSTATE, F_IPS_MODE,
    F_IPS_MODE_REQ, F_RF_PWRSTATE, _FALSE, _SUCCESS, _TRUE,
};
use std::ffi::c_void;

#[repr(C)]
pub struct PwrctrlPriv {
    pub lock: i32,
    pub bips_processing: u32,
    pub ips_mode: u8,
    pub ips_mode_req: u8,
    pub ips_enter_cnts: u32,
    pub ips_leave_cnts: u32,
    pub change_rfpwrstate: u8,
    pub bpower_saving: u8,
    pub bkeepfwalive: u8,
    pub rf_pwrstate: u8,
}

#[repr(C)]
pub struct HostAdapter {
    pub adapter_type: u8,
    pub pwrctrlpriv: PwrctrlPriv,
}

#[repr(C)]
pub struct HostIpsTrace {
    pub ips_pwr_down: u32,
    pub ips_pwr_up: u32,
    pub ips_pwr_up_result: i32,
    pub btcoex_notify: u32,
    pub last_btcoex_mode: u8,
    pub is_primary: u8,
}

static mut G_TRACE: HostIpsTrace = HostIpsTrace {
    ips_pwr_down: 0,
    ips_pwr_up: 0,
    ips_pwr_up_result: _SUCCESS,
    btcoex_notify: 0,
    last_btcoex_mode: 0,
    is_primary: 1,
};

pub fn reset_trace() {
    unsafe {
        G_TRACE = HostIpsTrace {
            ips_pwr_down: 0,
            ips_pwr_up: 0,
            ips_pwr_up_result: _SUCCESS,
            btcoex_notify: 0,
            last_btcoex_mode: 0,
            is_primary: 1,
        };
    }
}

pub fn set_pwr_up_result(r: i32) {
    unsafe {
        G_TRACE.ips_pwr_up_result = r;
    }
}

pub fn trace_ptr() -> *mut HostIpsTrace {
    unsafe { std::ptr::addr_of_mut!(G_TRACE) }
}

fn adapter(padapter: Padapter) -> *mut HostAdapter {
    padapter as *mut HostAdapter
}

fn pwr(padapter: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*adapter(padapter)).pwrctrlpriv }
}

#[no_mangle]
pub extern "C" fn rtw_rust_pwrctrl_enter_lock(_padapter: Padapter) {}

#[no_mangle]
pub extern "C" fn rtw_rust_pwrctrl_exit_lock(_padapter: Padapter) {}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_pwrpriv(padapter: Padapter) -> *mut c_void {
    pwr(padapter) as *mut c_void
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_get_u8(pwr: *mut c_void, field: u32) -> u8 {
    let p = pwr as *mut PwrctrlPriv;
    unsafe {
        match field {
            F_RF_PWRSTATE => (*p).rf_pwrstate,
            F_CHANGE_RFPWRSTATE => (*p).change_rfpwrstate,
            F_BIPS_PROCESSING => (*p).bips_processing as u8,
            F_IPS_MODE => (*p).ips_mode,
            F_IPS_MODE_REQ => (*p).ips_mode_req,
            F_BPOWER_SAVING => (*p).bpower_saving,
            F_BKEEPFWALIVE => (*p).bkeepfwalive,
            _ => 0,
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_set_u8(pwr: *mut c_void, field: u32, v: u8) {
    let p = pwr as *mut PwrctrlPriv;
    unsafe {
        match field {
            F_RF_PWRSTATE => (*p).rf_pwrstate = v,
            F_CHANGE_RFPWRSTATE => (*p).change_rfpwrstate = v,
            F_BIPS_PROCESSING => (*p).bips_processing = v as u32,
            F_IPS_MODE => (*p).ips_mode = v,
            F_IPS_MODE_REQ => (*p).ips_mode_req = v,
            F_BPOWER_SAVING => (*p).bpower_saving = v,
            F_BKEEPFWALIVE => (*p).bkeepfwalive = v,
            _ => {}
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_inc_enter_cnt(pwr: *mut c_void) {
    let p = pwr as *mut PwrctrlPriv;
    unsafe {
        (*p).ips_enter_cnts += 1;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_inc_leave_cnt(pwr: *mut c_void) {
    let p = pwr as *mut PwrctrlPriv;
    unsafe {
        (*p).ips_leave_cnts += 1;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_pwr_down(_padapter: Padapter) {
    unsafe {
        G_TRACE.ips_pwr_down += 1;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_pwr_up(_padapter: Padapter) -> i32 {
    unsafe {
        G_TRACE.ips_pwr_up += 1;
        G_TRACE.ips_pwr_up_result
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_is_primary(padapter: Padapter) -> u8 {
    unsafe {
        if (*adapter(padapter)).adapter_type == 0 {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_btcoex_notify(_padapter: Padapter, mode: u8) {
    unsafe {
        G_TRACE.btcoex_notify += 1;
        G_TRACE.last_btcoex_mode = mode;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_pci_aspm_ps(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_pci_aspm_perf(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_odm_reset(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_dbg_leave_fail(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_llstats_on_enter(_pwr: *mut c_void) {
    let _ = _pwr;
}

#[no_mangle]
pub extern "C" fn rtw_rust_ips_llstats_on_leave(_pwr: *mut c_void) {
    let _ = _pwr;
}
