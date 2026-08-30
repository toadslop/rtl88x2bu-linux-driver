// SPDX-License-Identifier: GPL-2.0
//! W3-65 network update merge — host L2 oracle.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals
)]

#[cfg(host_mlme_network_update_test)]
use std::os::raw::{c_int, c_long, c_void};

type U8 = u8;
type U32 = u32;
const ETH_ALEN: usize = 6;
const MAX_IE_SZ: usize = 768;
const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const WIFI_ASOC_STATE: c_int = 0x0000_0001;

#[repr(C)]
struct Ndis80211Ssid {
    ssid_length: U32,
    ssid: [U8; 32],
}
#[repr(C)]
struct WlanPhyInfo {
    signal_strength: U8,
    signal_quality: U8,
    pad: [U8; 6],
}
#[repr(C)]
struct WlanBssidEx {
    length: U32,
    mac_address: [U8; ETH_ALEN],
    reserved: [U8; 2],
    ssid: Ndis80211Ssid,
    mesh_id: Ndis80211Ssid,
    privacy: U32,
    rssi: c_long,
    configuration: [U8; 16],
    infrastructure_mode: U32,
    supported_rates: [U8; 16],
    phy_info: WlanPhyInfo,
    ie_length: U32,
    ies: [U8; MAX_IE_SZ],
}
#[repr(C)]
struct WlanNetwork {
    network: WlanBssidEx,
}
#[repr(C)]
struct MlmePriv {
    fw_state: c_int,
    cur_network: WlanNetwork,
}
#[repr(C)]
struct RecvPriv {
    signal_strength: U8,
    signal_qual: U8,
}
#[repr(C)]
struct Adapter {
    mlmepriv: MlmePriv,
    recvpriv: RecvPriv,
}

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn rtw_bug_check(p1: *mut c_void, p2: *mut c_void, p3: *mut c_void, p4: *mut c_void) -> c_int;
    fn is_all_null(c: *mut i8, len: c_int) -> c_int;
    fn rtw_update_protection(adapter: *mut Adapter, ie: *mut U8, ie_len: U32);
}

fn bss_sz(b: *mut WlanBssidEx) -> usize {
    unsafe { core::mem::size_of::<WlanBssidEx>() - MAX_IE_SZ + (*b).ie_length as usize }
}

fn fw_on(m: *mut MlmePriv, st: c_int) -> c_int {
    unsafe {
        if (*m).fw_state & st != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[no_mangle]
pub extern "C" fn is_same_network(src: *mut WlanBssidEx, dst: *mut WlanBssidEx, _: U8) -> c_int {
    if src.is_null() || dst.is_null() {
        return _FALSE;
    }
    unsafe {
        let (s_cap, d_cap) = (
            u16::from_le_bytes([(*src).ies[10], (*src).ies[11]]),
            u16::from_le_bytes([(*dst).ies[10], (*dst).ies[11]]),
        );
        if _rtw_memcmp(
            (*src).mac_address.as_ptr() as *const _,
            (*dst).mac_address.as_ptr() as *const _,
            ETH_ALEN,
        ) != _TRUE
            || (s_cap & 0x2) != (d_cap & 0x2)
            || (s_cap & 0x1) != (d_cap & 0x1)
        {
            return _FALSE;
        }
        if (*src).ssid.ssid_length == (*dst).ssid.ssid_length
            && (_rtw_memcmp(
                (*src).ssid.ssid.as_ptr() as *const _,
                (*dst).ssid.ssid.as_ptr() as *const _,
                (*src).ssid.ssid_length as usize,
            ) == _TRUE
                || is_all_null(
                    (*src).ssid.ssid.as_mut_ptr() as *mut i8,
                    (*src).ssid.ssid_length as c_int,
                ) == _TRUE
                || is_all_null(
                    (*dst).ssid.ssid.as_mut_ptr() as *mut i8,
                    (*dst).ssid.ssid_length as c_int,
                ) == _TRUE)
        {
            return _TRUE;
        }
        if (*src).ssid.ssid_length == 0 || (*dst).ssid.ssid_length == 0 {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[no_mangle]
pub extern "C" fn update_network(
    dst: *mut WlanBssidEx,
    src: *mut WlanBssidEx,
    a: *mut Adapter,
    update_ie: bool,
) {
    if dst.is_null() || src.is_null() || a.is_null() {
        return;
    }
    unsafe {
        let sq_smp = (*src).phy_info.signal_quality;
        let (ss, sq, rssi) = if fw_on(&mut (*a).mlmepriv, WIFI_ASOC_STATE) == _TRUE
            && is_same_network(&mut (*a).mlmepriv.cur_network.network, src, 0) == _TRUE
        {
            let r = if sq_smp != 101 {
                ((*src).rssi + (*dst).rssi * 4) / 5
            } else {
                (*dst).rssi
            };
            ((*a).recvpriv.signal_strength, (*a).recvpriv.signal_qual, r)
        } else if sq_smp != 101 {
            (
                ((u32::from((*src).phy_info.signal_strength)
                    + u32::from((*dst).phy_info.signal_strength) * 4)
                    / 5) as u8,
                ((u32::from((*src).phy_info.signal_quality)
                    + u32::from((*dst).phy_info.signal_quality) * 4)
                    / 5) as u8,
                ((*src).rssi + (*dst).rssi * 4) / 5,
            )
        } else {
            (
                (*dst).phy_info.signal_strength,
                (*dst).phy_info.signal_quality,
                (*dst).rssi,
            )
        };
        if update_ie {
            (*dst).reserved = (*src).reserved;
            core::ptr::copy_nonoverlapping(src as *const u8, dst as *mut u8, bss_sz(dst));
        }
        (*dst).phy_info.signal_strength = ss;
        (*dst).phy_info.signal_quality = sq;
        (*dst).rssi = rssi;
    }
}

#[no_mangle]
pub extern "C" fn update_current_network(a: *mut Adapter, pn: *mut WlanBssidEx) {
    if a.is_null() || pn.is_null() {
        return;
    }
    unsafe {
        let m = &mut (*a).mlmepriv;
        let n = &mut m.cur_network.network as *mut WlanBssidEx;
        rtw_bug_check(n as *mut _, n as *mut _, n as *mut _, n as *mut _);
        if fw_on(m, WIFI_ASOC_STATE) == _TRUE && is_same_network(n, pn, 0) == _TRUE {
            update_network(n, pn, a, true);
            rtw_update_protection(a, (*n).ies.as_mut_ptr().add(12), (*n).ie_length);
        }
    }
}
