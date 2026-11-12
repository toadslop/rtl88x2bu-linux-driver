// SPDX-License-Identifier: GPL-2.0
//! Host L2 stubs for `rtw_pwrctrl_cpwm_int.rs` (shared cores).

#![allow(non_snake_case, non_camel_case_types)]

use crate::{
    cpwm_int_hdl_core, lps_leave_check_core, ps_state, HostAdapter, Padapter, ReportPwrStateParm,
    Systime, PS_MODE_ACTIVE, PS_TOGGLE,
};
use std::ffi::c_ulong;

static mut G_TIME: Systime = 0;

pub fn reset() {
    unsafe {
        G_TIME = 0;
    }
}
pub fn set_time(t: Systime) {
    unsafe {
        G_TIME = t;
    }
}

fn adapter(a: Padapter) -> *mut HostAdapter {
    a as *mut HostAdapter
}

pub fn get_cpwm(a: Padapter) -> u8 {
    unsafe { (*adapter(a)).pwrctrlpriv.cpwm }
}
pub fn get_cpwm_tog(a: Padapter) -> u8 {
    unsafe { (*adapter(a)).pwrctrlpriv.cpwm_tog }
}
pub fn cmd_sem_up(a: Padapter) -> u32 {
    unsafe { (*adapter(a)).cmd_sem_up }
}
pub fn xmit_sem_up(a: Padapter) -> u32 {
    unsafe { (*adapter(a)).xmit_sem_up }
}

pub fn lps_leave_check(a: Padapter) {
    fn ready(ad: *mut HostAdapter) -> bool {
        let p = unsafe { &(*ad).pwrctrlpriv };
        unsafe {
            (*ad).b_surprise_removed != 0
                || (*ad).b_hw_init_completed == 0
                || (*ad).b_driver_stopped != 0
                || p.pwr_mode == PS_MODE_ACTIVE
        }
    }
    lps_leave_check_core(
        |_| {},
        |_| {},
        || {},
        || unsafe { G_TIME },
        |start| unsafe {
            let d = G_TIME.wrapping_sub(start);
            if d > c_ulong::from(u32::MAX) {
                u32::MAX
            } else {
                d as u32
            }
        },
        |ms| unsafe { G_TIME = G_TIME.wrapping_add(ms as c_ulong) },
        |pad| ready(adapter(pad)),
        || {},
        a,
    );
}

pub fn cpwm_int_hdl(a: Padapter, report: *const ReportPwrStateParm) {
    cpwm_int_hdl_core(
        |pad| {
            let ad = adapter(pad);
            unsafe { (*ad).b_surprise_removed != 0 || (*ad).b_driver_stopped != 0 }
        },
        |_| {},
        |_| {},
        |_| false,
        |pad, st| {
            let ad = adapter(pad);
            unsafe {
                let p = &mut (*ad).pwrctrlpriv;
                p.cpwm = ps_state(st);
                p.cpwm_tog = st & PS_TOGGLE;
            }
        },
        |pad| unsafe { (*adapter(pad)).pwrctrlpriv.alives },
        |pad| unsafe { (*adapter(pad)).cmd_sem_up += 1 },
        |pad| unsafe { (*adapter(pad)).xmit_sem_up += 1 },
        a,
        report,
    );
}
