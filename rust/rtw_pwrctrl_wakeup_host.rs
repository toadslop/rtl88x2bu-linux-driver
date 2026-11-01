// SPDX-License-Identifier: GPL-2.0
//! Host L2 stubs for W3-195 pwr wakeup (see `rtw_pwrctrl_wakeup.rs`).

use std::ffi::c_int;

use super::{Padapter, PwrctrlPrivPtr, Systime, _FALSE, _SUCCESS, _TRUE};

#[repr(C)]
struct MlmePriv {
    fw_state: u32,
}

#[repr(C)]
struct PwrctrlPriv {
    ps_processing: u8,
    bInSuspend: u8,
    do_late_resume: u8,
    rf_pwrstate: u8,
    ips_deny_time: Systime,
}

#[repr(C)]
struct DvobjPriv {
    padapters: [*mut HostAdapter; 4],
}

#[repr(C)]
struct HostAdapter {
    dvobj: *mut DvobjPriv,
    mlmepriv: MlmePriv,
    pwrctrlpriv: PwrctrlPriv,
    bup: u8,
}

static mut G_TIME: Systime = 0;
static mut G_PASSING_MS: u32 = 0;
static mut G_TRACE_LEAVE: u32 = 0;
static mut G_TRACE_IPS_LEAVE: u32 = 0;
static mut G_IPS_LEAVE_RESULT: c_int = _SUCCESS;

#[no_mangle]
pub extern "C" fn host_pwrctrl_wakeup_reset_trace() {
    unsafe {
        G_TRACE_LEAVE = 0;
        G_TRACE_IPS_LEAVE = 0;
        G_IPS_LEAVE_RESULT = _SUCCESS;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_wakeup_set_ips_leave_result(result: c_int) {
    unsafe {
        G_IPS_LEAVE_RESULT = result;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_wakeup_set_passing_ms(ms: u32) {
    unsafe {
        G_PASSING_MS = ms;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_wakeup_set_time(t: u32) {
    unsafe {
        G_TIME = t;
    }
}

#[repr(C)]
pub struct HostWakeupTrace {
    pub leave_all_ps: u32,
    pub ips_leave_calls: u32,
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_wakeup_get_trace() -> *mut HostWakeupTrace {
    static mut TR: HostWakeupTrace = HostWakeupTrace {
        leave_all_ps: 0,
        ips_leave_calls: 0,
    };
    unsafe {
        TR.leave_all_ps = G_TRACE_LEAVE;
        TR.ips_leave_calls = G_TRACE_IPS_LEAVE;
        core::ptr::addr_of_mut!(TR)
    }
}

pub fn primary_adapter(adapter: Padapter) -> Padapter {
    let a = adapter as *mut HostAdapter;
    unsafe { (*(*a).dvobj).padapters[0] as Padapter }
}

pub fn pwrpriv(adapter: Padapter) -> PwrctrlPrivPtr {
    unsafe {
        &mut (*(adapter as *mut HostAdapter)).pwrctrlpriv as *mut PwrctrlPriv as PwrctrlPrivPtr
    }
}

pub fn get_current_time() -> Systime {
    unsafe { G_TIME }
}

pub fn ms_to_systime(ms: u32) -> Systime {
    ms
}

pub fn time_after(a: Systime, b: Systime) -> bool {
    (a as i32).wrapping_sub(b as i32) > 0
}

pub fn get_passing_time_ms(_start: Systime) -> u32 {
    unsafe { G_PASSING_MS }
}

pub fn msleep_os(_ms: u32) {
    unsafe {
        G_PASSING_MS = G_PASSING_MS.saturating_add(10);
    }
}

pub fn leave_all_ps(_adapter: Padapter) {
    unsafe {
        G_TRACE_LEAVE += 1;
    }
}

pub fn is_do_late_resume(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe {
        let p = pwr as *mut PwrctrlPriv;
        if (*p).do_late_resume != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }
}

pub fn check_fwstate(adapter: Padapter, state: u32) -> u8 {
    let a = adapter as *mut HostAdapter;
    unsafe {
        if state == 0 && (*a).mlmepriv.fw_state == 0 {
            return _TRUE;
        }
        if ((*a).mlmepriv.fw_state & state) != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }
}

pub fn is_drv_stopped(_adapter: Padapter) -> u8 {
    _FALSE
}

pub fn is_hw_init_completed(_adapter: Padapter) -> u8 {
    _TRUE
}

pub fn get_bup(adapter: Padapter) -> u8 {
    unsafe { (*(adapter as *mut HostAdapter)).bup }
}

pub fn ips_leave(_adapter: Padapter) -> c_int {
    unsafe {
        G_TRACE_IPS_LEAVE += 1;
        G_IPS_LEAVE_RESULT
    }
}

pub fn get_ps_processing(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe { (*(pwr as *mut PwrctrlPriv)).ps_processing }
}

pub fn get_b_in_suspend(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe { (*(pwr as *mut PwrctrlPriv)).bInSuspend }
}

pub fn get_rf_pwrstate(pwr: PwrctrlPrivPtr) -> u8 {
    unsafe { (*(pwr as *mut PwrctrlPriv)).rf_pwrstate }
}

pub fn set_ips_deny_time(pwr: PwrctrlPrivPtr, t: Systime) {
    unsafe {
        (*(pwr as *mut PwrctrlPriv)).ips_deny_time = t;
    }
}

pub fn get_ips_deny_time(pwr: PwrctrlPrivPtr) -> Systime {
    unsafe { (*(pwr as *mut PwrctrlPriv)).ips_deny_time }
}
