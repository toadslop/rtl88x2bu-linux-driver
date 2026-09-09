// SPDX-License-Identifier: GPL-2.0
//! W3-73 AP STA assoc IE parse helpers — Rust port of core/rtw_ap_sta_ie*.c.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::{c_int, c_void};

type U8 = u8;
type U16 = u16;
type Adapter = c_void;
type StaInfo = c_void;
type Ieee80211Elems = c_void;

const _STATS_SUCCESSFUL_: U16 = 0;
const _STATS_FAILURE_: U16 = 1;
const _SUCCESS: c_int = 1;
const _FAIL: c_int = 0;

const WLAN_CAPABILITY_SHORT_PREAMBLE: U16 = 1 << 5;
const WLAN_STA_SHORT_PREAMBLE: i32 = 1 << 7;
const WLAN_STA_NONERP: i32 = 1 << 31;

const WLAN_EID_VENDOR_SPECIFIC: U8 = 221;
const WMM_OUI: [U8; 6] = [0x00, 0x50, 0xf2, 0x02, 0x00, 0x01];

extern "C" {
    fn _rtw_memcpy(d: *mut c_void, s: *const c_void, n: usize) -> *mut c_void;
    fn rtw_ies_get_supported_rate(
        ies: *mut U8,
        ies_len: u32,
        rate_set: *mut U8,
        rate_num: *mut U8,
    ) -> c_int;
    fn UpdateBrateTblForSoftAP(bssrateset: *mut U8, bssratelen: u32);
    fn rtw_get_ie_ex(
        in_ie: *const U8,
        in_len: u32,
        eid: U8,
        oui: *const U8,
        oui_len: U8,
        ie: *mut U8,
        ielen: *mut u32,
    ) -> *mut U8;
    fn rtw_get_multi_ap_ie_ext(ies: *const U8, ies_len: c_int) -> U8;

    fn rtw_rust_ap_sta_set_capability(sta: *mut StaInfo, cap: U16);
    fn rtw_rust_ap_sta_or_flags(sta: *mut StaInfo, mask: c_int);
    fn rtw_rust_ap_sta_andnot_flags(sta: *mut StaInfo, mask: c_int);
    fn rtw_rust_ap_sta_set_bssrates(sta: *mut StaInfo, rates: *const U8, len: u32);
    fn rtw_rust_ap_sta_finish_rates(sta: *mut StaInfo, is_ap: U8);
    fn rtw_rust_ap_mlme_qos_option(adapter: *mut Adapter) -> U8;
    fn rtw_rust_ap_mlme_ht_option(adapter: *mut Adapter) -> U8;
    fn rtw_rust_ap_mlme_vht_option(adapter: *mut Adapter) -> U8;
    fn rtw_rust_ap_mlme_is_ap(adapter: *mut Adapter) -> U8;
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_mlme_is_mesh(adapter: *mut Adapter) -> U8;
    fn rtw_rust_ap_sta_clear_wmm(sta: *mut StaInfo);
    fn rtw_rust_ap_sta_apply_wmm(sta: *mut StaInfo, qos_info: U8);
    fn rtw_rust_ap_sta_clear_ht(sta: *mut StaInfo);
    fn rtw_rust_ap_sta_apply_ht_from_elems(sta: *mut StaInfo, elems: *mut Ieee80211Elems);
    fn rtw_rust_ap_sta_clear_vht(sta: *mut StaInfo);
    fn rtw_rust_ap_sta_apply_vht_from_elems(sta: *mut StaInfo, elems: *mut Ieee80211Elems);
    fn rtw_rust_ap_adapter_multi_ap(adapter: *mut Adapter) -> U8;
    fn rtw_rust_ap_sta_clear_multi_ap(sta: *mut StaInfo);
    fn rtw_rust_ap_sta_apply_multi_ap(sta: *mut StaInfo, multi_ap: U8, role: U8);
}

#[inline]
fn le16(p: *const U8) -> U16 {
    unsafe { u16::from_le_bytes([*p, *p.add(1)]) }
}

#[cfg(rust_ap_sta_ie)]
#[no_mangle]
pub extern "C" fn rtw_ap_parse_sta_capability(
    adapter: *mut Adapter,
    sta: *mut StaInfo,
    cap: *mut U8,
) {
    let _ = adapter;
    if sta.is_null() || cap.is_null() {
        return;
    }
    let capability = le16(cap);
    unsafe {
        rtw_rust_ap_sta_set_capability(sta, capability);
        if capability & WLAN_CAPABILITY_SHORT_PREAMBLE != 0 {
            rtw_rust_ap_sta_or_flags(sta, WLAN_STA_SHORT_PREAMBLE);
        } else {
            rtw_rust_ap_sta_andnot_flags(sta, WLAN_STA_SHORT_PREAMBLE);
        }
    }
}

#[cfg(rust_ap_sta_ie)]
#[no_mangle]
pub extern "C" fn rtw_ap_parse_sta_supported_rates(
    adapter: *mut Adapter,
    sta: *mut StaInfo,
    tlv_ies: *mut U8,
    tlv_ies_len: U16,
) -> U16 {
    let mut rate_set = [0u8; 12];
    let mut rate_num = 0u8;
    let mut status = _STATS_SUCCESSFUL_;
    if sta.is_null() {
        return _STATS_FAILURE_;
    }
    unsafe {
        if rtw_ies_get_supported_rate(
            tlv_ies,
            tlv_ies_len as u32,
            rate_set.as_mut_ptr(),
            &mut rate_num,
        ) == _FAIL
            || rate_num == 0
        {
            status = _STATS_FAILURE_;
        } else {
            rtw_rust_ap_sta_set_bssrates(sta, rate_set.as_ptr(), rate_num as u32);
            rtw_rust_ap_sta_finish_rates(sta, rtw_rust_ap_mlme_is_ap(adapter));
        }
    }
    status
}

#[cfg(rust_ap_sta_ie)]
#[no_mangle]
pub extern "C" fn rtw_ap_parse_sta_wmm_ie(
    adapter: *mut Adapter,
    sta: *mut StaInfo,
    tlv_ies: *mut U8,
    tlv_ies_len: U16,
) {
    if adapter.is_null() || sta.is_null() {
        return;
    }
    unsafe {
        rtw_rust_ap_sta_clear_wmm(sta);
        if rtw_rust_ap_mlme_qos_option(adapter) == 0 {
            return;
        }
        #[cfg(config_rtw_mesh)]
        if rtw_rust_ap_mlme_is_mesh(adapter) != 0 {
            rtw_rust_ap_sta_or_flags(sta, 1 << 9);
        }
        let mut ielen = 0u32;
        let p = rtw_get_ie_ex(
            tlv_ies,
            tlv_ies_len as u32,
            WLAN_EID_VENDOR_SPECIFIC,
            WMM_OUI.as_ptr(),
            6,
            core::ptr::null_mut(),
            &mut ielen,
        );
        if p.is_null() {
            return;
        }
        rtw_rust_ap_sta_apply_wmm(sta, *p.add(8));
    }
}

#[cfg(rust_ap_sta_ie)]
#[no_mangle]
pub extern "C" fn rtw_ap_parse_sta_ht_ie(
    adapter: *mut Adapter,
    sta: *mut StaInfo,
    elems: *mut Ieee80211Elems,
) {
    if adapter.is_null() || sta.is_null() || elems.is_null() {
        return;
    }
    unsafe {
        rtw_rust_ap_sta_clear_ht(sta);
        if rtw_rust_ap_mlme_ht_option(adapter) == 0 {
            return;
        }
        rtw_rust_ap_sta_apply_ht_from_elems(sta, elems);
    }
}

#[cfg(rust_ap_sta_ie)]
#[no_mangle]
pub extern "C" fn rtw_ap_parse_sta_vht_ie(
    adapter: *mut Adapter,
    sta: *mut StaInfo,
    elems: *mut Ieee80211Elems,
) {
    if adapter.is_null() || sta.is_null() || elems.is_null() {
        return;
    }
    unsafe {
        rtw_rust_ap_sta_clear_vht(sta);
        if rtw_rust_ap_mlme_vht_option(adapter) == 0 {
            return;
        }
        rtw_rust_ap_sta_apply_vht_from_elems(sta, elems);
    }
}

#[cfg(rust_ap_sta_ie)]
#[no_mangle]
pub extern "C" fn rtw_ap_parse_sta_multi_ap_ie(
    adapter: *mut Adapter,
    sta: *mut StaInfo,
    ies: *mut U8,
    ies_len: c_int,
) {
    if adapter.is_null() || sta.is_null() {
        return;
    }
    unsafe {
        rtw_rust_ap_sta_clear_multi_ap(sta);
        let multi_ap = rtw_rust_ap_adapter_multi_ap(adapter);
        let role = rtw_get_multi_ap_ie_ext(ies, ies_len);
        rtw_rust_ap_sta_apply_multi_ap(sta, multi_ap, role);
    }
}
