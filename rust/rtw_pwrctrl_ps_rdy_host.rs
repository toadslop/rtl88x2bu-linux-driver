// SPDX-License-Identifier: GPL-2.0
//! Host L2 stubs for `rtw_pwrctrl_ps_rdy.rs` (PR8a: harness setters; PR8b adds FFI).

#![allow(non_snake_case)]

use crate::_TRUE;

static mut G_TIME: u32 = 0;
static mut G_SCAN_DENY: u8 = 0;
static mut G_ROCH: u8 = 0;
static mut G_PWR_MGMT: u8 = _TRUE;

pub(crate) fn host_time() -> u32 {
    unsafe { G_TIME }
}

pub(crate) fn host_scan_deny() -> u8 {
    unsafe { G_SCAN_DENY }
}

pub(crate) fn host_roch() -> u8 {
    unsafe { G_ROCH }
}

pub(crate) fn host_pwr_mgmt() -> u8 {
    unsafe { G_PWR_MGMT }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_time(t: u32) {
    unsafe {
        G_TIME = t;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_scan_deny(v: u8) {
    unsafe {
        G_SCAN_DENY = v;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_roch(v: u8) {
    unsafe {
        G_ROCH = v;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_pwr_mgmt(v: u8) {
    unsafe {
        G_PWR_MGMT = v;
    }
}
