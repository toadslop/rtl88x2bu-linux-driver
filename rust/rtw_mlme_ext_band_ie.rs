// SPDX-License-Identifier: GPL-2.0
//! W3-72 PR4: change_band_update_ie — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    unreachable_pub,
    missing_docs
)]

#[cfg(host_mlme_ext_band_ie_test)]
use std::os::raw::c_int;

#[cfg(rust_mlme_ext_band_ie)]
use core::ffi::c_int;

type U8 = u8;
type U32 = u32;
type Adapter = *mut core::ffi::c_void;
type WlanBssidEx = *mut core::ffi::c_void;

const _TRUE: c_int = 1;
const _ERPINFO_IE_: U8 = 42;
const _SUPPORTEDRATES_IE_: U8 = 1;
const _EXT_SUPPORTEDRATES_IE_: U8 = 50;
const IEEE80211_CCK_RATE_LEN: U8 = 4;
const IEEE80211_NUM_OFDM_RATESLEN: U8 = 8;
const WIRELESS_11B: U32 = 1 << 0;
const WIRELESS_11G: U32 = 1 << 1;
const WIRELESS_11A: U32 = 1 << 2;
const WIRELESS_11AC: U32 = 1 << 6;

extern "C" {
    fn rtw_rust_band_ie_ht_option(a: Adapter) -> U8;
    fn rtw_rust_band_ie_wireless_mode(a: Adapter) -> U32;
    fn rtw_rust_band_ie_vht_enable(a: Adapter) -> U8;
    fn rtw_rust_band_ie_ori_vht_en(a: Adapter) -> U8;
    fn rtw_rust_band_ie_country_en_11ac(a: Adapter) -> U8;
    fn rtw_rust_band_ie_supported_rates(n: WlanBssidEx) -> *mut U8;
    fn rtw_rust_band_ie_set_length(n: WlanBssidEx);
    fn rtw_add_bcn_ie(a: Adapter, n: WlanBssidEx, index: U8, data: *mut U8, len: U8);
    fn rtw_remove_bcn_ie(a: Adapter, n: WlanBssidEx, index: U8);
    fn rtw_set_supported_rate(rates: *mut U8, mode: U32);
    fn UpdateBrateTbl(a: Adapter, rates: *mut U8);
    #[cfg(any(config_80211ac_vht, host_mlme_ext_band_ie_test))]
    fn rtw_vht_ies_attach(a: Adapter, n: WlanBssidEx);
    #[cfg(any(config_80211ac_vht, host_mlme_ext_band_ie_test))]
    fn rtw_vht_ies_detach(a: Adapter, n: WlanBssidEx);
}

#[inline]
fn regsty_is_11ac_enable(vht_enable: U8) -> bool {
    vht_enable != 0
}

#[inline]
fn regsty_is_11ac_auto(vht_enable: U8) -> bool {
    vht_enable == 2
}

#[inline]
fn is_supported_vht(wireless_mode: U32) -> bool {
    (wireless_mode & WIRELESS_11AC) != 0
}

#[no_mangle]
pub extern "C" fn change_band_update_ie(padapter: Adapter, pnetwork: WlanBssidEx, ch: U8) {
    if padapter.is_null() || pnetwork.is_null() {
        return;
    }

    let mut network_type: U32 = 0;
    let rate_len: U8;
    let mut total_rate_len: U8;
    let remainder_rate_len: U8;
    let erpinfo: U8 = 0x4;

    if ch >= 36 {
        network_type = WIRELESS_11A;
        total_rate_len = IEEE80211_NUM_OFDM_RATESLEN;
        unsafe {
            rtw_remove_bcn_ie(padapter, pnetwork, _ERPINFO_IE_);
        }
        #[cfg(any(config_80211ac_vht, host_mlme_ext_band_ie_test))]
        {
            let ht = unsafe { rtw_rust_band_ie_ht_option(padapter) } == _TRUE as U8;
            let wm = unsafe { rtw_rust_band_ie_wireless_mode(padapter) };
            let vht_en = unsafe { rtw_rust_band_ie_vht_enable(padapter) };
            let country_ok = unsafe { rtw_rust_band_ie_country_en_11ac(padapter) } != 0;
            if ht && regsty_is_11ac_enable(vht_en) && is_supported_vht(wm) && country_ok {
                let auto_or_ori = regsty_is_11ac_auto(vht_en)
                    || unsafe { rtw_rust_band_ie_ori_vht_en(padapter) } != 0;
                if auto_or_ori {
                    unsafe {
                        rtw_vht_ies_attach(padapter, pnetwork);
                    }
                }
            }
        }
    } else {
        total_rate_len = 0;
        let wm = unsafe { rtw_rust_band_ie_wireless_mode(padapter) };
        if (wm & WIRELESS_11B) != 0 {
            network_type |= WIRELESS_11B;
            total_rate_len = total_rate_len.saturating_add(IEEE80211_CCK_RATE_LEN);
        }
        if (wm & WIRELESS_11G) != 0 {
            network_type |= WIRELESS_11G;
            total_rate_len = total_rate_len.saturating_add(IEEE80211_NUM_OFDM_RATESLEN);
        }
        unsafe {
            rtw_add_bcn_ie(
                padapter,
                pnetwork,
                _ERPINFO_IE_,
                &erpinfo as *const U8 as *mut U8,
                1,
            );
        }
        #[cfg(any(config_80211ac_vht, host_mlme_ext_band_ie_test))]
        unsafe {
            rtw_vht_ies_detach(padapter, pnetwork);
        }
    }

    let rates = unsafe { rtw_rust_band_ie_supported_rates(pnetwork) };
    unsafe {
        rtw_set_supported_rate(rates, network_type);
        UpdateBrateTbl(padapter, rates);
    }

    if total_rate_len > 8 {
        rate_len = 8;
        remainder_rate_len = total_rate_len - 8;
    } else {
        rate_len = total_rate_len;
        remainder_rate_len = 0;
    }

    unsafe {
        rtw_add_bcn_ie(padapter, pnetwork, _SUPPORTEDRATES_IE_, rates, rate_len);
        if remainder_rate_len != 0 {
            rtw_add_bcn_ie(
                padapter,
                pnetwork,
                _EXT_SUPPORTEDRATES_IE_,
                rates.add(8),
                remainder_rate_len,
            );
        } else {
            rtw_remove_bcn_ie(padapter, pnetwork, _EXT_SUPPORTEDRATES_IE_);
        }
        rtw_rust_band_ie_set_length(pnetwork);
    }
}
