// SPDX-License-Identifier: GPL-2.0
//! WLAN util helpers — Rust port of `core/rtw_wlan_util.c` slices (W3-08+).

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

#[path = "domain/types.rs"]
mod domain_types;

use domain_types::WifiRate;

#[cfg(host_wlan_util_test)]
use std::os::raw::c_int;

#[cfg(not(host_wlan_util_test))]
use core::ffi::c_int;

type U8 = u8;

const _TRUE: i32 = 1;
const _FALSE: i32 = 0;

const IEEE80211_BASIC_RATE_MASK: U8 = 0x80;
const IEEE80211_CCK_RATE_1MB: U8 = 0x02;
const IEEE80211_CCK_RATE_2MB: U8 = 0x04;
const IEEE80211_CCK_RATE_5MB: U8 = 0x0b;
const IEEE80211_CCK_RATE_11MB: U8 = 0x16;
const IEEE80211_OFDM_RATE_6MB: U8 = 0x0c;
const IEEE80211_OFDM_RATE_9MB: U8 = 0x12;
const IEEE80211_OFDM_RATE_12MB: U8 = 0x18;
const IEEE80211_OFDM_RATE_18MB: U8 = 0x24;
const IEEE80211_OFDM_RATE_24MB: U8 = 0x30;
const IEEE80211_OFDM_RATE_36MB: U8 = 0x48;
const IEEE80211_OFDM_RATE_48MB: U8 = 0x60;
const IEEE80211_OFDM_RATE_54MB: U8 = 0x6c;

static WIFI_CCKRATES: [U8; 4] = [
    IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
];

static WIFI_OFDMRATES: [U8; 8] = [
    IEEE80211_OFDM_RATE_6MB,
    IEEE80211_OFDM_RATE_9MB,
    IEEE80211_OFDM_RATE_12MB,
    IEEE80211_OFDM_RATE_18MB,
    IEEE80211_OFDM_RATE_24MB,
    IEEE80211_OFDM_RATE_36MB,
    IEEE80211_OFDM_RATE_48MB,
    IEEE80211_OFDM_RATE_54MB,
];

static RTW_BASIC_RATE_CCK: [U8; 4] = [
    IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
];

static RTW_BASIC_RATE_OFDM: [U8; 3] = [
    IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK,
];

static RTW_BASIC_RATE_MIX: [U8; 7] = [
    IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK,
];

#[no_mangle]
pub extern "C" fn rtw_rust_wlan_util_probe() -> c_int {
    0x1e08
}

fn rate_matches_table(rate: U8, table: &[U8]) -> bool {
    let idx = rate & 0x7f;
    table.iter().any(|&entry| (entry & 0x7f) == idx)
}

#[no_mangle]
pub extern "C" fn rtw_is_cck_rate(rate: U8) -> bool {
    rate_matches_table(rate, &WIFI_CCKRATES)
}

#[no_mangle]
pub extern "C" fn rtw_is_ofdm_rate(rate: U8) -> bool {
    rate_matches_table(rate, &WIFI_OFDMRATES)
}

#[no_mangle]
pub extern "C" fn rtw_is_basic_rate_cck(rate: U8) -> bool {
    rate_matches_table(rate, &RTW_BASIC_RATE_CCK)
}

#[no_mangle]
pub extern "C" fn rtw_is_basic_rate_ofdm(rate: U8) -> bool {
    rate_matches_table(rate, &RTW_BASIC_RATE_OFDM)
}

#[no_mangle]
pub extern "C" fn rtw_is_basic_rate_mix(rate: U8) -> bool {
    rate_matches_table(rate, &RTW_BASIC_RATE_MIX)
}

#[no_mangle]
pub extern "C" fn cckrates_included(rate: *mut U8, ratelen: i32) -> i32 {
    if rate.is_null() || ratelen <= 0 {
        return _FALSE;
    }
    let rates = unsafe { core::slice::from_raw_parts(rate, ratelen as usize) };
    for &r in rates {
        if let Ok(wr) = WifiRate::try_from(r) {
            if wr.is_cck() {
                return _TRUE;
            }
        }
    }
    _FALSE
}

#[no_mangle]
pub extern "C" fn cckratesonly_included(rate: *mut U8, ratelen: i32) -> i32 {
    if rate.is_null() {
        return _FALSE;
    }
    if ratelen <= 0 {
        return _TRUE;
    }
    let rates = unsafe { core::slice::from_raw_parts(rate, ratelen as usize) };
    for &r in rates {
        if let Ok(wr) = WifiRate::try_from(r) {
            if !wr.is_cck() {
                return _FALSE;
            }
        } else {
            return _FALSE;
        }
    }
    _TRUE
}
