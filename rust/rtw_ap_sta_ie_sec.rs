// SPDX-License-Identifier: GPL-2.0
//! W3-74 AP STA security IE parse — Rust port of `rtw_ap_parse_sta_security_ie`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(rust_ap_sta_ie_sec)]
use core::ffi::{c_int, c_void};

#[cfg(rust_ap_sta_ie_sec)]
type U8 = u8;
#[cfg(rust_ap_sta_ie_sec)]
type U16 = u16;
#[cfg(rust_ap_sta_ie_sec)]
type U32 = u32;
#[cfg(rust_ap_sta_ie_sec)]
type Adapter = c_void;
#[cfg(rust_ap_sta_ie_sec)]
type StaInfo = c_void;
#[cfg(rust_ap_sta_ie_sec)]
type SecurityPriv = c_void;
#[cfg(rust_ap_sta_ie_sec)]
type Ieee80211Elems = c_void;

#[cfg(rust_ap_sta_ie_sec)]
const _STATS_SUCCESSFUL_: U16 = 0;
#[cfg(rust_ap_sta_ie_sec)]
const MFP_NO: U8 = 0;

#[cfg(rust_ap_sta_ie_sec)]
extern "C" {
    fn rtw_rust_ap_sta_sec_reset(sta: *mut StaInfo);
    fn rtw_rust_ap_sec_priv(adapter: *mut Adapter) -> *mut SecurityPriv;

    fn rtw_ap_sta_sec_parse_cipher_ies(
        adapter: *mut Adapter,
        sta: *mut StaInfo,
        sec: *mut SecurityPriv,
        elems: *mut Ieee80211Elems,
        wpa_ie: *mut *mut U8,
        wpa_ie_len: *mut c_int,
        group_cipher: *mut c_int,
        pairwise_cipher: *mut c_int,
        gmcs: *mut c_int,
        akm: *mut U32,
        mfp_opt: *mut U8,
        spp_opt: *mut U8,
    ) -> U16;

    fn rtw_ap_sta_sec_apply_policy_wps(
        adapter: *mut Adapter,
        sta: *mut StaInfo,
        sec: *mut SecurityPriv,
        elems: *mut Ieee80211Elems,
        wpa_ie: *mut U8,
        wpa_ie_len: c_int,
        gmcs: c_int,
        mfp_opt: U8,
        spp_opt: U8,
    ) -> U16;
}

#[cfg(rust_ap_sta_ie_sec)]
#[no_mangle]
pub extern "C" fn rtw_ap_parse_sta_security_ie(
    adapter: *mut Adapter,
    sta: *mut StaInfo,
    elems: *mut Ieee80211Elems,
) -> U16 {
    let mut wpa_ie: *mut U8 = core::ptr::null_mut();
    let mut wpa_ie_len: c_int = 0;
    let mut group_cipher: c_int = 0;
    let mut pairwise_cipher: c_int = 0;
    let mut gmcs: c_int = 0;
    let mut akm: U32 = 0;
    let mut mfp_opt: U8 = MFP_NO;
    let mut spp_opt: U8 = 0;

    unsafe {
        rtw_rust_ap_sta_sec_reset(sta);
        let sec = rtw_rust_ap_sec_priv(adapter);
        let status = rtw_ap_sta_sec_parse_cipher_ies(
            adapter,
            sta,
            sec,
            elems,
            &mut wpa_ie,
            &mut wpa_ie_len,
            &mut group_cipher,
            &mut pairwise_cipher,
            &mut gmcs,
            &mut akm,
            &mut mfp_opt,
            &mut spp_opt,
        );
        if status != _STATS_SUCCESSFUL_ {
            return status;
        }
        rtw_ap_sta_sec_apply_policy_wps(
            adapter, sta, sec, elems, wpa_ie, wpa_ie_len, gmcs, mfp_opt, spp_opt,
        )
    }
}
