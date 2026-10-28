// SPDX-License-Identifier: GPL-2.0
//! W3-182 — `rtw_wow_lps_level_decide` from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_wow_lps_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_wow_lps_test))]
use core::ffi::c_void;

const FIELD_LPS_LEVEL: u32 = 0;
const FIELD_LPS_LEVEL_BK: u32 = 1;
const FIELD_WOWLAN_LPS_LEVEL: u32 = 2;
const FIELD_LPS_1T1R: u32 = 3;
const FIELD_LPS_1T1R_BK: u32 = 4;
const FIELD_WOWLAN_LPS_1T1R: u32 = 5;

type Padapter = *mut c_void;

#[cfg(host_pwrctrl_wow_lps_test)]
#[repr(C)]
struct PwrctrlPriv {
    wowlan_lps_level: u8,
    wowlan_lps_1t1r: u8,
    lps_level_bk: u8,
    lps_level: u8,
    lps_1t1r_bk: u8,
    lps_1t1r: u8,
}

#[cfg(host_pwrctrl_wow_lps_test)]
#[repr(C)]
struct DvobjPriv {
    pwrctl_priv: PwrctrlPriv,
}

#[cfg(host_pwrctrl_wow_lps_test)]
#[repr(C)]
struct HostAdapter {
    dvobj: *mut DvobjPriv,
}

#[cfg(host_pwrctrl_wow_lps_test)]
fn pwrpriv_host(adapter: Padapter) -> *mut PwrctrlPriv {
    let dvobj = unsafe { (*(adapter as *mut HostAdapter)).dvobj };
    unsafe { &mut (*dvobj).pwrctl_priv }
}

#[cfg(host_pwrctrl_wow_lps_test)]
fn field_get(adapter: Padapter, id: u32) -> u8 {
    let p = pwrpriv_host(adapter);
    unsafe {
        match id {
            FIELD_LPS_LEVEL => (*p).lps_level,
            FIELD_LPS_LEVEL_BK => (*p).lps_level_bk,
            FIELD_WOWLAN_LPS_LEVEL => (*p).wowlan_lps_level,
            FIELD_LPS_1T1R => (*p).lps_1t1r,
            FIELD_LPS_1T1R_BK => (*p).lps_1t1r_bk,
            FIELD_WOWLAN_LPS_1T1R => (*p).wowlan_lps_1t1r,
            _ => 0,
        }
    }
}

#[cfg(host_pwrctrl_wow_lps_test)]
fn field_set(adapter: Padapter, id: u32, v: u8) {
    let p = pwrpriv_host(adapter);
    unsafe {
        match id {
            FIELD_LPS_LEVEL => (*p).lps_level = v,
            FIELD_LPS_LEVEL_BK => (*p).lps_level_bk = v,
            FIELD_WOWLAN_LPS_LEVEL => (*p).wowlan_lps_level = v,
            FIELD_LPS_1T1R => (*p).lps_1t1r = v,
            FIELD_LPS_1T1R_BK => (*p).lps_1t1r_bk = v,
            FIELD_WOWLAN_LPS_1T1R => (*p).wowlan_lps_1t1r = v,
            _ => {}
        }
    }
}

#[cfg(not(host_pwrctrl_wow_lps_test))]
extern "C" {
    fn rtw_rust_wow_lps_field_get(adapter: Padapter, id: u32) -> u8;
    fn rtw_rust_wow_lps_field_set(adapter: Padapter, id: u32, v: u8);
}

fn apply_wow_on(adapter: Padapter) {
    let cur_level = field_get(adapter, FIELD_LPS_LEVEL);
    field_set(adapter, FIELD_LPS_LEVEL_BK, cur_level);
    field_set(
        adapter,
        FIELD_LPS_LEVEL,
        field_get(adapter, FIELD_WOWLAN_LPS_LEVEL),
    );
    let cur_1t1r = field_get(adapter, FIELD_LPS_1T1R);
    field_set(adapter, FIELD_LPS_1T1R_BK, cur_1t1r);
    field_set(
        adapter,
        FIELD_LPS_1T1R,
        field_get(adapter, FIELD_WOWLAN_LPS_1T1R),
    );
}

fn apply_wow_off(adapter: Padapter) {
    field_set(
        adapter,
        FIELD_LPS_LEVEL,
        field_get(adapter, FIELD_LPS_LEVEL_BK),
    );
    field_set(
        adapter,
        FIELD_LPS_1T1R,
        field_get(adapter, FIELD_LPS_1T1R_BK),
    );
}

#[cfg(not(host_pwrctrl_wow_lps_test))]
fn field_get(adapter: Padapter, id: u32) -> u8 {
    unsafe { rtw_rust_wow_lps_field_get(adapter, id) }
}

#[cfg(not(host_pwrctrl_wow_lps_test))]
fn field_set(adapter: Padapter, id: u32, v: u8) {
    unsafe { rtw_rust_wow_lps_field_set(adapter, id, v) }
}

#[no_mangle]
pub extern "C" fn rtw_wow_lps_level_decide(adapter: Padapter, wow_en: u8) {
    if adapter.is_null() {
        return;
    }
    if wow_en != 0 {
        apply_wow_on(adapter);
    } else {
        apply_wow_off(adapter);
    }
}

#[cfg(host_pwrctrl_wow_lps_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_wow_lps_read_state(
    adapter: Padapter,
    out: *mut host_pwrctrl_wow_lps::HostPwrctrlWowLpsState,
) {
    if adapter.is_null() || out.is_null() {
        return;
    }
    unsafe {
        (*out).lps_level = field_get(adapter, FIELD_LPS_LEVEL);
        (*out).lps_level_bk = field_get(adapter, FIELD_LPS_LEVEL_BK);
        (*out).lps_1t1r = field_get(adapter, FIELD_LPS_1T1R);
        (*out).lps_1t1r_bk = field_get(adapter, FIELD_LPS_1T1R_BK);
    }
}

#[cfg(host_pwrctrl_wow_lps_test)]
pub mod host_pwrctrl_wow_lps {
    #[repr(C)]
    pub struct HostPwrctrlWowLpsState {
        pub lps_level: u8,
        pub lps_level_bk: u8,
        pub lps_1t1r: u8,
        pub lps_1t1r_bk: u8,
    }
}
