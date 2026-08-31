// SPDX-License-Identifier: GPL-2.0
//! MLME rest helpers — Rust port of `core/rtw_mlme_rest.c` BSSID/compare slice (W3-53).

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    missing_docs
)]

#[cfg(not(host_mlme_test))]
use core::ffi::{c_int, c_void};
#[cfg(host_mlme_test)]
use std::os::raw::{c_int, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;
const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const ETH_ALEN: usize = 6;
const _NO_PRIVACY_: U32 = 0;

#[cfg(host_mlme_test)]
#[repr(C)]
struct HostMlmeBssidEx {
    length: U32,
    mac_address: [U8; ETH_ALEN],
    reserved: [U8; 2],
    ssid_length: U32,
    ssid: [U8; 32],
    mesh_id_length: U32,
    mesh_id: [U8; 32],
    privacy: U32,
    rssi: i64,
    configuration: [U8; 16],
    infrastructure_mode: U32,
    supported_rates: [U8; 16],
    phy_info: [U8; 8],
    ie_length: U32,
    ies: [U8; 768],
}
#[cfg(host_mlme_test)]
#[repr(C)]
struct HostMlmeNetwork {
    network: HostMlmeBssidEx,
}
#[cfg(host_mlme_test)]
#[repr(C)]
struct HostMlmeAdapter {
    securitypriv: HostSecurityPriv,
}
#[cfg(host_mlme_test)]
#[repr(C)]
struct HostSecurityPriv {
    dot11_privacy_algrthm: U32,
}

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn is_all_null(c: *mut i8, len: c_int) -> c_int;
    fn rtw_random32() -> U32;
}
#[cfg(not(host_mlme_test))]
extern "C" {
    fn _rtw_memcpy(d: *mut c_void, s: *const c_void, n: usize) -> *mut c_void;
    fn rtw_mlme_rest_bss_ies(b: *mut c_void) -> *mut U8;
    fn rtw_mlme_rest_bss_ssid_length(b: *mut c_void) -> *mut U32;
    fn rtw_mlme_rest_bss_ssid(b: *mut c_void) -> *mut U8;
    fn rtw_mlme_rest_bss_mac(b: *mut c_void) -> *mut U8;
    fn rtw_mlme_rest_network_privacy(p: *mut c_void) -> *mut U32;
    fn rtw_mlme_rest_adapter_privacy(a: *mut c_void) -> *mut U32;
}

#[inline]
fn cap_ptr(ie: *mut U8) -> *mut U8 {
    unsafe { ie.add(10) }
}
fn memcpy_bytes(dst: *mut u8, src: *const u8, n: usize) {
    #[cfg(host_mlme_test)]
    unsafe {
        core::ptr::copy_nonoverlapping(src, dst, n)
    }
    #[cfg(not(host_mlme_test))]
    unsafe {
        _rtw_memcpy(dst as *mut c_void, src as *const c_void, n);
    }
}
fn memcmp_bytes(a: *const u8, b: *const u8, n: usize) -> bool {
    unsafe { _rtw_memcmp(a as *const c_void, b as *const c_void, n) == _TRUE }
}
fn all_null_bytes(s: *mut u8, len: u32) -> bool {
    unsafe { is_all_null(s as *mut i8, len as c_int) == _TRUE }
}
fn read_le_u16_from_cap(ie: *mut U8) -> U16 {
    let p = cap_ptr(ie);
    unsafe { u16::from_le_bytes([*p, *p.add(1)]) }
}

#[no_mangle]
pub extern "C" fn rtw_generate_random_ibss(pibss: *mut U8) {
    if pibss.is_null() {
        return;
    }
    unsafe {
        core::ptr::write_unaligned(pibss.add(2) as *mut U32, rtw_random32());
        *pibss.add(0) = 0x02;
        *pibss.add(1) = 0x11;
        *pibss.add(2) = 0x87;
    }
}
#[no_mangle]
pub extern "C" fn rtw_get_capability_from_ie(ie: *mut U8) -> *mut U8 {
    cap_ptr(ie)
}
#[no_mangle]
pub extern "C" fn rtw_get_timestampe_from_ie(ie: *mut U8) -> *mut U8 {
    ie
}
#[no_mangle]
pub extern "C" fn rtw_get_beacon_interval_from_ie(ie: *mut U8) -> *mut U8 {
    unsafe { ie.add(8) }
}
#[no_mangle]
pub extern "C" fn rtw_get_capability(bss: *mut c_void) -> U16 {
    if bss.is_null() {
        return 0;
    }
    read_le_u16_from_cap(bss_ies(bss))
}
#[no_mangle]
pub extern "C" fn rtw_is_same_ibss(adapter: *mut c_void, pnetwork: *mut c_void) -> c_int {
    if adapter.is_null() || pnetwork.is_null() {
        return _FALSE;
    }
    let ap = unsafe { *adapter_privacy(adapter) };
    let np = unsafe { *network_privacy(pnetwork) };
    if ap != _NO_PRIVACY_ && np == 0 {
        _FALSE
    } else if ap == _NO_PRIVACY_ && np == 1 {
        _FALSE
    } else {
        _TRUE
    }
}
#[no_mangle]
pub extern "C" fn is_same_ess(a: *mut c_void, b: *mut c_void) -> c_int {
    if a.is_null() || b.is_null() {
        return _FALSE;
    }
    let (al, bl) = unsafe { (*bss_ssid_length(a), *bss_ssid_length(b)) };
    if al != bl {
        return _FALSE;
    }
    if memcmp_bytes(bss_ssid(a), bss_ssid(b), al as usize) {
        _TRUE
    } else {
        _FALSE
    }
}
#[no_mangle]
pub extern "C" fn is_same_network(src: *mut c_void, dst: *mut c_void, feature: U8) -> c_int {
    if src.is_null() || dst.is_null() {
        return _FALSE;
    }
    #[cfg(config_p2p)]
    {
        if feature == 1 && memcmp_bytes(bss_mac(src), bss_mac(dst), ETH_ALEN) {
            return _TRUE;
        }
    }
    let s_cap = read_le_u16_from_cap(bss_ies(src));
    let d_cap = read_le_u16_from_cap(bss_ies(dst));
    if memcmp_bytes(bss_mac(src), bss_mac(dst), ETH_ALEN)
        && (s_cap & 2) == (d_cap & 2)
        && (s_cap & 1) == (d_cap & 1)
    {
        let (sl, dl) = unsafe { (*bss_ssid_length(src), *bss_ssid_length(dst)) };
        if sl == dl
            && (memcmp_bytes(bss_ssid(src), bss_ssid(dst), sl as usize)
                || all_null_bytes(bss_ssid(src), sl)
                || all_null_bytes(bss_ssid(dst), dl))
        {
            return _TRUE;
        }
        if sl == 0 || dl == 0 {
            return _TRUE;
        }
        return _FALSE;
    }
    _FALSE
}

fn bss_ies(b: *mut c_void) -> *mut U8 {
    #[cfg(host_mlme_test)]
    unsafe {
        (&mut (*(b as *mut HostMlmeBssidEx)).ies) as *mut U8
    }
    #[cfg(not(host_mlme_test))]
    unsafe {
        rtw_mlme_rest_bss_ies(b)
    }
}
fn bss_ssid_length(b: *mut c_void) -> *mut U32 {
    #[cfg(host_mlme_test)]
    unsafe {
        &mut (*(b as *mut HostMlmeBssidEx)).ssid_length
    }
    #[cfg(not(host_mlme_test))]
    unsafe {
        rtw_mlme_rest_bss_ssid_length(b)
    }
}
fn bss_ssid(b: *mut c_void) -> *mut U8 {
    #[cfg(host_mlme_test)]
    unsafe {
        (*(b as *mut HostMlmeBssidEx)).ssid.as_mut_ptr()
    }
    #[cfg(not(host_mlme_test))]
    unsafe {
        rtw_mlme_rest_bss_ssid(b)
    }
}
fn bss_mac(b: *mut c_void) -> *mut U8 {
    #[cfg(host_mlme_test)]
    unsafe {
        (*(b as *mut HostMlmeBssidEx)).mac_address.as_mut_ptr()
    }
    #[cfg(not(host_mlme_test))]
    unsafe {
        rtw_mlme_rest_bss_mac(b)
    }
}
fn adapter_privacy(a: *mut c_void) -> *mut U32 {
    #[cfg(host_mlme_test)]
    unsafe {
        &mut (*(a as *mut HostMlmeAdapter))
            .securitypriv
            .dot11_privacy_algrthm
    }
    #[cfg(not(host_mlme_test))]
    unsafe {
        rtw_mlme_rest_adapter_privacy(a)
    }
}
fn network_privacy(p: *mut c_void) -> *mut U32 {
    #[cfg(host_mlme_test)]
    unsafe {
        &mut (*(p as *mut HostMlmeNetwork)).network.privacy
    }
    #[cfg(not(host_mlme_test))]
    unsafe {
        rtw_mlme_rest_network_privacy(p)
    }
}

#[cfg(rust_mlme_wmm_rsn)]
#[path = "rtw_mlme_wmm_rsn.rs"]
mod wmm_rsn;

#[cfg(rust_mlme_unassoc)]
#[path = "rtw_mlme_unassoc.rs"]
mod unassoc;
