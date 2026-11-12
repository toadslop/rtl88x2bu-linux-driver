// SPDX-License-Identifier: GPL-2.0
//! W3-188 — `LPS_Leave_check` + `cpwm_int_hdl` from `core/rtw_pwrctrl.c` (CONFIG_LPS_LCLK).

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

#[cfg(host_pwrctrl_cpwm_int_test)]
use std::ffi::{c_ulong, c_void};

#[cfg(not(host_pwrctrl_cpwm_int_test))]
use core::ffi::{c_ulong, c_void};

const _TRUE: u8 = 1;
const PS_MODE_ACTIVE: u8 = 0;
const PS_TOGGLE: u8 = 1 << 7;
const PS_STATE_MASK: u8 = 0x0F;
const PS_STATE_S2: u8 = 1 << 1;
const CMD_ALIVE: u8 = 1 << 2;
const XMIT_ALIVE: u8 = 1 << 0;

type Padapter = *mut c_void;
pub(crate) type Systime = c_ulong;

#[repr(C)]
pub struct ReportPwrStateParm {
    pub mode: u8,
    pub state: u8,
    pub rsvd: u16,
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[repr(C)]
pub(crate) struct PwrctrlPriv {
    lock: i32,
    pwr_mode: u8,
    cpwm: u8,
    cpwm_tog: u8,
    rpwm: u8,
    alives: u8,
    _pad: [u8; 3],
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[repr(C)]
pub(crate) struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
    b_surprise_removed: u8,
    b_hw_init_completed: u8,
    b_driver_stopped: u8,
    _pad: u8,
    cmd_sem_up: u32,
    xmit_sem_up: u32,
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[path = "rtw_pwrctrl_cpwm_int_host.rs"]
mod host;

#[inline]
pub(crate) fn ps_state(x: u8) -> u8 {
    PS_STATE_MASK & x
}

pub(crate) fn lps_leave_check_core(
    enter_lock: fn(Padapter),
    exit_lock: fn(Padapter),
    yield_os: fn(),
    now: fn() -> Systime,
    passing_ms: fn(Systime) -> u32,
    msleep: fn(i32),
    leave_ready: fn(Padapter) -> bool,
    leave_timeout_err: fn(),
    adapter: Padapter,
) {
    let start = now();
    yield_os();
    loop {
        enter_lock(adapter);
        let ready = leave_ready(adapter);
        exit_lock(adapter);
        if ready {
            break;
        }
        if passing_ms(start) > 100 {
            leave_timeout_err();
            break;
        }
        msleep(1);
    }
}

pub(crate) fn cpwm_int_hdl_core(
    cannot_run: fn(Padapter) -> bool,
    enter_lock: fn(Padapter),
    exit_lock: fn(Padapter),
    rpwm_timer_redundant: fn(Padapter) -> bool,
    set_cpwm_fields: fn(Padapter, u8),
    get_alives: fn(Padapter) -> u8,
    up_cmd_sema: fn(Padapter),
    up_xmit_sema: fn(Padapter),
    adapter: Padapter,
    report: *const ReportPwrStateParm,
) {
    if adapter.is_null() {
        return;
    }
    if cannot_run(adapter) {
        return;
    }
    enter_lock(adapter);
    if rpwm_timer_redundant(adapter) {
        exit_lock(adapter);
        return;
    }
    let st = unsafe { (*report).state };
    set_cpwm_fields(adapter, st);
    if ps_state(st) >= PS_STATE_S2 {
        let alives = get_alives(adapter);
        if alives & CMD_ALIVE != 0 {
            up_cmd_sema(adapter);
        }
        if alives & XMIT_ALIVE != 0 {
            up_xmit_sema(adapter);
        }
    }
    exit_lock(adapter);
}

#[cfg(not(host_pwrctrl_cpwm_int_test))]
extern "C" {
    fn rtw_rust_cpwm_enter_pwrlock(padapter: Padapter);
    fn rtw_rust_cpwm_exit_pwrlock(padapter: Padapter);
    fn rtw_rust_cpwm_yield_os();
    fn rtw_rust_cpwm_get_current_time() -> Systime;
    fn rtw_rust_cpwm_get_passing_time_ms(start: Systime) -> u32;
    fn rtw_rust_cpwm_msleep_os(ms: i32);
    fn rtw_rust_cpwm_leave_timeout_err();
    fn rtw_rust_cpwm_leave_ready(padapter: Padapter) -> u8;
    fn rtw_rust_cpwm_cannot_run(padapter: Padapter) -> u8;
    fn rtw_rust_cpwm_set_cpwm_fields(padapter: Padapter, state: u8);
    fn rtw_rust_cpwm_get_alives(padapter: Padapter) -> u8;
    fn rtw_rust_cpwm_up_cmd_sema(padapter: Padapter);
    fn rtw_rust_cpwm_up_xmit_sema(padapter: Padapter);
    fn rtw_rust_cpwm_rpwm_timer_redundant(padapter: Padapter) -> u8;
}

#[cfg(not(host_pwrctrl_cpwm_int_test))]
fn lps_leave_check_kernel(a: Padapter) {
    unsafe {
        lps_leave_check_core(
            |p| rtw_rust_cpwm_enter_pwrlock(p),
            |p| rtw_rust_cpwm_exit_pwrlock(p),
            || rtw_rust_cpwm_yield_os(),
            || rtw_rust_cpwm_get_current_time(),
            |s| rtw_rust_cpwm_get_passing_time_ms(s),
            |ms| rtw_rust_cpwm_msleep_os(ms),
            |p| rtw_rust_cpwm_leave_ready(p) == _TRUE,
            || rtw_rust_cpwm_leave_timeout_err(),
            a,
        );
    }
}

#[cfg(not(host_pwrctrl_cpwm_int_test))]
fn cpwm_int_hdl_kernel(a: Padapter, report: *const ReportPwrStateParm) {
    unsafe {
        cpwm_int_hdl_core(
            |p| rtw_rust_cpwm_cannot_run(p) == _TRUE,
            |p| rtw_rust_cpwm_enter_pwrlock(p),
            |p| rtw_rust_cpwm_exit_pwrlock(p),
            |p| rtw_rust_cpwm_rpwm_timer_redundant(p) == _TRUE,
            |p, st| rtw_rust_cpwm_set_cpwm_fields(p, st),
            |p| rtw_rust_cpwm_get_alives(p),
            |p| rtw_rust_cpwm_up_cmd_sema(p),
            |p| rtw_rust_cpwm_up_xmit_sema(p),
            a,
            report,
        );
    }
}

#[no_mangle]
pub extern "C" fn LPS_Leave_check(padapter: Padapter) {
    #[cfg(host_pwrctrl_cpwm_int_test)]
    host::lps_leave_check(padapter);
    #[cfg(not(host_pwrctrl_cpwm_int_test))]
    lps_leave_check_kernel(padapter);
}

#[no_mangle]
pub extern "C" fn cpwm_int_hdl(padapter: Padapter, preportpwrstate: *mut ReportPwrStateParm) {
    #[cfg(host_pwrctrl_cpwm_int_test)]
    host::cpwm_int_hdl(padapter, preportpwrstate);
    #[cfg(not(host_pwrctrl_cpwm_int_test))]
    cpwm_int_hdl_kernel(padapter, preportpwrstate);
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_cpwm_int_reset() {
    host::reset();
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_cpwm_int_set_time(t: Systime) {
    host::set_time(t);
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_cpwm_int_get_cpwm(adapter: Padapter) -> u8 {
    host::get_cpwm(adapter)
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_cpwm_int_get_cpwm_tog(adapter: Padapter) -> u8 {
    host::get_cpwm_tog(adapter)
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_cpwm_int_cmd_sem_up(adapter: Padapter) -> u32 {
    host::cmd_sem_up(adapter)
}

#[cfg(host_pwrctrl_cpwm_int_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_cpwm_int_xmit_sem_up(adapter: Padapter) -> u32 {
    host::xmit_sem_up(adapter)
}
