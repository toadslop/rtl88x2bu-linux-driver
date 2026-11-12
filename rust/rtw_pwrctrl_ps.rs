// SPDX-License-Identifier: GPL-2.0
//! W3-173 — `rtw_ps_processor` IPS loop from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_ps_processor_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_ps_processor_test))]
use core::ffi::c_void;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const IPS_NONE: u8 = 0;
const RF_ON: u8 = 0;
const RF_OFF: u8 = 2;

pub(crate) type Padapter = *mut c_void;

#[cfg(host_pwrctrl_ps_processor_test)]
#[path = "rtw_pwrctrl_ps_host.rs"]
mod host;

extern "C" {
    fn rtw_ps_deny_get(padapter: Padapter) -> u32;
    fn ips_enter(padapter: Padapter);
    fn rtw_rust_ps_proc_enter_pwrlock(padapter: Padapter);
    fn rtw_rust_ps_proc_exit_pwrlock(padapter: Padapter);
    fn rtw_rust_ps_proc_unassociated_idle(padapter: Padapter) -> u8;
    fn rtw_rust_ps_proc_set_pwr_timer(padapter: Padapter);
    fn rtw_rust_ps_proc_get_b_in_suspend(padapter: Padapter) -> u8;
    fn rtw_rust_ps_proc_set_ps_processing(padapter: Padapter, v: u8);
    fn rtw_rust_ps_proc_get_ips_mode_req(padapter: Padapter) -> u8;
    fn rtw_rust_ps_proc_get_rf_pwrstate(padapter: Padapter) -> u8;
    fn rtw_rust_ps_proc_set_change_rfpwrstate(padapter: Padapter, v: u8);
    fn rtw_rust_ps_proc_get_pwr_state_check_cnts(padapter: Padapter) -> u32;
    fn rtw_rust_ps_proc_report_ps_deny(padapter: Padapter, ps_deny: u32);
    fn rtw_rust_ps_proc_b_in_suspend(padapter: Padapter);
}

fn ps_processor_core(padapter: Padapter) {
    unsafe {
        rtw_rust_ps_proc_enter_pwrlock(padapter);
        let ps_deny = rtw_ps_deny_get(padapter);
        rtw_rust_ps_proc_exit_pwrlock(padapter);
        if ps_deny != 0 {
            rtw_rust_ps_proc_report_ps_deny(padapter, ps_deny);
            goto_exit(padapter);
            return;
        }
        if rtw_rust_ps_proc_get_b_in_suspend(padapter) == _TRUE {
            rtw_rust_ps_proc_b_in_suspend(padapter);
            return;
        }
        rtw_rust_ps_proc_set_ps_processing(padapter, _TRUE);

        if rtw_rust_ps_proc_get_ips_mode_req(padapter) == IPS_NONE {
            goto_exit(padapter);
            return;
        }
        if rtw_rust_ps_proc_unassociated_idle(padapter) == _FALSE {
            goto_exit(padapter);
            return;
        }
        let rf = rtw_rust_ps_proc_get_rf_pwrstate(padapter);
        let cnt = rtw_rust_ps_proc_get_pwr_state_check_cnts(padapter);
        if rf == RF_ON && (cnt % 4) == 0 {
            rtw_rust_ps_proc_set_change_rfpwrstate(padapter, RF_OFF);
            ips_enter(padapter);
        }
        goto_exit(padapter);
    }
}

fn goto_exit(padapter: Padapter) {
    unsafe {
        rtw_rust_ps_proc_set_pwr_timer(padapter);
        rtw_rust_ps_proc_set_ps_processing(padapter, _FALSE);
    }
}

#[no_mangle]
pub extern "C" fn rtw_ps_processor(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    ps_processor_core(padapter);
}
