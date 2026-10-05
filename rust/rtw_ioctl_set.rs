// SPDX-License-Identifier: GPL-2.0
//! W3-124/W3-125 ioctl helpers (host L2 Rust oracles + kernel leaf setters).
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    unreachable_pub
)]

#[cfg(rust_ioctl_set_leaf)]
use core::ffi::c_void;

#[cfg(rust_ioctl_set_leaf)]
mod kernel {
    use super::c_void;

    extern "C" {
        fn rtw_rust_ioctl_scan_mode_ptr(adapter: *mut c_void) -> *mut i32;
        fn rtw_rust_ioctl_setband_ptr(adapter: *mut c_void) -> *mut u32;
    }

    pub unsafe fn scan_mode_ptr(adapter: *mut c_void) -> *mut i32 {
        unsafe { rtw_rust_ioctl_scan_mode_ptr(adapter) }
    }

    pub unsafe fn setband_ptr(adapter: *mut c_void) -> *mut u32 {
        unsafe { rtw_rust_ioctl_setband_ptr(adapter) }
    }
}

#[cfg(rust_ioctl_set_leaf)]
const SCAN_PASSIVE_K: i32 = 0;
#[cfg(rust_ioctl_set_leaf)]
const SCAN_ACTIVE_K: i32 = 1;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_FREQUENCY_BAND_2GHZ_K: u8 = 2;

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_scan_mode(adapter: *mut c_void, scan_mode: i32) -> i32 {
    if scan_mode != SCAN_ACTIVE_K && scan_mode != SCAN_PASSIVE_K {
        return _FAIL;
    }
    let ptr = unsafe { kernel::scan_mode_ptr(adapter) };
    unsafe {
        *ptr = scan_mode;
    }
    _SUCCESS
}

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_band(adapter: *mut c_void, band: u8) -> i32 {
    if band > WIFI_FREQUENCY_BAND_2GHZ_K {
        return _FAIL;
    }
    let ptr = unsafe { kernel::setband_ptr(adapter) };
    unsafe {
        *ptr = band as u32;
    }
    _SUCCESS
}

#[cfg(rust_ioctl_set_leaf)]
const _SUCCESS: i32 = 1;
#[cfg(rust_ioctl_set_leaf)]
const _FAIL: i32 = 0;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;

#[cfg(host_ioctl_validate_test)]
#[repr(C)]
pub struct Ndis80211Ssid {
    pub SsidLength: u32,
    pub Ssid: [u8; 32],
}

#[cfg(host_ioctl_validate_test)]
fn is_zero_mac_addr(addr: &[u8; 6]) -> bool {
    addr.iter().all(|&b| b == 0)
}

#[cfg(host_ioctl_validate_test)]
fn is_broadcast_mac_addr(addr: &[u8; 6]) -> bool {
    addr.iter().all(|&b| b == 0xff)
}

#[cfg(host_ioctl_validate_test)]
fn is_multicast_mac_addr(addr: &[u8; 6]) -> bool {
    (addr[0] & 0x01) == 0x01 && addr[0] != 0xff
}

#[cfg(host_ioctl_validate_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_validate_bssid_rust(bssid: *mut u8) -> u8 {
    if bssid.is_null() {
        return _FALSE;
    }
    let mac: &[u8; 6] = &*(bssid as *const [u8; 6]);
    if is_zero_mac_addr(mac) || is_broadcast_mac_addr(mac) || is_multicast_mac_addr(mac) {
        _FALSE
    } else {
        _TRUE
    }
}

#[cfg(host_ioctl_validate_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_validate_ssid_rust(ssid: *mut Ndis80211Ssid) -> u8 {
    if ssid.is_null() {
        return _FALSE;
    }
    let ssid = &*ssid;
    if ssid.SsidLength > 32 {
        _FALSE
    } else {
        _TRUE
    }
}

#[cfg(host_ioctl_scan_channel_test)]
const _SUCCESS: i32 = 1;
#[cfg(host_ioctl_scan_channel_test)]
const _FAIL: i32 = 0;
#[cfg(host_ioctl_scan_channel_test)]
const SCAN_PASSIVE: i32 = 0;
#[cfg(host_ioctl_scan_channel_test)]
const SCAN_ACTIVE: i32 = 1;
#[cfg(host_ioctl_scan_channel_test)]
const WIFI_FREQUENCY_BAND_2GHZ: u8 = 2;
#[cfg(host_ioctl_scan_channel_test)]
const DOT11_AUTH_ALGRTHM_8021X: u32 = 2;
#[cfg(host_ioctl_scan_channel_test)]
const _WEP40_: u32 = 0x01;
#[cfg(host_ioctl_scan_channel_test)]
const _WEP104_: u32 = 0x05;
#[cfg(host_ioctl_scan_channel_test)]
const _NO_PRIVACY_: u32 = 0x00;

#[cfg(host_ioctl_scan_channel_test)]
#[repr(C)]
pub struct MockMlmePriv {
    pub scan_mode: i32,
}

#[cfg(host_ioctl_scan_channel_test)]
#[repr(C)]
pub struct MockAdapter {
    pub mlmepriv: MockMlmePriv,
    pub setband: u8,
}

#[cfg(host_ioctl_scan_channel_test)]
#[repr(C)]
pub struct MockSecurityPriv {
    pub ndisauthtype: u32,
    pub dot11AuthAlgrthm: u32,
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_scan_mode_rust(adapter: *mut MockAdapter, scan_mode: i32) -> i32 {
    if adapter.is_null() {
        return _FAIL;
    }
    let a = &mut *adapter;
    if scan_mode != SCAN_ACTIVE && scan_mode != SCAN_PASSIVE {
        return _FAIL;
    }
    a.mlmepriv.scan_mode = scan_mode;
    _SUCCESS
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_band_rust(adapter: *mut MockAdapter, band: u8) -> i32 {
    if adapter.is_null() {
        return _FAIL;
    }
    let a = &mut *adapter;
    if band > WIFI_FREQUENCY_BAND_2GHZ {
        return _FAIL;
    }
    a.setband = band;
    _SUCCESS
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_add_wep_privacy_rust(
    key_index: u32,
    key_length: u32,
    privacy_out: *mut u32,
) -> u8 {
    let keyid = key_index & 0x3fff_ffff;
    if keyid >= 4 {
        return _FALSE;
    }
    let privacy = match key_length {
        5 => _WEP40_,
        13 => _WEP104_,
        _ => _NO_PRIVACY_,
    };
    if !privacy_out.is_null() {
        *privacy_out = privacy;
    }
    _TRUE
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_auth_mode_map_rust(
    sec: *mut MockSecurityPriv,
    authmode: u32,
    dot11_out: *mut u32,
) -> u8 {
    if sec.is_null() {
        return _FALSE;
    }
    let s = &mut *sec;
    s.ndisauthtype = authmode;
    if s.ndisauthtype > 3 {
        s.dot11AuthAlgrthm = DOT11_AUTH_ALGRTHM_8021X;
    }
    if !dot11_out.is_null() {
        *dot11_out = s.dot11AuthAlgrthm;
    }
    _TRUE
}

#[cfg(host_ioctl_connect_test)]
const WIFI_ASOC_STATE_K: u32 = 0x0000_0001;
#[cfg(host_ioctl_connect_test)]
const WIFI_UNDER_LINKING_K: u32 = 0x0000_0080;
#[cfg(host_ioctl_connect_test)]
const WIFI_UNDER_SURVEY_K: u32 = 0x0000_0800;

#[cfg(host_ioctl_connect_test)]
#[repr(C)]
#[derive(Copy, Clone)]
pub struct HostNdis80211Ssid {
    pub SsidLength: u32,
    pub Ssid: [u8; 32],
}

#[cfg(host_ioctl_connect_test)]
#[repr(C)]
pub struct HostMlmePriv {
    pub fw_state: u32,
    pub to_join: u8,
    pub assoc_by_bssid: u8,
    pub assoc_ch: u16,
    pub assoc_bssid: [u8; 6],
    pub assoc_ssid: HostNdis80211Ssid,
}

#[cfg(host_ioctl_connect_test)]
#[repr(C)]
pub struct HostAdapter {
    pub hw_init_done: u8,
    pub tkip_fail: u8,
    pub do_join_ret: u8,
    pub mlmepriv: HostMlmePriv,
}

#[cfg(host_ioctl_connect_test)]
static mut HOST_IOCTL_CONNECT_DISASSOC_CALLS: u32 = 0;
#[cfg(host_ioctl_connect_test)]
static mut HOST_IOCTL_CONNECT_JOIN_CALLS: u32 = 0;

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub extern "C" fn ioctl_connect_test_reset_counters() {
    unsafe {
        HOST_IOCTL_CONNECT_DISASSOC_CALLS = 0;
        HOST_IOCTL_CONNECT_JOIN_CALLS = 0;
    }
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub extern "C" fn ioctl_connect_test_disassoc_calls() -> u32 {
    unsafe { HOST_IOCTL_CONNECT_DISASSOC_CALLS }
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub extern "C" fn ioctl_connect_test_join_calls() -> u32 {
    unsafe { HOST_IOCTL_CONNECT_JOIN_CALLS }
}

#[cfg(host_ioctl_connect_test)]
fn host_validate_bssid(bssid: &[u8; 6]) -> bool {
    let all_zero = bssid.iter().all(|&b| b == 0);
    let all_ff = bssid.iter().all(|&b| b == 0xff);
    if all_zero || all_ff {
        return false;
    }
    !((bssid[0] & 0x01) != 0 && bssid[0] != 0xff)
}

#[cfg(host_ioctl_connect_test)]
fn host_validate_ssid(ssid: &HostNdis80211Ssid) -> bool {
    ssid.SsidLength <= 32
}

#[cfg(host_ioctl_connect_test)]
fn host_chk_fw(mlme: &HostMlmePriv, st: i32) -> bool {
    st == 0 && mlme.fw_state == 0 || (mlme.fw_state & st as u32) != 0
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_disassociate_rust(p: *mut HostAdapter) -> u8 {
    if p.is_null() {
        return _FALSE;
    }
    let padapter = &mut *p;
    if host_chk_fw(&padapter.mlmepriv, WIFI_ASOC_STATE_K as i32) {
        unsafe {
            HOST_IOCTL_CONNECT_DISASSOC_CALLS += 1;
        }
    }
    _TRUE
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_connect_rust(
    p: *mut HostAdapter,
    bssid: *mut u8,
    ssid: *mut HostNdis80211Ssid,
    ch: u16,
) -> u8 {
    if p.is_null() {
        return _FAIL as u8;
    }
    let padapter = &mut *p;
    let mlme = &mut padapter.mlmepriv;

    let mut sv = true;
    let mut bv = true;
    if ssid.is_null() || !host_validate_ssid(&*ssid) {
        sv = false;
    }
    if bssid.is_null() || !host_validate_bssid(&*(bssid as *const [u8; 6])) {
        bv = false;
    }
    if !sv && !bv {
        return _FAIL as u8;
    }
    if padapter.hw_init_done == 0 {
        return _FAIL as u8;
    }

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        // handle_tkip_countermeasure — no-op in host oracle
    } else if host_chk_fw(mlme, WIFI_UNDER_LINKING_K as i32) {
        return _SUCCESS as u8;
    }

    if padapter.tkip_fail != 0 {
        return _FAIL as u8;
    }

    if !ssid.is_null() && sv {
        mlme.assoc_ssid = *ssid;
    } else {
        mlme.assoc_ssid = HostNdis80211Ssid {
            SsidLength: 0,
            Ssid: [0; 32],
        };
    }

    if !bssid.is_null() && bv {
        mlme.assoc_bssid.copy_from_slice(&*(bssid as *const [u8; 6]));
        mlme.assoc_by_bssid = _TRUE;
    } else {
        mlme.assoc_by_bssid = _FALSE;
    }
    mlme.assoc_ch = ch;

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        mlme.to_join = _TRUE;
        return _SUCCESS as u8;
    }

    unsafe {
        HOST_IOCTL_CONNECT_JOIN_CALLS += 1;
    }
    if padapter.do_join_ret != 0 {
        _SUCCESS as u8
    } else {
        _FAIL as u8
    }
}

#[cfg(host_ioctl_connect_test)]
const _SUCCESS: i32 = 1;
#[cfg(host_ioctl_connect_test)]
const _FAIL: i32 = 0;
