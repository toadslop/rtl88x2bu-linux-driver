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
use std::ffi::{c_ulong, c_void};

#[cfg(not(host_pwrctrl_ps_rdy_test))]
use core::ffi::{c_ulong, c_void};

/// Kernel `systime` (`unsigned long` jiffies). Must stay full width.
type Systime = c_ulong;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;

pub(crate) const WIFI_UNDER_SURVEY: u32 = 0x0000_0800;
pub(crate) const WIFI_UNDER_LINKING: u32 = 0x0000_0080;
pub(crate) const WIFI_UNDER_WPS: u32 = 0x0000_0100;
pub(crate) const WIFI_AP_STATE: u32 = 0x0000_0010;
pub(crate) const WIFI_MESH_STATE: u32 = 0x0000_0200;
pub(crate) const WIFI_MONITOR_STATE: u32 = 0x8000_0000;
pub(crate) const WIFI_ADHOC_MASTER_STATE: u32 = 0x0000_0020;
pub(crate) const WIFI_ADHOC_STATE: u32 = 0x0000_0040;
pub(crate) const dot11AuthAlgrthm_8021X: u32 = 2;

pub(crate) type Padapter = *mut c_void;

#[cfg(host_pwrctrl_ps_rdy_test)]
#[path = "rtw_pwrctrl_ps_rdy_host.rs"]
mod host;

extern "C" {
    fn rtw_rust_ps_rdy_get_current_time() -> Systime;
    fn rtw_rust_ps_rdy_time_after(a: Systime, b: Systime) -> u8;
    fn rtw_rust_ps_rdy_check_fwstate(adapter: Padapter, state: u32) -> u8;
    fn rtw_rust_ps_rdy_mlme_is_ap(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_mlme_is_mesh(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_mlme_is_monitor(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_get_b_in_suspend(adapter: Padapter) -> u8;
    fn rtw_rust_ps_rdy_get_lps_deny_time(adapter: Padapter) -> Systime;
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

fn ps_rdy_check_core(adapter: Padapter) -> u8 {
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
    ps_rdy_check_core(padapter)
}
