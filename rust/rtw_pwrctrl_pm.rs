// SPDX-License-Identifier: GPL-2.0
//! W3-184 / W3-196 — PM ioctl setters from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_pm_test)]
use std::ffi::{c_int, c_void};

#[cfg(not(host_pwrctrl_pm_test))]
use core::ffi::{c_int, c_void};

type Padapter = *mut c_void;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;

#[cfg(host_pwrctrl_pm_test)]
const PWR_WAKEUP_OK: c_int = 0;
#[cfg(host_pwrctrl_pm_test)]
const PWR_WAKEUP_FAIL: c_int = -1;

#[cfg(not(host_pwrctrl_pm_test))]
const PWR_WAKEUP_OK: c_int = 1;
#[cfg(not(host_pwrctrl_pm_test))]
const PWR_WAKEUP_FAIL: c_int = 0;

const PS_MODE_ACTIVE: u8 = 0;
const PS_MODE_NUM: u8 = 9;

const IPS_NONE: u8 = 0;
const IPS_NORMAL: u8 = 1;
const IPS_LEVEL_2: u8 = 2;

const LPS_LEVEL_MAX: u8 = 3;

const EINVAL: c_int = -22;
const EFAULT: c_int = -14;

#[cfg(host_pwrctrl_pm_test)]
#[repr(C)]
struct PwrctrlPriv {
    ips_mode_req: u8,
    power_mgnt: u8,
    bLeisurePs: u8,
    LpsIdleCount: u8,
    lps_level: u8,
    wowlan_power_mgmt: u8,
    wowlan_lps_level: u8,
    wowlan_lps_1t1r: u8,
}

#[cfg(host_pwrctrl_pm_test)]
#[repr(C)]
struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
    bSurpriseRemoved: u8,
}

#[cfg(host_pwrctrl_pm_test)]
static mut G_TRACE: HostPmTrace = HostPmTrace {
    leave_all_ps: 0,
    ips_mode_req_calls: 0,
    last_ips_mode_req: 0,
    pwr_wakeup_calls: 0,
};

#[cfg(host_pwrctrl_pm_test)]
static mut G_PWR_WAKEUP_RESULT: c_int = PWR_WAKEUP_OK;

#[cfg(host_pwrctrl_pm_test)]
#[repr(C)]
pub struct HostPmTrace {
    pub leave_all_ps: i32,
    pub ips_mode_req_calls: i32,
    pub last_ips_mode_req: u8,
    pub pwr_wakeup_calls: i32,
}

#[cfg(host_pwrctrl_pm_test)]
fn pwrpriv(adapter: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*(adapter as *mut HostAdapter)).pwrctrlpriv }
}

#[cfg(host_pwrctrl_pm_test)]
fn leave_all_ps(adapter: Padapter) {
    unsafe {
        G_TRACE.leave_all_ps += 1;
    }
    let _ = adapter;
}

#[cfg(host_pwrctrl_pm_test)]
fn ips_mode_req(adapter: Padapter, mode: u8) {
    unsafe {
        (*pwrpriv(adapter)).ips_mode_req = mode;
        G_TRACE.ips_mode_req_calls += 1;
        G_TRACE.last_ips_mode_req = mode;
    }
}

#[cfg(host_pwrctrl_pm_test)]
fn is_surprise_removed(adapter: Padapter) -> u8 {
    unsafe { (*(adapter as *mut HostAdapter)).bSurpriseRemoved }
}

#[cfg(host_pwrctrl_pm_test)]
fn pwr_wakeup(adapter: Padapter) -> c_int {
    let _ = adapter;
    unsafe {
        G_TRACE.pwr_wakeup_calls += 1;
        G_PWR_WAKEUP_RESULT
    }
}

#[cfg(not(host_pwrctrl_pm_test))]
extern "C" {
    fn LeaveAllPowerSaveMode(adapter: Padapter);
    fn rtw_rust_pm_ips_mode_req(adapter: Padapter, mode: u8);
    fn rtw_rust_pm_get_power_mgnt(adapter: Padapter) -> u8;
    fn rtw_rust_pm_set_power_mgnt(adapter: Padapter, mode: u8);
    fn rtw_rust_pm_set_lps_idle_count(adapter: Padapter, count: u8);
    fn rtw_rust_pm_get_lps_level(adapter: Padapter) -> u8;
    fn rtw_rust_pm_try_set_lps_level(adapter: Padapter, level: u8);
    fn rtw_rust_pm_is_surprise_removed(adapter: Padapter) -> u8;
    fn rtw_rust_pm_pwr_wakeup(adapter: Padapter) -> c_int;
    #[cfg(config_wowlan)]
    fn rtw_rust_pm_get_wowlan_power_mgmt(adapter: Padapter) -> u8;
    #[cfg(config_wowlan)]
    fn rtw_rust_pm_set_wowlan_power_mgmt(adapter: Padapter, mode: u8);
    #[cfg(config_wowlan)]
    fn rtw_rust_pm_get_wowlan_lps_level(adapter: Padapter) -> u8;
    #[cfg(config_wowlan)]
    fn rtw_rust_pm_set_wowlan_lps_level(adapter: Padapter, level: u8);
    #[cfg(all(config_wowlan, config_lps_1t1r))]
    fn rtw_rust_pm_set_wowlan_lps_1t1r(adapter: Padapter, en: u8);
}

#[cfg(not(host_pwrctrl_pm_test))]
fn leave_all_ps(adapter: Padapter) {
    unsafe {
        LeaveAllPowerSaveMode(adapter);
    }
}

#[cfg(not(host_pwrctrl_pm_test))]
fn ips_mode_req(adapter: Padapter, mode: u8) {
    unsafe {
        rtw_rust_pm_ips_mode_req(adapter, mode);
    }
}

#[cfg(not(host_pwrctrl_pm_test))]
fn get_power_mgnt(adapter: Padapter) -> u8 {
    unsafe { rtw_rust_pm_get_power_mgnt(adapter) }
}

#[cfg(not(host_pwrctrl_pm_test))]
fn set_power_mgnt(adapter: Padapter, mode: u8) {
    unsafe {
        rtw_rust_pm_set_power_mgnt(adapter, mode);
    }
}

#[cfg(not(host_pwrctrl_pm_test))]
fn set_lps_idle_count(adapter: Padapter, count: u8) {
    unsafe {
        rtw_rust_pm_set_lps_idle_count(adapter, count);
    }
}

#[cfg(not(host_pwrctrl_pm_test))]
fn try_set_lps_level(adapter: Padapter, level: u8) {
    unsafe {
        rtw_rust_pm_try_set_lps_level(adapter, level);
    }
}

#[cfg(not(host_pwrctrl_pm_test))]
fn is_surprise_removed(adapter: Padapter) -> u8 {
    unsafe { rtw_rust_pm_is_surprise_removed(adapter) }
}

#[cfg(not(host_pwrctrl_pm_test))]
fn pwr_wakeup(adapter: Padapter) -> c_int {
    unsafe { rtw_rust_pm_pwr_wakeup(adapter) }
}

#[cfg(host_pwrctrl_pm_test)]
fn get_power_mgnt(adapter: Padapter) -> u8 {
    unsafe { (*pwrpriv(adapter)).power_mgnt }
}

#[cfg(host_pwrctrl_pm_test)]
fn set_power_mgnt(adapter: Padapter, mode: u8) {
    unsafe {
        (*pwrpriv(adapter)).power_mgnt = mode;
        (*pwrpriv(adapter)).bLeisurePs = if mode != PS_MODE_ACTIVE {
            _TRUE
        } else {
            _FALSE
        };
    }
}

#[cfg(host_pwrctrl_pm_test)]
fn set_lps_idle_count(adapter: Padapter, count: u8) {
    unsafe {
        (*pwrpriv(adapter)).LpsIdleCount = count;
    }
}

#[cfg(host_pwrctrl_pm_test)]
fn try_set_lps_level(adapter: Padapter, level: u8) {
    unsafe {
        if (*pwrpriv(adapter)).lps_level != level {
            (*pwrpriv(adapter)).lps_level = level;
        }
    }
}

#[cfg(all(not(host_pwrctrl_pm_test), config_wowlan))]
fn get_wowlan_power_mgmt(adapter: Padapter) -> u8 {
    unsafe { rtw_rust_pm_get_wowlan_power_mgmt(adapter) }
}

#[cfg(all(not(host_pwrctrl_pm_test), config_wowlan))]
fn set_wowlan_power_mgmt(adapter: Padapter, mode: u8) {
    unsafe {
        rtw_rust_pm_set_wowlan_power_mgmt(adapter, mode);
    }
}

#[cfg(all(not(host_pwrctrl_pm_test), config_wowlan))]
fn set_wowlan_lps_level(adapter: Padapter, level: u8) {
    unsafe {
        rtw_rust_pm_set_wowlan_lps_level(adapter, level);
    }
}

#[cfg(all(not(host_pwrctrl_pm_test), config_wowlan, config_lps_1t1r))]
fn set_wowlan_lps_1t1r(adapter: Padapter, en: u8) {
    unsafe {
        rtw_rust_pm_set_wowlan_lps_1t1r(adapter, en);
    }
}

#[cfg(host_pwrctrl_pm_test)]
fn get_wowlan_power_mgmt(adapter: Padapter) -> u8 {
    unsafe { (*pwrpriv(adapter)).wowlan_power_mgmt }
}

#[cfg(host_pwrctrl_pm_test)]
fn set_wowlan_power_mgmt(adapter: Padapter, mode: u8) {
    unsafe {
        (*pwrpriv(adapter)).wowlan_power_mgmt = mode;
    }
}

#[cfg(host_pwrctrl_pm_test)]
fn set_wowlan_lps_level(adapter: Padapter, level: u8) {
    unsafe {
        (*pwrpriv(adapter)).wowlan_lps_level = level;
    }
}

#[cfg(host_pwrctrl_pm_test)]
fn set_wowlan_lps_1t1r(adapter: Padapter, en: u8) {
    unsafe {
        (*pwrpriv(adapter)).wowlan_lps_1t1r = if en != 0 { 1 } else { 0 };
    }
}

#[no_mangle]
pub extern "C" fn rtw_pm_set_lps(adapter: Padapter, mode: u8) -> c_int {
    if adapter.is_null() {
        return EINVAL;
    }
    if mode >= PS_MODE_NUM {
        return EINVAL;
    }
    if get_power_mgnt(adapter) != mode {
        if mode == PS_MODE_ACTIVE {
            leave_all_ps(adapter);
        } else {
            set_lps_idle_count(adapter, 2);
        }
        set_power_mgnt(adapter, mode);
    }
    0
}

#[no_mangle]
pub extern "C" fn rtw_pm_set_lps_level(adapter: Padapter, level: u8) -> c_int {
    if adapter.is_null() {
        return EINVAL;
    }
    if level >= LPS_LEVEL_MAX {
        return EINVAL;
    }
    try_set_lps_level(adapter, level);
    0
}

#[no_mangle]
pub extern "C" fn rtw_pm_set_ips(adapter: Padapter, mode: u8) -> c_int {
    if adapter.is_null() {
        return EINVAL;
    }
    if mode == IPS_NORMAL || mode == IPS_LEVEL_2 {
        ips_mode_req(adapter, mode);
        return 0;
    }
    if mode == IPS_NONE {
        ips_mode_req(adapter, mode);
        if is_surprise_removed(adapter) == _FALSE && pwr_wakeup(adapter) == PWR_WAKEUP_FAIL {
            return EFAULT;
        }
        return 0;
    }
    EINVAL
}

#[cfg(any(config_wowlan, host_pwrctrl_pm_test))]
#[no_mangle]
pub extern "C" fn rtw_pm_set_wow_lps(adapter: Padapter, mode: u8) -> c_int {
    if adapter.is_null() {
        return EINVAL;
    }
    if mode >= PS_MODE_NUM {
        return EINVAL;
    }
    if get_wowlan_power_mgmt(adapter) != mode {
        set_wowlan_power_mgmt(adapter, mode);
    }
    0
}

#[cfg(any(config_wowlan, host_pwrctrl_pm_test))]
#[no_mangle]
pub extern "C" fn rtw_pm_set_wow_lps_level(adapter: Padapter, level: u8) -> c_int {
    if adapter.is_null() {
        return EINVAL;
    }
    if level >= LPS_LEVEL_MAX {
        return EINVAL;
    }
    set_wowlan_lps_level(adapter, level);
    0
}

#[cfg(any(all(config_wowlan, config_lps_1t1r), host_pwrctrl_pm_test))]
#[no_mangle]
pub extern "C" fn rtw_pm_set_wow_lps_1t1r(adapter: Padapter, en: u8) -> c_int {
    if adapter.is_null() {
        return EINVAL;
    }
    set_wowlan_lps_1t1r(adapter, en);
    0
}

#[cfg(host_pwrctrl_pm_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_pm_reset_trace() {
    unsafe {
        G_TRACE.leave_all_ps = 0;
        G_TRACE.ips_mode_req_calls = 0;
        G_TRACE.last_ips_mode_req = 0;
        G_TRACE.pwr_wakeup_calls = 0;
    }
}

#[cfg(host_pwrctrl_pm_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_pm_get_trace() -> *mut HostPmTrace {
    unsafe { &mut G_TRACE }
}

#[cfg(host_pwrctrl_pm_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_pm_set_pwr_wakeup_result(result: c_int) {
    unsafe {
        G_PWR_WAKEUP_RESULT = result;
    }
}

#[cfg(host_pwrctrl_pm_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_pm_read_state(
    adapter: Padapter,
    out: *mut host_pwrctrl_pm::HostPwrctrlPmState,
) {
    if adapter.is_null() || out.is_null() {
        return;
    }
    unsafe {
        let p = pwrpriv(adapter);
        (*out).power_mgnt = (*p).power_mgnt;
        (*out).bLeisurePs = (*p).bLeisurePs;
        (*out).LpsIdleCount = (*p).LpsIdleCount;
        (*out).lps_level = (*p).lps_level;
        (*out).ips_mode_req = (*p).ips_mode_req;
    }
}

#[cfg(host_pwrctrl_pm_test)]
pub mod host_pwrctrl_pm {
    #[repr(C)]
    pub struct HostPwrctrlPmState {
        pub power_mgnt: u8,
        pub bLeisurePs: u8,
        pub LpsIdleCount: u8,
        pub lps_level: u8,
        pub ips_mode_req: u8,
    }
}
