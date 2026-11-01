// SPDX-License-Identifier: GPL-2.0
//! Host stubs for W3-194 early suspend / resume workqueue tests.

use super::{HostEarlySuspend, HostPwrctrlPriv, _FALSE, _TRUE};

static mut G_TRACE: super::HostResumeTrace = super::HostResumeTrace {
    register_calls: 0,
    unregister_calls: 0,
    resume_process_calls: 0,
    queue_work_calls: 0,
    lock_suspend_calls: 0,
    unlock_suspend_calls: 0,
};

pub fn reset_trace() {
    unsafe {
        G_TRACE = super::HostResumeTrace {
            register_calls: 0,
            unregister_calls: 0,
            resume_process_calls: 0,
            queue_work_calls: 0,
            lock_suspend_calls: 0,
            unlock_suspend_calls: 0,
        };
    }
}

pub fn trace_ptr() -> *mut super::HostResumeTrace {
    unsafe { core::ptr::addr_of_mut!(G_TRACE) }
}

fn container_of_early(h: *mut HostEarlySuspend) -> *mut HostPwrctrlPriv {
    unsafe {
        let off = core::mem::offset_of!(HostPwrctrlPriv, early_suspend);
        (h as *mut u8).sub(off) as *mut HostPwrctrlPriv
    }
}

extern "C" fn early_suspend_cb(h: *mut HostEarlySuspend) {
    let p = unsafe { &mut *container_of_early(h) };
    p.do_late_resume = _FALSE;
}

extern "C" fn late_resume_cb(h: *mut HostEarlySuspend) {
    let p = unsafe { &mut *container_of_early(h) };
    if p.do_late_resume != 0 {
        p.do_late_resume = _FALSE;
        unsafe {
            G_TRACE.resume_process_calls += 1;
        }
    }
}

#[cfg(early_suspend)]
pub fn register_early_suspend(p: *mut HostPwrctrlPriv) {
    unsafe {
        G_TRACE.register_calls += 1;
        (*p).early_suspend.level = 30;
        (*p).early_suspend.suspend = Some(early_suspend_cb);
        (*p).early_suspend.resume = Some(late_resume_cb);
    }
}

#[cfg(android_power)]
pub fn register_early_suspend_android(p: *mut HostPwrctrlPriv) {
    register_early_suspend(p);
}

pub fn unregister_early_suspend(p: *mut HostPwrctrlPriv) {
    unsafe {
        G_TRACE.unregister_calls += 1;
        (*p).do_late_resume = _FALSE;
        (*p).early_suspend.suspend = None;
        (*p).early_suspend.resume = None;
    }
}

pub fn is_earlysuspend_registered(p: *mut HostPwrctrlPriv) -> u8 {
    unsafe {
        if (*p).early_suspend.suspend.is_some() {
            _TRUE
        } else {
            _FALSE
        }
    }
}

pub fn is_do_late_resume(p: *mut HostPwrctrlPriv) -> u8 {
    unsafe {
        if (*p).do_late_resume != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }
}

pub fn set_do_late_resume(p: *mut HostPwrctrlPriv, enable: u8) {
    unsafe {
        (*p).do_late_resume = enable;
    }
}

#[cfg(resume_wq)]
pub fn resume_in_workqueue(p: *mut HostPwrctrlPriv) {
    unsafe {
        G_TRACE.lock_suspend_calls += 1;
        if !(*p).rtw_workqueue.is_null() {
            G_TRACE.queue_work_calls += 1;
        }
    }
}
