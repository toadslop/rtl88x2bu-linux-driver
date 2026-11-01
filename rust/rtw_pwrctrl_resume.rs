// SPDX-License-Identifier: GPL-2.0
//! W3-194 — early suspend registration + resume workqueue from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_resume_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_resume_test))]
use core::ffi::c_void;

pub const _TRUE: u8 = 1;
pub const _FALSE: u8 = 0;

type Padapter = *mut c_void;
type PwrctrlPriv = *mut c_void;

#[repr(C)]
pub struct HostEarlySuspend {
    pub suspend: Option<extern "C" fn(*mut HostEarlySuspend)>,
    pub resume: Option<extern "C" fn(*mut HostEarlySuspend)>,
    pub level: i32,
}

#[repr(C)]
pub struct HostPwrctrlPriv {
    pub early_suspend: HostEarlySuspend,
    pub do_late_resume: u8,
    pub resume_work: *mut c_void,
    pub rtw_workqueue: *mut c_void,
}

#[repr(C)]
pub struct HostAdapter {
    pub pwrctrlpriv: HostPwrctrlPriv,
}

#[repr(C)]
pub struct HostResumeTrace {
    pub register_calls: u32,
    pub unregister_calls: u32,
    pub resume_process_calls: u32,
    pub queue_work_calls: u32,
    pub lock_suspend_calls: u32,
    pub unlock_suspend_calls: u32,
}

#[cfg(host_pwrctrl_resume_test)]
#[path = "rtw_pwrctrl_resume_host.rs"]
mod host;

#[cfg(not(host_pwrctrl_resume_test))]
extern "C" {
    fn rtw_rust_resume_pwr_from_es(h: *mut c_void) -> PwrctrlPriv;
    fn rtw_rust_resume_pwr_from_work(work: *mut c_void) -> PwrctrlPriv;
    fn rtw_rust_resume_get_es_suspend(pwrpriv: PwrctrlPriv) -> *mut c_void;
    fn rtw_rust_resume_set_es_suspend(pwrpriv: PwrctrlPriv, cb: *mut c_void);
    fn rtw_rust_resume_set_es_resume(pwrpriv: PwrctrlPriv, cb: *mut c_void);
    fn rtw_rust_resume_set_es_level(pwrpriv: PwrctrlPriv, level: i32);
    fn rtw_rust_resume_register_es_has(pwrpriv: PwrctrlPriv);
    fn rtw_rust_resume_unregister_es_has(pwrpriv: PwrctrlPriv);
    fn rtw_rust_resume_register_es_android(pwrpriv: PwrctrlPriv);
    fn rtw_rust_resume_unregister_es_android(pwrpriv: PwrctrlPriv);
    fn rtw_rust_resume_get_do_late(pwrpriv: PwrctrlPriv) -> u8;
    fn rtw_rust_resume_set_do_late(pwrpriv: PwrctrlPriv, v: u8);
    fn rtw_rust_resume_queue_work(pwrpriv: PwrctrlPriv);
    fn rtw_rust_resume_lock_suspend();
    fn rtw_rust_resume_unlock_suspend();
    fn rtw_rust_resume_process_adapter(padapter: Padapter) -> i32;
    fn rtw_rust_resume_primary_adapter(pwrpriv: PwrctrlPriv) -> Padapter;
    fn rtw_rust_resume_register_level() -> i32;
    fn rtw_rust_resume_android_late_hci_ok() -> u8;
}

#[cfg(not(host_pwrctrl_resume_test))]
#[no_mangle]
pub extern "C" fn rtw_rust_es_early_suspend(h: *mut c_void) {
    unsafe {
        let p = rtw_rust_resume_pwr_from_es(h);
        rtw_rust_resume_set_do_late(p, _FALSE);
    }
}

#[cfg(not(host_pwrctrl_resume_test))]
#[no_mangle]
pub extern "C" fn rtw_rust_es_late_resume(h: *mut c_void) {
    unsafe {
        let p = rtw_rust_resume_pwr_from_es(h);
        if rtw_rust_resume_get_do_late(p) == 0 {
            return;
        }
        if rtw_rust_resume_android_late_hci_ok() == 0 {
            return;
        }
        rtw_rust_resume_set_do_late(p, _FALSE);
        let ad = rtw_rust_resume_primary_adapter(p);
        rtw_rust_resume_process_adapter(ad);
    }
}

#[cfg(not(host_pwrctrl_resume_test))]
#[no_mangle]
pub extern "C" fn resume_workitem_callback(work: *mut c_void) {
    unsafe {
        let p = rtw_rust_resume_pwr_from_work(work);
        let ad = rtw_rust_resume_primary_adapter(p);
        rtw_rust_resume_process_adapter(ad);
        rtw_rust_resume_unlock_suspend();
    }
}

#[cfg(any(early_suspend, android_power))]
#[no_mangle]
pub extern "C" fn rtw_is_earlysuspend_registered(pwrpriv: PwrctrlPriv) -> u8 {
    #[cfg(host_pwrctrl_resume_test)]
    {
        return host::is_earlysuspend_registered(pwrpriv as *mut HostPwrctrlPriv);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        if !rtw_rust_resume_get_es_suspend(pwrpriv).is_null() {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[cfg(any(early_suspend, android_power))]
#[no_mangle]
pub extern "C" fn rtw_is_do_late_resume(pwrpriv: PwrctrlPriv) -> u8 {
    #[cfg(host_pwrctrl_resume_test)]
    {
        return host::is_do_late_resume(pwrpriv as *mut HostPwrctrlPriv);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        if rtw_rust_resume_get_do_late(pwrpriv) != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[cfg(any(early_suspend, android_power))]
#[no_mangle]
pub extern "C" fn rtw_set_do_late_resume(pwrpriv: PwrctrlPriv, enable: u8) {
    #[cfg(host_pwrctrl_resume_test)]
    {
        host::set_do_late_resume(pwrpriv as *mut HostPwrctrlPriv, enable);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        rtw_rust_resume_set_do_late(pwrpriv, enable);
    }
}

#[cfg(early_suspend)]
#[no_mangle]
pub extern "C" fn rtw_register_early_suspend(pwrpriv: PwrctrlPriv) {
    #[cfg(host_pwrctrl_resume_test)]
    {
        host::register_early_suspend(pwrpriv as *mut HostPwrctrlPriv);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        rtw_rust_resume_set_es_level(pwrpriv, rtw_rust_resume_register_level());
        rtw_rust_resume_set_es_suspend(pwrpriv, rtw_rust_es_early_suspend as *mut c_void);
        rtw_rust_resume_set_es_resume(pwrpriv, rtw_rust_es_late_resume as *mut c_void);
        rtw_rust_resume_register_es_has(pwrpriv);
    }
}

#[cfg(early_suspend)]
#[no_mangle]
pub extern "C" fn rtw_unregister_early_suspend(pwrpriv: PwrctrlPriv) {
    #[cfg(host_pwrctrl_resume_test)]
    {
        host::unregister_early_suspend(pwrpriv as *mut HostPwrctrlPriv);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        rtw_rust_resume_set_do_late(pwrpriv, _FALSE);
        if !rtw_rust_resume_get_es_suspend(pwrpriv).is_null() {
            rtw_rust_resume_unregister_es_has(pwrpriv);
        }
        rtw_rust_resume_set_es_suspend(pwrpriv, core::ptr::null_mut());
        rtw_rust_resume_set_es_resume(pwrpriv, core::ptr::null_mut());
    }
}

#[cfg(all(android_power, not(early_suspend)))]
#[no_mangle]
pub extern "C" fn rtw_register_early_suspend(pwrpriv: PwrctrlPriv) {
    #[cfg(host_pwrctrl_resume_test)]
    {
        host::register_early_suspend_android(pwrpriv as *mut HostPwrctrlPriv);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        rtw_rust_resume_set_es_level(pwrpriv, rtw_rust_resume_register_level());
        rtw_rust_resume_set_es_suspend(pwrpriv, rtw_rust_es_early_suspend as *mut c_void);
        rtw_rust_resume_set_es_resume(pwrpriv, rtw_rust_es_late_resume as *mut c_void);
        rtw_rust_resume_register_es_android(pwrpriv);
    }
}

#[cfg(all(android_power, not(early_suspend)))]
#[no_mangle]
pub extern "C" fn rtw_unregister_early_suspend(pwrpriv: PwrctrlPriv) {
    #[cfg(host_pwrctrl_resume_test)]
    {
        host::unregister_early_suspend(pwrpriv as *mut HostPwrctrlPriv);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        rtw_rust_resume_set_do_late(pwrpriv, _FALSE);
        if !rtw_rust_resume_get_es_suspend(pwrpriv).is_null() {
            rtw_rust_resume_unregister_es_android(pwrpriv);
        }
        rtw_rust_resume_set_es_suspend(pwrpriv, core::ptr::null_mut());
        rtw_rust_resume_set_es_resume(pwrpriv, core::ptr::null_mut());
    }
}

#[cfg(resume_wq)]
#[no_mangle]
pub extern "C" fn rtw_resume_in_workqueue(pwrpriv: PwrctrlPriv) {
    #[cfg(host_pwrctrl_resume_test)]
    {
        host::resume_in_workqueue(pwrpriv as *mut HostPwrctrlPriv);
    }
    #[cfg(not(host_pwrctrl_resume_test))]
    unsafe {
        rtw_rust_resume_lock_suspend();
        rtw_rust_resume_queue_work(pwrpriv);
    }
}

#[cfg(host_pwrctrl_resume_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_resume_reset_trace() {
    host::reset_trace();
}

#[cfg(host_pwrctrl_resume_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_resume_get_trace() -> *mut HostResumeTrace {
    host::trace_ptr()
}
