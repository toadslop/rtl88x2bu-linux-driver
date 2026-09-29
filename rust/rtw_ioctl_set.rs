// SPDX-License-Identifier: GPL-2.0
//! W3-124/W3-125 ioctl helpers (host L2 Rust oracles).
#![allow(dead_code, improper_ctypes, missing_docs, non_snake_case)]

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
    s.dot11AuthAlgrthm = 0;
    if s.ndisauthtype > 3 {
        s.dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
    }
    if !dot11_out.is_null() {
        *dot11_out = s.dot11AuthAlgrthm;
    }
    _TRUE
}
