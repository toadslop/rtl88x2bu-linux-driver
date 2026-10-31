// SPDX-License-Identifier: GPL-2.0
//! W3-188 — host L2 stubs for `rtw_pwrctrl_cpwm_int.rs`.

#![allow(dead_code, non_snake_case)]

use super::*;

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

fn leave_ready(a: Padapter) -> bool {
    let ad = adapter(a);
    let p = unsafe { &(*ad).pwrctrlpriv };
    unsafe {
        (*ad).b_surprise_removed != 0
            || (*ad).b_hw_init_completed == 0
            || (*ad).b_driver_stopped != 0
            || p.pwr_mode == PS_MODE_ACTIVE
    }
}

pub fn lps_leave_check(a: Padapter) {
    let start_time = unsafe { G_TIME };
    loop {
        let ready = leave_ready(a);
        if ready {
            break;
        }
        if unsafe { G_TIME - start_time } > 100 {
            break;
        }
        unsafe {
            G_TIME += 1;
        }
    }
}

pub fn cpwm_int_hdl(a: Padapter, report: *const ReportPwrStateParm) {
    if a.is_null() {
        return;
    }
    let ad = adapter(a);
    if unsafe { (*ad).b_surprise_removed != 0 || (*ad).b_driver_stopped != 0 } {
        return;
    }
    unsafe {
        let p = &mut (*ad).pwrctrlpriv;
        let st = (*report).state;
        p.cpwm = ps_state(st);
        p.cpwm_tog = st & PS_TOGGLE;
        if p.cpwm >= PS_STATE_S2 {
            if p.alives & CMD_ALIVE != 0 {
                (*ad).cmd_sem_up += 1;
            }
            if p.alives & XMIT_ALIVE != 0 {
                (*ad).xmit_sem_up += 1;
            }
        }
    }
}
