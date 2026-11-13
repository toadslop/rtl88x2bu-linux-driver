// SPDX-License-Identifier: GPL-2.0
//! Host L2 stubs for `rtw_pwrctrl_ps_rdy.rs` (shared PS_RDY_CHECK core).

#![allow(non_snake_case, non_camel_case_types)]

use crate::{Padapter, _FALSE, _TRUE};
use std::ffi::c_ulong;

#[repr(C)]
pub struct MlmePriv {
    pub fw_state: u32,
}

#[repr(C)]
pub struct SecurityPriv {
    pub dot11AuthAlgrthm: u32,
    pub binstallGrpkey: u8,
}

#[repr(C)]
pub struct TdlsInfo {
    pub link_established: u8,
}

#[repr(C)]
pub struct RfCtl {
    pub radar_detect_enabled: u8,
}

#[repr(C)]
pub struct PwrctrlPriv {
    pub bInSuspend: u8,
    pub lps_deny_time: u32,
    pub wowlan_mode: u8,
    pub wowlan_ap_mode: u8,
}

#[repr(C)]
pub struct HostAdapter {
    pub mlmepriv: MlmePriv,
    pub pwrctrlpriv: PwrctrlPriv,
    pub securitypriv: SecurityPriv,
    pub tdlsinfo: TdlsInfo,
    pub rfctl: *mut RfCtl,
}

static mut G_TIME: c_ulong = 0;
static mut G_SCAN_DENY: u8 = 0;
static mut G_ROCH: u8 = 0;
static mut G_PWR_MGMT: u8 = _TRUE;

fn adapter(padapter: Padapter) -> *mut HostAdapter {
    padapter as *mut HostAdapter
}

fn mlme(padapter: Padapter) -> *mut MlmePriv {
    unsafe { &mut (*adapter(padapter)).mlmepriv }
}

fn pwr(padapter: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*adapter(padapter)).pwrctrlpriv }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_time(t: c_ulong) {
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

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_get_current_time() -> c_ulong {
    unsafe { G_TIME }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_time_after(a: c_ulong, b: c_ulong) -> u8 {
    if (b as i64).wrapping_sub(a as i64) < 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_check_fwstate(padapter: Padapter, state: u32) -> u8 {
    let m = unsafe { &*mlme(padapter) };
    if state == 0 && m.fw_state == 0 {
        return _TRUE;
    }
    if m.fw_state & state != 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_mlme_is_ap(padapter: Padapter) -> u8 {
    let m = unsafe { &*mlme(padapter) };
    if m.fw_state & crate::WIFI_AP_STATE != 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_mlme_is_mesh(padapter: Padapter) -> u8 {
    let m = unsafe { &*mlme(padapter) };
    if m.fw_state & crate::WIFI_MESH_STATE != 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_mlme_is_monitor(padapter: Padapter) -> u8 {
    let m = unsafe { &*mlme(padapter) };
    if m.fw_state & crate::WIFI_MONITOR_STATE != 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_get_b_in_suspend(padapter: Padapter) -> u8 {
    unsafe { (*pwr(padapter)).bInSuspend }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_get_lps_deny_time(padapter: Padapter) -> c_ulong {
    unsafe { (*pwr(padapter)).lps_deny_time as c_ulong }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_get_wowlan_mode(padapter: Padapter) -> u8 {
    unsafe { (*pwr(padapter)).wowlan_mode }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_get_wowlan_ap_mode(padapter: Padapter) -> u8 {
    unsafe { (*pwr(padapter)).wowlan_ap_mode }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_cfg80211_get_is_roch(padapter: Padapter) -> u8 {
    let _ = padapter;
    unsafe { G_ROCH }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_is_scan_deny(padapter: Padapter) -> u8 {
    let _ = padapter;
    unsafe { G_SCAN_DENY }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_tdls_linked(padapter: Padapter) -> u8 {
    unsafe { (*adapter(padapter)).tdlsinfo.link_established }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_radar_detect(padapter: Padapter) -> u8 {
    let rf = unsafe { (*adapter(padapter)).rfctl };
    if rf.is_null() {
        return _FALSE;
    }
    unsafe {
        if (*rf).radar_detect_enabled == _TRUE {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_get_dot11_auth(padapter: Padapter) -> u32 {
    unsafe { (*adapter(padapter)).securitypriv.dot11AuthAlgrthm }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_get_binstall_grpkey(padapter: Padapter) -> u8 {
    unsafe { (*adapter(padapter)).securitypriv.binstallGrpkey }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_cfg80211_pwr_mgmt(padapter: Padapter) -> u8 {
    let _ = padapter;
    unsafe { G_PWR_MGMT }
}

#[no_mangle]
pub extern "C" fn rtw_rust_ps_rdy_log_group_handshake_in_progress() {}
