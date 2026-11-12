// SPDX-License-Identifier: GPL-2.0
//! Host L2 stubs for `rtw_pwrctrl_ps.rs` (shared ps_processor core).

#![allow(non_snake_case, non_camel_case_types)]

use crate::{Padapter, _TRUE};

#[repr(C)]
pub struct PwrctrlPriv {
    pub lock: i32,
    pub ps_deny: u32,
    pub bInSuspend: u8,
    pub ps_processing: u8,
    pub ips_mode_req: u8,
    pub rf_pwrstate: u8,
    pub change_rfpwrstate: u8,
    pub pwr_state_check_cnts: u32,
}

#[repr(C)]
pub struct MlmePriv {
    pub fw_state: u32,
}

#[repr(C)]
pub struct HostAdapter {
    pub pwrctrlpriv: PwrctrlPriv,
    pub mlmepriv: MlmePriv,
}

#[repr(C)]
pub struct HostPsProcTrace {
    pub ips_enter: u32,
    pub pwr_timer_set: u32,
}

static mut G_TRACE: HostPsProcTrace = HostPsProcTrace {
    ips_enter: 0,
    pwr_timer_set: 0,
};
static mut G_UNASSOC_IDLE: u8 = _TRUE;

pub fn reset_trace() {
    unsafe {
        G_TRACE = HostPsProcTrace {
            ips_enter: 0,
            pwr_timer_set: 0,
        };
        G_UNASSOC_IDLE = _TRUE;
    }
}

pub fn set_unassoc_idle(v: u8) {
    unsafe {
        G_UNASSOC_IDLE = v;
    }
}

pub fn trace_ptr() -> *mut HostPsProcTrace {
    unsafe { std::ptr::addr_of_mut!(G_TRACE) }
}

fn adapter(padapter: Padapter) -> *mut HostAdapter {
    padapter as *mut HostAdapter
}

fn pwr(padapter: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*adapter(padapter)).pwrctrlpriv }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_reset_trace() {
    reset_trace();
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_set_unassoc_idle(idle: u8) {
    set_unassoc_idle(idle);
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_get_trace() -> *mut HostPsProcTrace {
    trace_ptr()
}

#[no_mangle]
pub extern "C" fn rtw_ps_deny_get(padapter: Padapter) -> u32 {
    unsafe { (*pwr(padapter)).ps_deny }
}

#[no_mangle]
pub extern "C" fn ips_enter(padapter: Padapter) {
    let _ = padapter;
    unsafe {
        G_TRACE.ips_enter += 1;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_enter_pwrlock(_padapter: Padapter) {}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_exit_pwrlock(_padapter: Padapter) {}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_get_b_in_suspend(padapter: Padapter) -> u8 {
    unsafe { (*pwr(padapter)).bInSuspend }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_set_ps_processing(padapter: Padapter, v: u8) {
    unsafe {
        (*pwr(padapter)).ps_processing = v;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_get_ips_mode_req(padapter: Padapter) -> u8 {
    unsafe { (*pwr(padapter)).ips_mode_req }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_get_rf_pwrstate(padapter: Padapter) -> u8 {
    unsafe { (*pwr(padapter)).rf_pwrstate }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_set_change_rfpwrstate(padapter: Padapter, v: u8) {
    unsafe {
        (*pwr(padapter)).change_rfpwrstate = v;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_get_pwr_state_check_cnts(padapter: Padapter) -> u32 {
    unsafe { (*pwr(padapter)).pwr_state_check_cnts }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_unassociated_idle(_padapter: Padapter) -> u8 {
    unsafe { G_UNASSOC_IDLE }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_set_pwr_timer(padapter: Padapter) {
    let _ = padapter;
    unsafe {
        G_TRACE.pwr_timer_set += 1;
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_report_ps_deny(_padapter: Padapter, _ps_deny: u32) {}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_proc_b_in_suspend(_padapter: Padapter) {}
