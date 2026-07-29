// SPDX-License-Identifier: GPL-2.0
//! IEEE 802.11 rest helpers — Rust port of `core/rtw_ieee80211_rest.c` rate
//! classification slice (W3-26) and WPA/RSN cipher suite getters (W3-27).

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

#[cfg(host_ieee80211_rest_test)]
use std::os::raw::{c_int, c_uint};

#[cfg(not(host_ieee80211_rest_test))]
use core::ffi::{c_int, c_uint, c_void};

type U8 = u8;
type Sint = i32;

const _TRUE: i32 = 1;
const _FALSE: i32 = 0;
const _SUCCESS: i32 = 1;

const IEEE80211_BASIC_RATE_MASK: U8 = 0x80;
const IEEE80211_CCK_RATE_LEN: usize = 4;
const IEEE80211_NUM_OFDM_RATESLEN: usize = 8;
const NDIS_802_11_LENGTH_RATES_EX: usize = 16;

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

const WIRELESS_INVALID: u32 = 0;
const WIRELESS_11B: u32 = 1 << 0;
const WIRELESS_11G: u32 = 1 << 1;
const WIRELESS_11A: u32 = 1 << 2;
const WIRELESS_11BG: u32 = WIRELESS_11B | WIRELESS_11G;
const WIRELESS_11_5N: u32 = 1 << 4;
const WIRELESS_11A_5N: u32 = WIRELESS_11A | WIRELESS_11_5N;
const WIRELESS_11G_24N: u32 = WIRELESS_11G | (1 << 3);
const WIRELESS_11_24N: u32 = 1 << 3;
const WIRELESS_11BG_24N: u32 = WIRELESS_11B | WIRELESS_11G | (1 << 3);
const WIRELESS_11_5AC: u32 = 1 << 6;

const CCK: U8 = 0;
const OFDM: U8 = 1;

const _BEACON_IE_OFFSET_: usize = 12;
const _SUPPORTEDRATES_IE_: U8 = 1;
const _EXT_SUPPORTEDRATES_IE_: U8 = 50;

const WPA_SELECTOR_LEN: usize = 4;
const RSN_SELECTOR_LEN: usize = 4;

const WPA_CIPHER_NONE: i32 = 1 << 0;
const WPA_CIPHER_WEP40: i32 = 1 << 1;
const WPA_CIPHER_WEP104: i32 = 1 << 2;
const WPA_CIPHER_TKIP: i32 = 1 << 3;
const WPA_CIPHER_CCMP: i32 = 1 << 4;
const WPA_CIPHER_GCMP: i32 = 1 << 5;
const WPA_CIPHER_GCMP_256: i32 = 1 << 6;
const WPA_CIPHER_CCMP_256: i32 = 1 << 7;
const WPA_CIPHER_BIP_CMAC_128: i32 = 1 << 8;
const WPA_CIPHER_BIP_GMAC_128: i32 = 1 << 9;
const WPA_CIPHER_BIP_GMAC_256: i32 = 1 << 10;
const WPA_CIPHER_BIP_CMAC_256: i32 = 1 << 11;

const WLAN_AKM_TYPE_8021X: u32 = 1 << 0;
const WLAN_AKM_TYPE_PSK: u32 = 1 << 1;
const WLAN_AKM_TYPE_FT_8021X: u32 = 1 << 2;
const WLAN_AKM_TYPE_FT_PSK: u32 = 1 << 3;
const WLAN_AKM_TYPE_8021X_SHA256: u32 = 1 << 4;
const WLAN_AKM_TYPE_PSK_SHA256: u32 = 1 << 5;
const WLAN_AKM_TYPE_TDLS: u32 = 1 << 6;
const WLAN_AKM_TYPE_SAE: u32 = 1 << 7;
const WLAN_AKM_TYPE_FT_OVER_SAE: u32 = 1 << 8;
const WLAN_AKM_TYPE_8021X_SUITE_B: u32 = 1 << 9;
const WLAN_AKM_TYPE_8021X_SUITE_B_192: u32 = 1 << 10;
const WLAN_AKM_TYPE_FILS_SHA256: u32 = 1 << 11;
const WLAN_AKM_TYPE_FILS_SHA384: u32 = 1 << 12;
const WLAN_AKM_TYPE_FT_FILS_SHA256: u32 = 1 << 13;
const WLAN_AKM_TYPE_FT_FILS_SHA384: u32 = 1 << 14;

const WPA_CIPHER_SUITE_NONE: [U8; 4] = [0x00, 0x50, 0xf2, 0];
const WPA_CIPHER_SUITE_WEP40: [U8; 4] = [0x00, 0x50, 0xf2, 1];
const WPA_CIPHER_SUITE_TKIP: [U8; 4] = [0x00, 0x50, 0xf2, 2];
const WPA_CIPHER_SUITE_CCMP: [U8; 4] = [0x00, 0x50, 0xf2, 4];
const WPA_CIPHER_SUITE_WEP104: [U8; 4] = [0x00, 0x50, 0xf2, 5];

const RSN_CIPHER_SUITE_NONE: [U8; 4] = [0x00, 0x0f, 0xac, 0];
const RSN_CIPHER_SUITE_WEP40: [U8; 4] = [0x00, 0x0f, 0xac, 1];
const RSN_CIPHER_SUITE_TKIP: [U8; 4] = [0x00, 0x0f, 0xac, 2];
const RSN_CIPHER_SUITE_CCMP: [U8; 4] = [0x00, 0x0f, 0xac, 4];
const RSN_CIPHER_SUITE_AES_128_CMAC: [U8; 4] = [0x00, 0x0f, 0xac, 6];
const RSN_CIPHER_SUITE_GCMP: [U8; 4] = [0x00, 0x0f, 0xac, 8];
const RSN_CIPHER_SUITE_GCMP_256: [U8; 4] = [0x00, 0x0f, 0xac, 9];
const RSN_CIPHER_SUITE_CCMP_256: [U8; 4] = [0x00, 0x0f, 0xac, 10];
const RSN_CIPHER_SUITE_BIP_GMAC_128: [U8; 4] = [0x00, 0x0f, 0xac, 11];
const RSN_CIPHER_SUITE_BIP_GMAC_256: [U8; 4] = [0x00, 0x0f, 0xac, 12];
const RSN_CIPHER_SUITE_BIP_CMAC_256: [U8; 4] = [0x00, 0x0f, 0xac, 13];
const RSN_CIPHER_SUITE_WEP104: [U8; 4] = [0x00, 0x0f, 0xac, 5];

const WLAN_AKM_8021X: [U8; 4] = [0x00, 0x0f, 0xac, 1];
const WLAN_AKM_PSK: [U8; 4] = [0x00, 0x0f, 0xac, 2];
const WLAN_AKM_FT_8021X: [U8; 4] = [0x00, 0x0f, 0xac, 3];
const WLAN_AKM_FT_PSK: [U8; 4] = [0x00, 0x0f, 0xac, 4];
const WLAN_AKM_8021X_SHA256: [U8; 4] = [0x00, 0x0f, 0xac, 5];
const WLAN_AKM_PSK_SHA256: [U8; 4] = [0x00, 0x0f, 0xac, 6];
const WLAN_AKM_TDLS: [U8; 4] = [0x00, 0x0f, 0xac, 7];
const WLAN_AKM_SAE: [U8; 4] = [0x00, 0x0f, 0xac, 8];
const WLAN_AKM_FT_OVER_SAE: [U8; 4] = [0x00, 0x0f, 0xac, 9];
const WLAN_AKM_8021X_SUITE_B: [U8; 4] = [0x00, 0x0f, 0xac, 11];
const WLAN_AKM_8021X_SUITE_B_192: [U8; 4] = [0x00, 0x0f, 0xac, 12];
const WLAN_AKM_FILS_SHA256: [U8; 4] = [0x00, 0x0f, 0xac, 14];
const WLAN_AKM_FILS_SHA384: [U8; 4] = [0x00, 0x0f, 0xac, 15];
const WLAN_AKM_FT_FILS_SHA256: [U8; 4] = [0x00, 0x0f, 0xac, 16];
const WLAN_AKM_FT_FILS_SHA384: [U8; 4] = [0x00, 0x0f, 0xac, 17];

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

#[cfg(host_ieee80211_rest_test)]
#[repr(C)]
pub struct HostNdisConfiguration {
    pub length: u32,
    pub beacon_period: u32,
    pub atim_window: u32,
    pub ds_config: u32,
}

#[cfg(host_ieee80211_rest_test)]
#[repr(C)]
pub struct HostWlanPhyInfo {
    pub signal_strength: u8,
    pub signal_quality: u8,
    pub optimum_antenna: u8,
}

#[cfg(host_ieee80211_rest_test)]
#[repr(C)]
pub struct HostWlanBssidEx {
    pub length: u32,
    pub mac_address: [u8; 6],
    pub reserved: [u8; 2],
    pub ssid_len: u32,
    pub ssid: [u8; 32],
    pub mesh_id_len: u32,
    pub mesh_id: [u8; 32],
    pub privacy: u32,
    pub rssi: i64,
    pub configuration: HostNdisConfiguration,
    pub infrastructure_mode: u32,
    pub supported_rates: [u8; 16],
    pub phy_info: HostWlanPhyInfo,
    pub ie_length: u32,
    pub ies: [u8; 256],
}

extern "C" {
    fn memcpy(dst: *mut u8, src: *const u8, n: usize) -> *mut u8;
    fn memmove(dst: *mut u8, src: *const u8, n: usize) -> *mut u8;
    fn memset(s: *mut u8, c: i32, n: usize) -> *mut u8;
    fn memcmp(s1: *const u8, s2: *const u8, n: usize) -> i32;

    fn rtw_get_ie(pbuf: *const u8, index: Sint, len: *mut Sint, limit: Sint) -> *mut u8;
    fn rtw_ies_remove_ie(
        ies: *mut u8,
        ies_len: *mut c_uint,
        offset: c_uint,
        eid: U8,
        oui: *mut U8,
        oui_len: U8,
    ) -> c_int;
    fn rtw_is_cck_rate(rate: U8) -> bool;
    fn rtw_is_ofdm_rate(rate: U8) -> bool;
    fn rtw_is_basic_rate_ofdm(rate: U8) -> bool;

    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_dsconfig(bss: *mut c_void) -> *mut u32;
    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_ielength(bss: *mut c_void) -> *mut u32;
    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_ies(bss: *mut c_void) -> *mut u8;
    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_supported_rates(bss: *mut c_void) -> *mut u8;
}

fn bit(i: u32) -> i32 {
    1i32 << i
}

fn suite_matches(s: *const U8, suite: &[U8; 4], len: usize) -> bool {
    if s.is_null() {
        return false;
    }
    unsafe { memcmp(s, suite.as_ptr(), len) == 0 }
}

#[no_mangle]
pub extern "C" fn rtw_get_wpa_cipher_suite(s: *mut U8) -> c_int {
    if suite_matches(s, &WPA_CIPHER_SUITE_NONE, WPA_SELECTOR_LEN) {
        return WPA_CIPHER_NONE;
    }
    if suite_matches(s, &WPA_CIPHER_SUITE_WEP40, WPA_SELECTOR_LEN) {
        return WPA_CIPHER_WEP40;
    }
    if suite_matches(s, &WPA_CIPHER_SUITE_TKIP, WPA_SELECTOR_LEN) {
        return WPA_CIPHER_TKIP;
    }
    if suite_matches(s, &WPA_CIPHER_SUITE_CCMP, WPA_SELECTOR_LEN) {
        return WPA_CIPHER_CCMP;
    }
    if suite_matches(s, &WPA_CIPHER_SUITE_WEP104, WPA_SELECTOR_LEN) {
        return WPA_CIPHER_WEP104;
    }
    0
}

#[no_mangle]
pub extern "C" fn rtw_get_rsn_cipher_suite(s: *mut U8) -> c_int {
    if suite_matches(s, &RSN_CIPHER_SUITE_NONE, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_NONE;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_WEP40, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_WEP40;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_TKIP, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_TKIP;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_CCMP, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_CCMP;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_GCMP, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_GCMP;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_GCMP_256, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_GCMP_256;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_CCMP_256, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_CCMP_256;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_WEP104, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_WEP104;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_AES_128_CMAC, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_BIP_CMAC_128;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_BIP_GMAC_128, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_BIP_GMAC_128;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_BIP_GMAC_256, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_BIP_GMAC_256;
    }
    if suite_matches(s, &RSN_CIPHER_SUITE_BIP_CMAC_256, RSN_SELECTOR_LEN) {
        return WPA_CIPHER_BIP_CMAC_256;
    }
    0
}

#[no_mangle]
pub extern "C" fn rtw_get_akm_suite_bitmap(s: *mut U8) -> u32 {
    if suite_matches(s, &WLAN_AKM_8021X, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_8021X;
    }
    if suite_matches(s, &WLAN_AKM_PSK, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_PSK;
    }
    if suite_matches(s, &WLAN_AKM_FT_8021X, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_FT_8021X;
    }
    if suite_matches(s, &WLAN_AKM_FT_PSK, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_FT_PSK;
    }
    if suite_matches(s, &WLAN_AKM_8021X_SHA256, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_8021X_SHA256;
    }
    if suite_matches(s, &WLAN_AKM_PSK_SHA256, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_PSK_SHA256;
    }
    if suite_matches(s, &WLAN_AKM_TDLS, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_TDLS;
    }
    if suite_matches(s, &WLAN_AKM_SAE, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_SAE;
    }
    if suite_matches(s, &WLAN_AKM_FT_OVER_SAE, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_FT_OVER_SAE;
    }
    if suite_matches(s, &WLAN_AKM_8021X_SUITE_B, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_8021X_SUITE_B;
    }
    if suite_matches(s, &WLAN_AKM_8021X_SUITE_B_192, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_8021X_SUITE_B_192;
    }
    if suite_matches(s, &WLAN_AKM_FILS_SHA256, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_FILS_SHA256;
    }
    if suite_matches(s, &WLAN_AKM_FILS_SHA384, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_FILS_SHA384;
    }
    if suite_matches(s, &WLAN_AKM_FT_FILS_SHA256, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_FT_FILS_SHA256;
    }
    if suite_matches(s, &WLAN_AKM_FT_FILS_SHA384, RSN_SELECTOR_LEN) {
        return WLAN_AKM_TYPE_FT_FILS_SHA384;
    }
    0
}

fn is_cck_rate_byte(rate: U8) -> bool {
    let masked = rate & 0x7f;
    masked == 2 || masked == 4 || masked == 11 || masked == 22
}

// C dereferences unconditionally; in-tree callers always pass valid pointers.
fn is_cckrates_included(rate: *const U8) -> bool {
    if rate.is_null() {
        return false;
    }
    unsafe {
        let mut i = 0usize;
        while *rate.add(i) != 0 {
            if is_cck_rate_byte(*rate.add(i)) {
                return true;
            }
            i += 1;
        }
    }
    false
}

// C dereferences unconditionally; in-tree callers always pass valid pointers.
fn is_cckratesonly_included(rate: *const U8) -> bool {
    if rate.is_null() {
        return false;
    }
    unsafe {
        let mut i = 0usize;
        while *rate.add(i) != 0 {
            if !is_cck_rate_byte(*rate.add(i)) {
                return false;
            }
            i += 1;
        }
    }
    true
}

#[cfg(host_ieee80211_rest_test)]
fn bss_dsconfig(bss: *mut HostWlanBssidEx) -> *mut u32 {
    unsafe { &mut (*bss).configuration.ds_config }
}

#[cfg(host_ieee80211_rest_test)]
fn bss_ielength(bss: *mut HostWlanBssidEx) -> *mut u32 {
    unsafe { &mut (*bss).ie_length }
}

#[cfg(host_ieee80211_rest_test)]
fn bss_ies(bss: *mut HostWlanBssidEx) -> *mut u8 {
    unsafe { (*bss).ies.as_mut_ptr() }
}

#[cfg(host_ieee80211_rest_test)]
fn bss_supported_rates(bss: *mut HostWlanBssidEx) -> *mut u8 {
    unsafe { (*bss).supported_rates.as_mut_ptr() }
}

#[cfg(not(host_ieee80211_rest_test))]
fn bss_dsconfig(bss: *mut c_void) -> *mut u32 {
    unsafe { rtw_ieee80211_rest_bss_dsconfig(bss) }
}

#[cfg(not(host_ieee80211_rest_test))]
fn bss_ielength(bss: *mut c_void) -> *mut u32 {
    unsafe { rtw_ieee80211_rest_bss_ielength(bss) }
}

#[cfg(not(host_ieee80211_rest_test))]
fn bss_ies(bss: *mut c_void) -> *mut u8 {
    unsafe { rtw_ieee80211_rest_bss_ies(bss) }
}

#[cfg(not(host_ieee80211_rest_test))]
fn bss_supported_rates(bss: *mut c_void) -> *mut u8 {
    unsafe { rtw_ieee80211_rest_bss_supported_rates(bss) }
}

#[no_mangle]
pub extern "C" fn rtw_get_bit_value_from_ieee_value(val: U8) -> c_int {
    static DOT11_RATE_TABLE: [U8; 13] = [2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 0];
    for (i, &rate) in DOT11_RATE_TABLE.iter().enumerate() {
        if rate == 0 {
            break;
        }
        if rate == val {
            return bit(i as u32);
        }
    }
    0
}

#[no_mangle]
pub extern "C" fn rtw_check_network_type(rate: *mut U8, ratelen: c_int, channel: c_int) -> c_int {
    let _ = ratelen;
    if channel > 14 {
        if is_cckrates_included(rate) {
            WIRELESS_INVALID as c_int
        } else {
            WIRELESS_11A as c_int
        }
    } else if is_cckratesonly_included(rate) {
        WIRELESS_11B as c_int
    } else if is_cckrates_included(rate) {
        WIRELESS_11BG as c_int
    } else {
        WIRELESS_11G as c_int
    }
}

#[no_mangle]
pub extern "C" fn rtw_set_supported_rate(supported_rates: *mut U8, mode: c_uint) {
    if supported_rates.is_null() {
        return;
    }
    unsafe {
        memset(
            supported_rates,
            0,
            NDIS_802_11_LENGTH_RATES_EX,
        );
        match mode {
            WIRELESS_11B => {
                memcpy(
                    supported_rates,
                    WIFI_CCKRATES.as_ptr(),
                    IEEE80211_CCK_RATE_LEN,
                );
            }
            WIRELESS_11G | WIRELESS_11A | WIRELESS_11_5N | WIRELESS_11A_5N | WIRELESS_11_5AC => {
                memcpy(
                    supported_rates,
                    WIFI_OFDMRATES.as_ptr(),
                    IEEE80211_NUM_OFDM_RATESLEN,
                );
            }
            WIRELESS_11BG | WIRELESS_11G_24N | WIRELESS_11_24N | WIRELESS_11BG_24N => {
                memcpy(
                    supported_rates,
                    WIFI_CCKRATES.as_ptr(),
                    IEEE80211_CCK_RATE_LEN,
                );
                memcpy(
                    supported_rates.add(IEEE80211_CCK_RATE_LEN),
                    WIFI_OFDMRATES.as_ptr(),
                    IEEE80211_NUM_OFDM_RATESLEN,
                );
            }
            _ => {}
        }
    }
}

#[cfg(host_ieee80211_rest_test)]
type BssPtr = *mut HostWlanBssidEx;

#[cfg(not(host_ieee80211_rest_test))]
type BssPtr = *mut c_void;

fn filter_suppport_rateie_inner(pbss_network: BssPtr, keep: U8) {
    if pbss_network.is_null() {
        return;
    }
    unsafe {
        let ie_length = bss_ielength(pbss_network);
        let ies = bss_ies(pbss_network);
        let mut ie_orilen: Sint = 0;
        let p = rtw_get_ie(
            ies.add(_BEACON_IE_OFFSET_),
            _SUPPORTEDRATES_IE_ as Sint,
            &mut ie_orilen,
            (*ie_length as Sint) - _BEACON_IE_OFFSET_ as Sint,
        );
        if p.is_null() {
            return;
        }

        let mut new_rate = [0u8; NDIS_802_11_LENGTH_RATES_EX];
        let mut idx: u8 = 0;
        for i in 0..ie_orilen as usize {
            let rate = *p.add(2 + i);
            let iscck = rtw_is_cck_rate(rate);
            let isofdm = rtw_is_ofdm_rate(rate);
            if (keep == CCK && iscck) || (keep == OFDM && isofdm) {
                new_rate[idx as usize] = if rtw_is_basic_rate_ofdm(rate) {
                    rate | IEEE80211_BASIC_RATE_MASK
                } else {
                    rate
                };
                idx += 1;
            }
        }
        *p.add(1) = idx;
        memcpy(p.add(2), new_rate.as_ptr(), idx as usize);
        let remain_ies = p.add(2 + ie_orilen as usize);
        let remain_len = (*ie_length as usize) - (remain_ies as usize - ies as usize);
        memmove(p.add(2 + idx as usize), remain_ies, remain_len);
        *ie_length -= (ie_orilen as u32).saturating_sub(idx as u32);
    }
}

#[no_mangle]
pub extern "C" fn rtw_filter_suppport_rateie(pbss_network: BssPtr, keep: U8) {
    filter_suppport_rateie_inner(pbss_network, keep);
}

#[no_mangle]
pub extern "C" fn rtw_update_rate_bymode(pbss_network: BssPtr, mode: u32) -> U8 {
    if pbss_network.is_null() {
        return 0;
    }
    let network_type = unsafe {
        let ie_length = bss_ielength(pbss_network);
        let ies = bss_ies(pbss_network);
        let mut network_ielen = *ie_length;
        let network_type = if mode == WIRELESS_11B {
            filter_suppport_rateie_inner(pbss_network, CCK);
            let mut ie_len: Sint = 0;
            let p = rtw_get_ie(
                ies.add(_BEACON_IE_OFFSET_),
                _EXT_SUPPORTEDRATES_IE_ as Sint,
                &mut ie_len,
                (*ie_length as Sint) - _BEACON_IE_OFFSET_ as Sint,
            );
            if !p.is_null() {
                rtw_ies_remove_ie(
                    ies,
                    &mut network_ielen,
                    _BEACON_IE_OFFSET_ as c_uint,
                    _EXT_SUPPORTEDRATES_IE_,
                    core::ptr::null_mut(),
                    0,
                );
                *ie_length -= ie_len as u32;
            }
            WIRELESS_11B
        } else if *bss_dsconfig(pbss_network) > 14 {
            filter_suppport_rateie_inner(pbss_network, OFDM);
            WIRELESS_11A
        } else if (mode & WIRELESS_11B) == 0 {
            filter_suppport_rateie_inner(pbss_network, OFDM);
            WIRELESS_11G
        } else {
            WIRELESS_11BG
        };
        rtw_set_supported_rate(bss_supported_rates(pbss_network), network_type);
        network_type as U8
    };
    network_type
}
