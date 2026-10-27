// SPDX-License-Identifier: GPL-2.0
//! W3-179 — register task/IO alive helpers from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_alive_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_alive_test))]
use core::ffi::c_void;

const _TRUE: u8 = 1;
const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;
const PS_STATE_S2: u8 = 1 << 1;
const XMIT_ALIVE: u32 = 1 << 0;
const RECV_ALIVE: u32 = 1 << 1;
const CMD_ALIVE: u32 = 1 << 2;
const EVT_ALIVE: u32 = 1 << 3;

type Padapter = *mut c_void;

#[cfg(host_pwrctrl_alive_test)]
#[repr(C)]
struct PwrctrlPriv {
    lock: i32,
    alives: u32,
    bFwCurrentInPSMode: u8,
    cpwm: u8,
    rpwm: u8,
}

#[cfg(host_pwrctrl_alive_test)]
#[repr(C)]
struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
}

#[cfg(host_pwrctrl_alive_test)]
#[repr(C)]
pub struct HostAliveTrace {
    pub set_rpwm_calls: u32,
    pub last_set_rpwm_pslv: u8,
}

#[cfg(host_pwrctrl_alive_test)]
mod host_hal {
    use super::*;

    static mut G_TRACE: HostAliveTrace = HostAliveTrace {
        set_rpwm_calls: 0,
        last_set_rpwm_pslv: 0,
    };
    static mut G_SET_RPWM_RAISES_CPWM: u8 = 0;

    pub fn reset_trace() {
        unsafe {
            G_TRACE = HostAliveTrace {
                set_rpwm_calls: 0,
                last_set_rpwm_pslv: 0,
            };
            G_SET_RPWM_RAISES_CPWM = 0;
        }
    }

    pub fn set_rpwm_raises_cpwm(v: u8) {
        unsafe {
            G_SET_RPWM_RAISES_CPWM = v;
        }
    }

    pub fn trace_ptr() -> *mut HostAliveTrace {
        unsafe { &raw mut G_TRACE }
    }

    fn pwr(a: Padapter) -> *mut PwrctrlPriv {
        unsafe { &mut (*(a as *mut HostAdapter)).pwrctrlpriv }
    }

    pub fn enter_lock(_a: Padapter) {}

    pub fn exit_lock(_a: Padapter) {}

    pub fn or_alives(a: Padapter, tag: u32) {
        let p = pwr(a);
        unsafe {
            (*p).alives |= tag;
        }
    }

    pub fn get_u8(a: Padapter, field: u32) -> u8 {
        let p = pwr(a);
        unsafe {
            match field {
                0 => (*p).bFwCurrentInPSMode,
                1 => (*p).cpwm,
                2 => (*p).rpwm,
                _ => 0,
            }
        }
    }

    pub fn set_rpwm(a: Padapter, pslv: u8) -> u8 {
        let p = pwr(a);
        unsafe {
            G_TRACE.set_rpwm_calls += 1;
            G_TRACE.last_set_rpwm_pslv = pslv;
            (*p).rpwm = pslv;
            if G_SET_RPWM_RAISES_CPWM != 0 {
                (*p).cpwm = pslv;
            }
        }
        pslv
    }
}

#[cfg(not(host_pwrctrl_alive_test))]
extern "C" {
    fn rtw_rust_alive_enter_pwrlock(padapter: Padapter);
    fn rtw_rust_alive_exit_pwrlock(padapter: Padapter);
    fn rtw_rust_alive_or_alives(padapter: Padapter, tag: u32);
    fn rtw_rust_alive_get_u8(padapter: Padapter, field: u32) -> u8;
    fn rtw_set_rpwm(padapter: Padapter, pslv: u8) -> u8;
}

#[cfg(not(host_pwrctrl_alive_test))]
fn enter_lock(a: Padapter) {
    unsafe { rtw_rust_alive_enter_pwrlock(a) };
}

#[cfg(not(host_pwrctrl_alive_test))]
fn exit_lock(a: Padapter) {
    unsafe { rtw_rust_alive_exit_pwrlock(a) };
}

#[cfg(not(host_pwrctrl_alive_test))]
fn or_alives(a: Padapter, tag: u32) {
    unsafe { rtw_rust_alive_or_alives(a, tag) };
}

#[cfg(not(host_pwrctrl_alive_test))]
fn get_u8(a: Padapter, field: u32) -> u8 {
    unsafe { rtw_rust_alive_get_u8(a, field) }
}

#[cfg(not(host_pwrctrl_alive_test))]
fn set_rpwm(a: Padapter, pslv: u8) -> u8 {
    unsafe { rtw_set_rpwm(a, pslv) }
}

#[cfg(host_pwrctrl_alive_test)]
fn enter_lock(a: Padapter) {
    host_hal::enter_lock(a);
}

#[cfg(host_pwrctrl_alive_test)]
fn exit_lock(a: Padapter) {
    host_hal::exit_lock(a);
}

#[cfg(host_pwrctrl_alive_test)]
fn or_alives(a: Padapter, tag: u32) {
    host_hal::or_alives(a, tag);
}

#[cfg(host_pwrctrl_alive_test)]
fn get_u8(a: Padapter, field: u32) -> u8 {
    host_hal::get_u8(a, field)
}

#[cfg(host_pwrctrl_alive_test)]
fn set_rpwm(a: Padapter, pslv: u8) -> u8 {
    host_hal::set_rpwm(a, pslv)
}

fn register_ps_alive(a: Padapter, tag: u32) -> i32 {
    let mut res = _SUCCESS;
    let pslv = PS_STATE_S2;

    enter_lock(a);
    or_alives(a, tag);

    if get_u8(a, 0) == _TRUE {
        let cpwm = get_u8(a, 1);
        let rpwm = get_u8(a, 2);
        if cpwm < pslv {
            if cpwm < PS_STATE_S2 {
                res = _FAIL;
            }
            if rpwm < pslv {
                set_rpwm(a, pslv);
            }
        }
    }

    exit_lock(a);

    #[cfg(any(detect_cpwm_by_polling, host_pwrctrl_alive_test))]
    if res == _FAIL && get_u8(a, 1) >= PS_STATE_S2 {
        res = _SUCCESS;
    }

    res
}

fn register_simple_alive(a: Padapter, tag: u32) -> i32 {
    enter_lock(a);
    or_alives(a, tag);
    exit_lock(a);
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_register_task_alive(padapter: Padapter, task: u32) -> i32 {
    register_ps_alive(padapter, task)
}

#[no_mangle]
pub extern "C" fn rtw_register_tx_alive(padapter: Padapter) -> i32 {
    register_ps_alive(padapter, XMIT_ALIVE)
}

#[no_mangle]
pub extern "C" fn rtw_register_cmd_alive(padapter: Padapter) -> i32 {
    register_ps_alive(padapter, CMD_ALIVE)
}

#[no_mangle]
pub extern "C" fn rtw_register_rx_alive(padapter: Padapter) -> i32 {
    register_simple_alive(padapter, RECV_ALIVE)
}

#[no_mangle]
pub extern "C" fn rtw_register_evt_alive(padapter: Padapter) -> i32 {
    register_simple_alive(padapter, EVT_ALIVE)
}

#[cfg(host_pwrctrl_alive_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_alive_reset_trace() {
    host_hal::reset_trace();
}

#[cfg(host_pwrctrl_alive_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_alive_get_trace() -> *mut HostAliveTrace {
    host_hal::trace_ptr()
}

#[cfg(host_pwrctrl_alive_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_alive_set_rpwm_raises_cpwm(v: u8) {
    host_hal::set_rpwm_raises_cpwm(v);
}
