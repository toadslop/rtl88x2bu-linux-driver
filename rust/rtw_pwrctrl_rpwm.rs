// SPDX-License-Identifier: GPL-2.0
//! W3-175 — `rtw_cpwm_polling` / `rtw_set_rpwm` from `core/rtw_pwrctrl.c`.

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

use std::ffi::c_void;

const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;
const PS_TOGGLE: u8 = 1 << 7;
const PS_STATE_S4: u8 = (1 << 3) | (1 << 2); /* PS_ST_ACTIVE | PS_ALL_ON */
const LPS_CPWM_TIMEOUT_MS: u32 = 10;
const LPS_RPWM_RETRY_CNT: u32 = 3;

type Padapter = *mut c_void;

#[repr(C)]
struct PwrctrlPriv {
    cpwm: u8,
    cpwm_tog: u8,
    tog: u8,
    rpwm_retry: u8,
}

#[repr(C)]
struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
    b_surprise_removed: u8,
    b_driver_stopped: u8,
}

mod host_hal {
    use super::*;

    static mut G_TIME: u32 = 0;
    static mut G_CPWM_ORIG: u8 = 0;
    static mut G_POLL_READS_BEFORE_TOGGLE: u32 = 0;
    static mut G_CPWM_READ_COUNT: u32 = 0;

    pub fn reset_hal() {
        unsafe {
            G_TIME = 0;
            G_CPWM_ORIG = 0;
            G_POLL_READS_BEFORE_TOGGLE = 0;
            G_CPWM_READ_COUNT = 0;
        }
    }

    pub fn set_time(t: u32) {
        unsafe {
            G_TIME = t;
        }
    }

    pub fn set_cpwm_behavior(cpwm_orig: u8, poll_reads_before_toggle: u32) {
        unsafe {
            G_CPWM_ORIG = cpwm_orig;
            G_POLL_READS_BEFORE_TOGGLE = poll_reads_before_toggle;
            G_CPWM_READ_COUNT = 0;
        }
    }

    pub fn get_cpwm(adapter: Padapter) -> u8 {
        let a = adapter as *mut HostAdapter;
        unsafe { (*a).pwrctrlpriv.cpwm }
    }

    pub fn get_cpwm_tog(adapter: Padapter) -> u8 {
        let a = adapter as *mut HostAdapter;
        unsafe { (*a).pwrctrlpriv.cpwm_tog }
    }

    fn current_time() -> u32 {
        unsafe { G_TIME }
    }

    fn passing_time_ms(start: u32) -> u32 {
        unsafe { G_TIME - start }
    }

    fn msleep_os(ms: i32) {
        unsafe {
            G_TIME += ms as u32;
        }
    }

    pub fn hal_get_cpwm(_adapter: Padapter, out: *mut u8) {
        unsafe {
            G_CPWM_READ_COUNT += 1;
            let mut cpwm_now = G_CPWM_ORIG;
            if G_CPWM_READ_COUNT > G_POLL_READS_BEFORE_TOGGLE {
                cpwm_now ^= PS_TOGGLE;
            }
            *out = cpwm_now;
        }
    }

    pub fn hal_set_rpwm(_adapter: Padapter, _rpwm: u8) {}

    pub fn cannot_run(adapter: Padapter) -> bool {
        let a = adapter as *mut HostAdapter;
        unsafe { (*a).b_surprise_removed != 0 || (*a).b_driver_stopped != 0 }
    }

    pub fn cpwm_polling_loop(adapter: Padapter, mut rpwm: u8, mut cpwm_orig: u8) -> i32 {
        let a = adapter as *mut HostAdapter;
        let pwr = unsafe { &mut (*a).pwrctrlpriv };
        let mut rst = _FAIL;
        let mut cpwm_now = 0u8;

        pwr.rpwm_retry = 0;
        loop {
            let start_time = current_time();
            while {
                msleep_os(1);
                hal_get_cpwm(adapter, &mut cpwm_now);
                if (cpwm_orig ^ cpwm_now) & 0x80 != 0 {
                    pwr.cpwm = PS_STATE_S4;
                    pwr.cpwm_tog = cpwm_now & PS_TOGGLE;
                    rst = _SUCCESS;
                    false
                } else {
                    passing_time_ms(start_time) < LPS_CPWM_TIMEOUT_MS && !cannot_run(adapter)
                }
            } {}
            if rst == _SUCCESS {
                break;
            }
            cpwm_orig = cpwm_now;
            rpwm &= !PS_TOGGLE;
            rpwm |= pwr.tog;
            hal_set_rpwm(adapter, rpwm);
            pwr.tog = pwr.tog.wrapping_add(0x80);
            let retry = pwr.rpwm_retry as u32;
            pwr.rpwm_retry = (retry + 1) as u8;
            if retry >= LPS_RPWM_RETRY_CNT || cannot_run(adapter) {
                break;
            }
        }
        rst
    }
}

#[no_mangle]
pub extern "C" fn rtw_cpwm_polling(adapter: Padapter, rpwm: u8, cpwm_orig: u8) -> u8 {
    host_hal::cpwm_polling_loop(adapter, rpwm, cpwm_orig) as u8
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_rpwm_reset_hal() {
    host_hal::reset_hal();
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_rpwm_set_time(t: u32) {
    host_hal::set_time(t);
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_rpwm_set_cpwm_behavior(
    cpwm_orig: u8,
    poll_reads_before_toggle: u32,
) {
    host_hal::set_cpwm_behavior(cpwm_orig, poll_reads_before_toggle);
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_rpwm_get_cpwm(adapter: Padapter) -> u8 {
    host_hal::get_cpwm(adapter)
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_rpwm_get_cpwm_tog(adapter: Padapter) -> u8 {
    host_hal::get_cpwm_tog(adapter)
}
