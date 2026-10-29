// SPDX-License-Identifier: GPL-2.0
//! W3-183 — `PS_RDY_CHECK` from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_ps_rdy_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_ps_rdy_test))]
use core::ffi::c_void;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;

const WIFI_UNDER_SURVEY: u32 = 0x0000_0800;
const WIFI_UNDER_LINKING: u32 = 0x0000_0080;
const WIFI_UNDER_WPS: u32 = 0x0000_0100;
const WIFI_AP_STATE: u32 = 0x0000_0010;
const WIFI_MESH_STATE: u32 = 0x0000_0200;
const WIFI_MONITOR_STATE: u32 = 0x8000_0000;
const WIFI_ADHOC_MASTER_STATE: u32 = 0x0000_0020;
const WIFI_ADHOC_STATE: u32 = 0x0000_0040;
const dot11AuthAlgrthm_8021X: u32 = 2;

type Padapter = *mut c_void;

#[cfg(host_pwrctrl_ps_rdy_test)]
#[repr(C)]
struct MlmePriv {
    fw_state: u32,
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[repr(C)]
struct SecurityPriv {
    dot11AuthAlgrthm: u32,
    binstallGrpkey: u8,
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[repr(C)]
struct TdlsInfo {
    link_established: u8,
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[repr(C)]
struct RfCtl {
    radar_detect_enabled: u8,
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[repr(C)]
struct PwrctrlPriv {
    bInSuspend: u8,
    lps_deny_time: u32,
    wowlan_mode: u8,
    wowlan_ap_mode: u8,
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[repr(C)]
struct HostAdapter {
    mlmepriv: MlmePriv,
    pwrctrlpriv: PwrctrlPriv,
    securitypriv: SecurityPriv,
    tdlsinfo: TdlsInfo,
    rfctl: *mut RfCtl,
}

#[cfg(host_pwrctrl_ps_rdy_test)]
static mut G_TIME: u32 = 0;
#[cfg(host_pwrctrl_ps_rdy_test)]
static mut G_SCAN_DENY: u8 = 0;
#[cfg(host_pwrctrl_ps_rdy_test)]
static mut G_ROCH: u8 = 0;
#[cfg(host_pwrctrl_ps_rdy_test)]
static mut G_PWR_MGMT: u8 = _TRUE;

#[cfg(host_pwrctrl_ps_rdy_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_time(t: u32) {
    unsafe {
        G_TIME = t;
    }
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_scan_deny(v: u8) {
    unsafe {
        G_SCAN_DENY = v;
    }
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_roch(v: u8) {
    unsafe {
        G_ROCH = v;
    }
}

#[cfg(host_pwrctrl_ps_rdy_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_ps_rdy_set_pwr_mgmt(v: u8) {
    unsafe {
        G_PWR_MGMT = v;
    }
}

#[cfg(host_pwrctrl_ps_rdy_test)]
fn check_fwstate(m: &MlmePriv, s: u32) -> bool {
    if s == 0 && m.fw_state == 0 {
        return true;
    }
    (m.fw_state & s) != 0
}

#[cfg(host_pwrctrl_ps_rdy_test)]
fn mlme_is_ap(a: &HostAdapter) -> bool {
    (a.mlmepriv.fw_state & WIFI_AP_STATE) != 0
}

#[cfg(host_pwrctrl_ps_rdy_test)]
fn mlme_is_mesh(a: &HostAdapter) -> bool {
    (a.mlmepriv.fw_state & WIFI_MESH_STATE) != 0
}

#[cfg(host_pwrctrl_ps_rdy_test)]
fn mlme_is_monitor(a: &HostAdapter) -> bool {
    (a.mlmepriv.fw_state & WIFI_MONITOR_STATE) != 0
}

#[cfg(not(host_pwrctrl_ps_rdy_test))]
extern "C" {
    fn rtw_rust_ps_rdy_get_current_time() -> u32;
    fn rtw_rust_ps_rdy_time_after(a: u32, b: u32) -> u8;
    fn rtw_rust_ps_rdy_check_fwstate(adapter: Padapter, state: u32) -> u8;
    fn rtw_rust_ps_rdy_mlme_is_ap(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_mlme_is_mesh(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_mlme_is_monitor(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_get_b_in_suspend(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_get_lps_deny_time(adapter: Padapter) -> u32;
    fn rtw_rust_ps_rdy_get_wowlan_mode(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_get_wowlan_ap_mode(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_cfg80211_get_is_roch(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_is_scan_deny(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_tdls_linked(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_radar_detect(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_get_dot11_auth(adapter: Padapter) -> u32;
    fn rtw_rust_ps_rdy_get_binstall_grpkey(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_cfg80211_pwr_mgmt(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_log_group_handshake_in_progress();
}

fn ps_rdy_check_impl(adapter: Padapter) -> u8 {
    #[cfg(host_pwrctrl_ps_rdy_test)]
    {
        let a = unsafe { &*(adapter as *mut HostAdapter) };
        let p = &a.pwrctrlpriv;
        let m = &a.mlmepriv;

        if p.bInSuspend == _TRUE && p.wowlan_mode == _TRUE {
            return _TRUE;
        }
        if p.bInSuspend == _TRUE && p.wowlan_ap_mode == _TRUE {
            return _TRUE;
        }
        if p.bInSuspend == _TRUE {
            return _FALSE;
        }

        let now = unsafe { G_TIME };
        if (p.lps_deny_time as i32).wrapping_sub(now as i32) > 0 {
            return _FALSE;
        }

        if check_fwstate(m, WIFI_UNDER_SURVEY)
            || check_fwstate(m, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)
            || mlme_is_ap(a)
            || mlme_is_mesh(a)
            || mlme_is_monitor(a)
            || check_fwstate(m, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE)
            || unsafe { G_ROCH } == _TRUE
            || unsafe { G_SCAN_DENY } == _TRUE
            || a.tdlsinfo.link_established == _TRUE
            || (!a.rfctl.is_null() && unsafe { (*a.rfctl).radar_detect_enabled } == _TRUE)
        {
            return _FALSE;
        }

        if a.securitypriv.dot11AuthAlgrthm == dot11AuthAlgrthm_8021X
            && a.securitypriv.binstallGrpkey == _FALSE
        {
            return _FALSE;
        }

        if unsafe { G_PWR_MGMT } == _FALSE {
            return _FALSE;
        }

        return _TRUE;
    }

    #[cfg(not(host_pwrctrl_ps_rdy_test))]
    unsafe {
        if rtw_rust_ps_rdy_get_b_in_suspend(adapter) == _TRUE
            && rtw_rust_ps_rdy_get_wowlan_mode(adapter) == _TRUE
        {
            return _TRUE;
        }
        if rtw_rust_ps_rdy_get_b_in_suspend(adapter) == _TRUE
            && rtw_rust_ps_rdy_get_wowlan_ap_mode(adapter) == _TRUE
        {
            return _TRUE;
        }
        if rtw_rust_ps_rdy_get_b_in_suspend(adapter) == _TRUE {
            return _FALSE;
        }

        let now = rtw_rust_ps_rdy_get_current_time();
        let deny = rtw_rust_ps_rdy_get_lps_deny_time(adapter);
        if rtw_rust_ps_rdy_time_after(deny, now) == _TRUE {
            return _FALSE;
        }

        if rtw_rust_ps_rdy_check_fwstate(adapter, WIFI_UNDER_SURVEY) == _TRUE
            || rtw_rust_ps_rdy_check_fwstate(adapter, WIFI_UNDER_LINKING | WIFI_UNDER_WPS) == _TRUE
            || rtw_rust_ps_rdy_mlme_is_ap(adapter) == _TRUE
            || rtw_rust_ps_rdy_mlme_is_mesh(adapter) == _TRUE
            || rtw_rust_ps_rdy_mlme_is_monitor(adapter) == _TRUE
            || rtw_rust_ps_rdy_check_fwstate(adapter, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE)
                == _TRUE
            || rtw_rust_ps_rdy_cfg80211_get_is_roch(adapter) == _TRUE
            || rtw_rust_ps_rdy_is_scan_deny(adapter) == _TRUE
            || rtw_rust_ps_rdy_tdls_linked(adapter) == _TRUE
            || rtw_rust_ps_rdy_radar_detect(adapter) == _TRUE
        {
            return _FALSE;
        }

        if rtw_rust_ps_rdy_get_dot11_auth(adapter) == dot11AuthAlgrthm_8021X
            && rtw_rust_ps_rdy_get_binstall_grpkey(adapter) == _FALSE
        {
            rtw_rust_ps_rdy_log_group_handshake_in_progress();
            return _FALSE;
        }

        if rtw_rust_ps_rdy_cfg80211_pwr_mgmt(adapter) == _FALSE {
            return _FALSE;
        }

        _TRUE
    }
}

#[no_mangle]
pub extern "C" fn PS_RDY_CHECK(padapter: Padapter) -> u8 {
    ps_rdy_check_impl(padapter)
}
