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
    if rate.is_null() || ratelen <= 0 {
        return _FALSE;
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

const NUM_RATES: usize = 13;
const RATE_6M: U8 = 4;

#[cfg(host_wlan_util_test)]
#[repr(C)]
struct HostMlmeExtPriv {
    basicrate: [U8; NUM_RATES],
    datarate: [U8; NUM_RATES],
}

#[cfg(host_wlan_util_test)]
#[repr(C)]
struct HostWlanAdapter {
    oper_ch: U8,
    mlmeextpriv: HostMlmeExtPriv,
}

#[cfg(not(host_wlan_util_test))]
mod kernel_layout {
    use super::U8;

    /// `offsetof(struct _adapter, mlmeextpriv.basicrate)` — re-run L1 after layout changes.
    pub const MLMEEXT_BASICRATE_OFFSET: usize = 0x848;
    /// `offsetof(struct _adapter, mlmeextpriv.datarate)`
    pub const MLMEEXT_DATARATE_OFFSET: usize = 0x855;

    extern "C" {
        pub fn rtw_get_oper_ch(adapter: *mut core::ffi::c_void) -> U8;
    }
}

fn ratetbl_val_2wifirate_inner(rate: U8) -> U8 {
    match rate & 0x7f {
        0 => IEEE80211_CCK_RATE_1MB,
        1 => IEEE80211_CCK_RATE_2MB,
        2 => IEEE80211_CCK_RATE_5MB,
        3 => IEEE80211_CCK_RATE_11MB,
        4 => IEEE80211_OFDM_RATE_6MB,
        5 => IEEE80211_OFDM_RATE_9MB,
        6 => IEEE80211_OFDM_RATE_12MB,
        7 => IEEE80211_OFDM_RATE_18MB,
        8 => IEEE80211_OFDM_RATE_24MB,
        9 => IEEE80211_OFDM_RATE_36MB,
        10 => IEEE80211_OFDM_RATE_48MB,
        11 => IEEE80211_OFDM_RATE_54MB,
        _ => 0,
    }
}

#[no_mangle]
pub extern "C" fn ratetbl_val_2wifirate(rate: U8) -> U8 {
    ratetbl_val_2wifirate_inner(rate)
}

unsafe fn oper_ch(padapter: *mut U8) -> U8 {
    #[cfg(host_wlan_util_test)]
    {
        (*padapter.cast::<HostWlanAdapter>()).oper_ch
    }
    #[cfg(not(host_wlan_util_test))]
    {
        unsafe { kernel_layout::rtw_get_oper_ch(padapter.cast()) }
    }
}

unsafe fn basicrate_slot(padapter: *mut U8, i: usize) -> U8 {
    #[cfg(host_wlan_util_test)]
    {
        (*padapter.cast::<HostWlanAdapter>()).mlmeextpriv.basicrate[i]
    }
    #[cfg(not(host_wlan_util_test))]
    {
        unsafe { *padapter.add(kernel_layout::MLMEEXT_BASICRATE_OFFSET + i) }
    }
}

unsafe fn datarate_slot(padapter: *mut U8, i: usize) -> U8 {
    #[cfg(host_wlan_util_test)]
    {
        (*padapter.cast::<HostWlanAdapter>()).mlmeextpriv.datarate[i]
    }
    #[cfg(not(host_wlan_util_test))]
    {
        unsafe { *padapter.add(kernel_layout::MLMEEXT_DATARATE_OFFSET + i) }
    }
}

#[no_mangle]
pub extern "C" fn is_basicrate(padapter: *mut U8, rate: U8) -> i32 {
    if padapter.is_null() {
        return _FALSE;
    }
    unsafe {
        for i in 0..NUM_RATES {
            let val = basicrate_slot(padapter, i);
            if val != 0xff && val != 0xfe {
                if rate == ratetbl_val_2wifirate_inner(val) {
                    return _TRUE;
                }
            }
        }
    }
    _FALSE
}

#[no_mangle]
pub extern "C" fn ratetbl2rateset(padapter: *mut U8, rateset: *mut U8) -> u32 {
    if padapter.is_null() || rateset.is_null() {
        return 0;
    }
    let mut len: u32 = 0;
    unsafe {
        let oper = oper_ch(padapter);
        for i in 0..NUM_RATES {
            let mut rate = datarate_slot(padapter, i);
            if oper > 14 && rate < RATE_6M {
                continue;
            }
            match rate {
                0xff => return len,
                0xfe => continue,
                _ => {
                    rate = ratetbl_val_2wifirate_inner(rate);
                    if is_basicrate(padapter, rate) == _TRUE {
                        rate |= IEEE80211_BASIC_RATE_MASK;
                    }
                    *rateset.add(len as usize) = rate;
                    len += 1;
                }
            }
        }
    }
    len
}
