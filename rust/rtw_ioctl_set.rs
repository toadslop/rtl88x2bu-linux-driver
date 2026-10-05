// SPDX-License-Identifier: GPL-2.0
//! W3-124 ioctl validate helpers (host L2 Rust oracle).
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
