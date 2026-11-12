// SPDX-License-Identifier: GPL-2.0
//! Host L2 trace hooks for `rtw_pwrctrl_ps.rs` (PR7); FFI stubs land in PR8.

#![allow(non_snake_case, non_camel_case_types)]

use crate::_TRUE;

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

pub fn reset_trace() {
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

pub fn ips_enter_trace() {
    unsafe {
        G_TRACE.ips_enter += 1;
    }
}

pub fn pwr_timer_tick() {
    unsafe {
        G_TRACE.pwr_timer_set += 1;
    }
}

pub fn unassoc_idle_flag() -> u8 {
    unsafe { G_UNASSOC_IDLE }
}

pub fn trace_ptr() -> *mut HostPsProcTrace {
    unsafe { std::ptr::addr_of_mut!(G_TRACE) }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_reset_trace() {
    reset_trace();
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_set_unassoc_idle(idle: u8) {
    set_unassoc_idle(idle);
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_proc_get_trace() -> *mut HostPsProcTrace {
    trace_ptr()
}
