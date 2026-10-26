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
fn adapter_to_pwrctl(a: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*a).pwrctrlpriv }
}

#[cfg(host_pwrctrl_ps_processor_test)]
mod host_trace {
    use super::*;

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

    pub fn reset() {
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

    pub fn ips_enter() {
        unsafe {
            G_TRACE.ips_enter += 1;
        }
    }

    pub fn set_pwr_timer() {
        unsafe {
            G_TRACE.pwr_timer_set += 1;
        }
    }

    pub fn unassoc_idle() -> u8 {
        unsafe { G_UNASSOC_IDLE }
    }

    pub fn trace_ptr() -> *mut HostPsProcTrace {
        unsafe { &mut G_TRACE }
    }
}

#[cfg(host_pwrctrl_ps_processor_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_reset_trace() {
    host_trace::reset();
}

#[cfg(host_pwrctrl_ps_processor_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_set_unassoc_idle(idle: u8) {
    host_trace::set_unassoc_idle(idle);
}

#[cfg(host_pwrctrl_ps_processor_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_get_trace() -> *mut host_trace::HostPsProcTrace {
    host_trace::trace_ptr()
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
fn ips_enter(_padapter: Padapter) {
    host_trace::ips_enter();
}

#[cfg(host_pwrctrl_ps_processor_test)]
fn unassociated_idle(_padapter: Padapter) -> u8 {
    host_trace::unassoc_idle()
}

#[cfg(host_pwrctrl_ps_processor_test)]
fn set_pwr_state_check_timer(_pwr: *mut PwrctrlPriv) {
    host_trace::set_pwr_timer();
}

#[cfg(not(host_pwrctrl_ps_processor_test))]
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
}

#[no_mangle]
pub extern "C" fn rtw_ps_processor(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }

    #[cfg(host_pwrctrl_ps_processor_test)]
    let pwr = adapter_to_pwrctl(padapter);

    #[cfg(host_pwrctrl_ps_processor_test)]
    {
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
            ips_enter(padapter);
        }
        set_pwr_state_check_timer(pwr);
        unsafe {
            (*pwr).ps_processing = _FALSE;
        }
        return;
    }

    #[cfg(not(host_pwrctrl_ps_processor_test))]
    unsafe {
        rtw_rust_ps_proc_enter_pwrlock(padapter);
        let ps_deny = rtw_ps_deny_get(padapter);
        rtw_rust_ps_proc_exit_pwrlock(padapter);
        if ps_deny != 0 {
            goto_exit(padapter);
            return;
        }
        if rtw_rust_ps_proc_get_b_in_suspend(padapter) == _TRUE {
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

#[cfg(not(host_pwrctrl_ps_processor_test))]
fn goto_exit(padapter: Padapter) {
    unsafe {
        rtw_rust_ps_proc_set_pwr_timer(padapter);
        rtw_rust_ps_proc_set_ps_processing(padapter, _FALSE);
    }
}
