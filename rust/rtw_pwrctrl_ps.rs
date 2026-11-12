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

#[cfg(host_pwrctrl_ps_processor_test)]
type Padapter = *mut HostAdapter;

#[cfg(not(host_pwrctrl_ps_processor_test))]
type Padapter = *mut c_void;

#[cfg(host_pwrctrl_ps_processor_test)]
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

#[cfg(host_pwrctrl_ps_processor_test)]
#[repr(C)]
pub struct MlmePriv {
    pub fw_state: u32,
}

#[cfg(host_pwrctrl_ps_processor_test)]
#[repr(C)]
pub struct HostAdapter {
    pub pwrctrlpriv: PwrctrlPriv,
    pub mlmepriv: MlmePriv,
}

#[cfg(host_pwrctrl_ps_processor_test)]
#[path = "rtw_pwrctrl_ps_host.rs"]
mod host;

#[cfg(host_pwrctrl_ps_processor_test)]
fn adapter_to_pwrctl(a: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*a).pwrctrlpriv }
}

#[cfg(host_pwrctrl_ps_processor_test)]
fn enter_pwrlock(_pwr: *mut PwrctrlPriv) {}

#[cfg(host_pwrctrl_ps_processor_test)]
fn exit_pwrlock(_pwr: *mut PwrctrlPriv) {}

#[cfg(host_pwrctrl_ps_processor_test)]
fn ps_deny_get_locked(pwr: *mut PwrctrlPriv) -> u32 {
    unsafe { (*pwr).ps_deny }
}

#[cfg(host_pwrctrl_ps_processor_test)]
fn ips_enter_local(_padapter: Padapter) {
    host::ips_enter_trace();
}

#[cfg(host_pwrctrl_ps_processor_test)]
fn unassociated_idle(_padapter: Padapter) -> u8 {
    host::unassoc_idle_flag()
}

#[cfg(host_pwrctrl_ps_processor_test)]
fn set_pwr_state_check_timer(_pwr: *mut PwrctrlPriv) {
    host::pwr_timer_tick();
}

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

    #[cfg(host_pwrctrl_ps_processor_test)]
    {
        let pwr = adapter_to_pwrctl(padapter);
        enter_pwrlock(pwr);
        let ps_deny = ps_deny_get_locked(pwr);
        exit_pwrlock(pwr);
        if ps_deny != 0 {
            set_pwr_state_check_timer(pwr);
            unsafe {
                (*pwr).ps_processing = _FALSE;
            }
            return;
        }
        if unsafe { (*pwr).bInSuspend } == _TRUE {
            return;
        }
        unsafe {
            (*pwr).ps_processing = _TRUE;
        }
        if unsafe { (*pwr).ips_mode_req } == IPS_NONE {
            set_pwr_state_check_timer(pwr);
            unsafe {
                (*pwr).ps_processing = _FALSE;
            }
            return;
        }
        if unassociated_idle(padapter) == _FALSE {
            set_pwr_state_check_timer(pwr);
            unsafe {
                (*pwr).ps_processing = _FALSE;
            }
            return;
        }
        let cnt = unsafe { (*pwr).pwr_state_check_cnts };
        let rf = unsafe { (*pwr).rf_pwrstate };
        if rf == RF_ON && (cnt % 4) == 0 {
            unsafe {
                (*pwr).change_rfpwrstate = RF_OFF;
            }
            ips_enter_local(padapter);
        }
        set_pwr_state_check_timer(pwr);
        unsafe {
            (*pwr).ps_processing = _FALSE;
        }
        return;
    }

    #[cfg(not(host_pwrctrl_ps_processor_test))]
    ps_processor_core(padapter);
}
