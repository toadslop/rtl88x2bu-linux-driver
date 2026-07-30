// SPDX-License-Identifier: GPL-2.0
//! IEEE 802.11 rest helpers — Rust port of `core/rtw_ieee80211_rest.c` rate
//! classification slice (W3-26), WPA/RSN cipher suite getters (W3-27), and
//! WPA/RSN IE parse (W3-28), and WAPI/WPS/sec-IE getters (W3-29), and
//! string/MAC address helpers (W3-30), and chbw grouping/sync (W3-31), and
//! frame header / HT MCS helpers (W3-32).

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

const ETH_ALEN: usize = 6;
const BIT0: U8 = 1;
const BIT1: U8 = 2;

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

const _WPA_IE_ID_: U8 = 0xdd;
const _WPA2_IE_ID_: U8 = 0x30;
const _WAPI_IE_: U8 = 68;
const _TIMESTAMP_: usize = 8;
const _BEACON_ITERVAL_: usize = 2;
const _CAPABILITY_: usize = 2;

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

const _DSSET_IE_: U8 = 3;
const EID_HTCAPABILITY: Sint = 45;
const EID_HTINFO: Sint = 61;
const EID_VHTOPERATION: Sint = 192;

const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const CHANNEL_WIDTH_80: U8 = 2;

const HAL_PRIME_CHNL_OFFSET_DONT_CARE: U8 = 0;
const HAL_PRIME_CHNL_OFFSET_LOWER: U8 = 1;
const HAL_PRIME_CHNL_OFFSET_UPPER: U8 = 2;

const SCA: U8 = 1;
const SCB: U8 = 3;

const NDIS_802_11_FIXED_IES_LEN: usize = 12;

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

    static mut rtw_initmac: *mut U8;
    fn rtw_random32() -> u32;
    #[cfg(all(not(host_ieee80211_rest_test), CONFIG_PLATFORM_INTEL_BYT))]
    fn rtw_get_mac_addr_intel(buf: *mut U8) -> c_int;

    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_dsconfig(bss: *mut c_void) -> *mut u32;
    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_ielength(bss: *mut c_void) -> *mut u32;
    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_ies(bss: *mut c_void) -> *mut u8;
    #[cfg(not(host_ieee80211_rest_test))]
    fn rtw_ieee80211_rest_bss_supported_rates(bss: *mut c_void) -> *mut u8;

    fn rtw_get_offset_by_chbw(ch: U8, bw: U8, r_offset: *mut U8) -> U8;
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

const _FAIL: i32 = 0;

const WLAN_EID_RSN: U8 = 48;

const RTW_WPA_OUI_TYPE: [U8; 4] = [0x00, 0x50, 0xf2, 1];
const SUITE_1X: [U8; 4] = [0x00, 0x50, 0xf2, 1];

const MFP_NO: U8 = 0;

fn rtw_get_le16(a: *const U8) -> u16 {
    unsafe { ((*(a.add(1)) as u16) << 8) | (*(a) as u16) }
}

fn le_bits_to_2byte(p: *const U8, bit_offset: u32, bit_len: u32) -> u16 {
    let val = rtw_get_le16(p);
    (val >> bit_offset) & ((1u16 << bit_len) - 1)
}

fn get_rsn_cap_mfp_option(cap: *const U8) -> U8 {
    le_bits_to_2byte(cap, 6, 2) as U8
}

fn get_rsn_cap_spp_opt(cap: *const U8) -> U8 {
    le_bits_to_2byte(cap, 10, 2) as U8
}

#[repr(C)]
pub struct RsneInfo {
    pub gcs: *mut U8,
    pub pcs_cnt: u16,
    pub pcs_list: *mut U8,
    pub akm_cnt: u16,
    pub akm_list: *mut U8,
    pub cap: *mut U8,
    pub pmkid_cnt: u16,
    pub pmkid_list: *mut U8,
    pub gmcs: *mut U8,
    pub err: U8,
}

#[no_mangle]
pub extern "C" fn rtw_parse_wpa_ie(
    wpa_ie: *mut U8,
    wpa_ie_len: c_int,
    group_cipher: *mut c_int,
    pairwise_cipher: *mut c_int,
    akm: *mut u32,
) -> c_int {
    if wpa_ie_len <= 0 {
        return _FAIL;
    }
    unsafe {
        if *wpa_ie != _WPA_IE_ID_
            || *(wpa_ie.add(1)) != (wpa_ie_len - 2) as U8
            || memcmp(
                wpa_ie.add(2),
                RTW_WPA_OUI_TYPE.as_ptr(),
                WPA_SELECTOR_LEN,
            ) != 0
        {
            return _FAIL;
        }

        let mut pos = wpa_ie.add(8);
        let mut left = wpa_ie_len - 8;

        if left >= WPA_SELECTOR_LEN as c_int {
            *group_cipher = rtw_get_wpa_cipher_suite(pos);
            pos = pos.add(WPA_SELECTOR_LEN);
            left -= WPA_SELECTOR_LEN as c_int;
        } else if left > 0 {
            return _FAIL;
        }

        if left >= 2 {
            let count = rtw_get_le16(pos) as c_int;
            pos = pos.add(2);
            left -= 2;

            if count == 0 || left < count * WPA_SELECTOR_LEN as c_int {
                return _FAIL;
            }

            for _ in 0..count {
                *pairwise_cipher |= rtw_get_wpa_cipher_suite(pos);
                pos = pos.add(WPA_SELECTOR_LEN);
                left -= WPA_SELECTOR_LEN as c_int;
            }
        } else if left == 1 {
            return _FAIL;
        }

        if !akm.is_null() && left >= 6 {
            pos = pos.add(2);
            if memcmp(pos, SUITE_1X.as_ptr(), 4) == 0 {
                *akm = WLAN_AKM_TYPE_8021X;
            }
        }
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_rsne_info_parse(ie: *const U8, ie_len: c_uint, info: *mut RsneInfo) -> c_int {
    unsafe {
        memset(info as *mut U8, 0, core::mem::size_of::<RsneInfo>());

        let mut pos = ie;
        if ie.add(ie_len as usize) < pos.add(4) {
            (*info).err = 1;
            return _FAIL;
        }

        if *ie != WLAN_EID_RSN || *(ie.add(1)) != (ie_len - 2) as U8 {
            (*info).err = 1;
            return _FAIL;
        }
        pos = pos.add(2);

        let ver = rtw_get_le16(pos);
        if ver != 1 {
            (*info).err = 1;
            return _FAIL;
        }
        pos = pos.add(2);

        if ie.add(ie_len as usize) < pos.add(4) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        (*info).gcs = pos as *mut U8;
        pos = pos.add(4);

        if ie.add(ie_len as usize) < pos.add(2) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        let mut cnt = rtw_get_le16(pos);
        pos = pos.add(2);
        if ie.add(ie_len as usize) < pos.add(4 * cnt as usize) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        (*info).pcs_cnt = cnt;
        (*info).pcs_list = pos as *mut U8;
        pos = pos.add(4 * cnt as usize);

        if ie.add(ie_len as usize) < pos.add(2) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        cnt = rtw_get_le16(pos);
        pos = pos.add(2);
        if ie.add(ie_len as usize) < pos.add(4 * cnt as usize) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        (*info).akm_cnt = cnt;
        (*info).akm_list = pos as *mut U8;
        pos = pos.add(4 * cnt as usize);

        if ie.add(ie_len as usize) < pos.add(2) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        (*info).cap = pos as *mut U8;
        pos = pos.add(2);

        if ie.add(ie_len as usize) < pos.add(2) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        cnt = rtw_get_le16(pos);
        pos = pos.add(2);
        if ie.add(ie_len as usize) < pos.add(16 * cnt as usize) {
            (*info).err = 1;
            return _FAIL;
        }
        (*info).pmkid_cnt = cnt;
        (*info).pmkid_list = pos as *mut U8;
        pos = pos.add(16 * cnt as usize);

        if ie.add(ie_len as usize) < pos.add(4) {
            if ie.add(ie_len as usize) != pos {
                (*info).err = 1;
                return _FAIL;
            }
            return _SUCCESS;
        }
        (*info).gmcs = pos as *mut U8;
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_parse_wpa2_ie(
    rsn_ie: *mut U8,
    rsn_ie_len: c_int,
    group_cipher: *mut c_int,
    pairwise_cipher: *mut c_int,
    gmcs: *mut c_int,
    akm: *mut u32,
    mfp_opt: *mut U8,
    spp_opt: *mut U8,
) -> c_int {
    let mut info = RsneInfo {
        gcs: core::ptr::null_mut(),
        pcs_cnt: 0,
        pcs_list: core::ptr::null_mut(),
        akm_cnt: 0,
        akm_list: core::ptr::null_mut(),
        cap: core::ptr::null_mut(),
        pmkid_cnt: 0,
        pmkid_list: core::ptr::null_mut(),
        gmcs: core::ptr::null_mut(),
        err: 0,
    };

    if rtw_rsne_info_parse(rsn_ie, rsn_ie_len as c_uint, &mut info) != _SUCCESS {
        return _FAIL;
    }

    unsafe {
        if !group_cipher.is_null() {
            if !info.gcs.is_null() {
                *group_cipher = rtw_get_rsn_cipher_suite(info.gcs);
            } else {
                *group_cipher = 0;
            }
        }

        if !pairwise_cipher.is_null() {
            *pairwise_cipher = 0;
            if !info.pcs_list.is_null() {
                for i in 0..info.pcs_cnt {
                    *pairwise_cipher |=
                        rtw_get_rsn_cipher_suite(info.pcs_list.add(4 * i as usize));
                }
            }
        }

        if !gmcs.is_null() {
            if !info.gmcs.is_null() {
                *gmcs = rtw_get_rsn_cipher_suite(info.gmcs);
            } else {
                *gmcs = WPA_CIPHER_BIP_CMAC_128;
            }
        }

        if !akm.is_null() {
            *akm = 0;
            if !info.akm_list.is_null() {
                for i in 0..info.akm_cnt {
                    *akm |= rtw_get_akm_suite_bitmap(info.akm_list.add(4 * i as usize));
                }
            }
        }

        if !mfp_opt.is_null() {
            *mfp_opt = MFP_NO;
            if !info.cap.is_null() {
                *mfp_opt = get_rsn_cap_mfp_option(info.cap);
            }
        }

        if !spp_opt.is_null() {
            *spp_opt = 0;
            if !info.cap.is_null() {
                *spp_opt = get_rsn_cap_spp_opt(info.cap);
            }
        }
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_get_wapi_ie(
    in_ie: *mut U8,
    in_len: c_uint,
    wapi_ie: *mut U8,
    wapi_len: *mut u16,
) -> c_int {
    let mut len = 0;
    let wapi_oui1: [U8; 4] = [0x0, 0x14, 0x72, 0x01];
    let wapi_oui2: [U8; 4] = [0x0, 0x14, 0x72, 0x02];

    unsafe {
        if !wapi_len.is_null() {
            *wapi_len = 0;
        }
        if in_ie.is_null() || in_len == 0 {
            return len;
        }

        let mut cnt = _TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_;
        while cnt < in_len as usize {
            let authmode = *in_ie.add(cnt);
            if authmode == _WAPI_IE_
                && (suite_matches(in_ie.add(cnt + 6), &wapi_oui1, 4)
                    || suite_matches(in_ie.add(cnt + 6), &wapi_oui2, 4))
            {
                if !wapi_ie.is_null() {
                    let ie_len = *in_ie.add(cnt + 1) as usize + 2;
                    memcpy(wapi_ie, in_ie.add(cnt), ie_len);
                }
                if !wapi_len.is_null() {
                    *wapi_len = (*in_ie.add(cnt + 1) as u16) + 2;
                }
                cnt += *in_ie.add(cnt + 1) as usize + 2;
            } else {
                cnt += *in_ie.add(cnt + 1) as usize + 2;
            }
        }

        if !wapi_len.is_null() {
            len = *wapi_len as c_int;
        }
    }
    len
}

#[no_mangle]
pub extern "C" fn rtw_get_sec_ie(
    in_ie: *mut U8,
    in_len: c_uint,
    rsn_ie: *mut U8,
    rsn_len: *mut u16,
    wpa_ie: *mut U8,
    wpa_len: *mut u16,
) -> c_int {
    let wpa_oui: [U8; 4] = [0x0, 0x50, 0xf2, 0x01];

    unsafe {
        let mut cnt = _TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_;
        while cnt < in_len as usize {
            let authmode = *in_ie.add(cnt);
            if authmode == _WPA_IE_ID_ && suite_matches(in_ie.add(cnt + 2), &wpa_oui, 4) {
                if !wpa_ie.is_null() {
                    let ie_len = *in_ie.add(cnt + 1) as usize + 2;
                    memcpy(wpa_ie, in_ie.add(cnt), ie_len);
                }
                *wpa_len = (*in_ie.add(cnt + 1) as u16) + 2;
                cnt += *in_ie.add(cnt + 1) as usize + 2;
            } else if authmode == _WPA2_IE_ID_ {
                if !rsn_ie.is_null() {
                    let ie_len = *in_ie.add(cnt + 1) as usize + 2;
                    memcpy(rsn_ie, in_ie.add(cnt), ie_len);
                }
                *rsn_len = (*in_ie.add(cnt + 1) as u16) + 2;
                cnt += *in_ie.add(cnt + 1) as usize + 2;
            } else {
                cnt += *in_ie.add(cnt + 1) as usize + 2;
            }
        }
        (*rsn_len as c_int) + (*wpa_len as c_int)
    }
}

#[no_mangle]
pub extern "C" fn rtw_is_wps_ie(ie_ptr: *mut U8, wps_ielen: *mut c_uint) -> U8 {
    let wps_oui: [U8; 4] = [0x0, 0x50, 0xf2, 0x04];

    if ie_ptr.is_null() {
        return _FALSE as U8;
    }
    unsafe {
        let eid = *ie_ptr;
        if eid == _WPA_IE_ID_ && suite_matches(ie_ptr.add(2), &wps_oui, 4) {
            *wps_ielen = (*ie_ptr.add(1) as c_uint) + 2;
            return _TRUE as U8;
        }
    }
    _FALSE as U8
}

fn key_char2num(ch: U8) -> U8 {
    match ch {
        b'0'..=b'9' => ch - b'0',
        b'a'..=b'f' => ch - b'a' + 10,
        b'A'..=b'F' => ch - b'A' + 10,
        _ => 0xff,
    }
}

#[no_mangle]
pub extern "C" fn str_2char2num(hch: U8, lch: U8) -> U8 {
    key_char2num(hch)
        .wrapping_mul(10)
        .wrapping_add(key_char2num(lch))
}

#[no_mangle]
pub extern "C" fn key_2char2num(hch: U8, lch: U8) -> U8 {
    (key_char2num(hch) << 4) | key_char2num(lch)
}

#[no_mangle]
pub extern "C" fn macstr2num(dst: *mut U8, src: *mut U8) {
    if dst.is_null() || src.is_null() {
        return;
    }
    unsafe {
        for jj in 0..ETH_ALEN {
            let kk = jj * 3;
            *dst.add(jj) = key_2char2num(*src.add(kk), *src.add(kk + 1));
        }
    }
}

#[no_mangle]
pub extern "C" fn convert_ip_addr(hch: U8, mch: U8, lch: U8) -> U8 {
    key_char2num(hch)
        .wrapping_mul(100)
        .wrapping_add(key_char2num(mch).wrapping_mul(10))
        .wrapping_add(key_char2num(lch))
}

#[no_mangle]
pub extern "C" fn rtw_check_invalid_mac_address(mac_addr: *mut U8, check_local_bit: U8) -> U8 {
    let null_mac_addr = [0u8; ETH_ALEN];
    let multi_mac_addr = [0xffu8; ETH_ALEN];

    if mac_addr.is_null() {
        return _TRUE as U8;
    }
    unsafe {
        if memcmp(mac_addr, null_mac_addr.as_ptr(), ETH_ALEN) == 0 {
            return _TRUE as U8;
        }
        if memcmp(mac_addr, multi_mac_addr.as_ptr(), ETH_ALEN) == 0 {
            return _TRUE as U8;
        }
        if *mac_addr & BIT0 != 0 {
            return _TRUE as U8;
        }
        if check_local_bit == _TRUE as U8 && *mac_addr & BIT1 != 0 {
            return _TRUE as U8;
        }
    }
    _FALSE as U8
}

#[no_mangle]
pub extern "C" fn rtw_macaddr_cfg(out: *mut U8, hw_mac_addr: *const U8) {
    // Zero-init: C leaves `mac` uninitialized when no source is provided; production
    // call sites always pass hw_mac_addr, but zeroing avoids UB on that edge path.
    let mut mac = [0u8; ETH_ALEN];

    if out.is_null() {
        return;
    }

    unsafe {
        if !rtw_initmac.is_null() {
            for jj in 0..ETH_ALEN {
                let kk = jj * 3;
                mac[jj] = key_2char2num(*rtw_initmac.add(kk), *rtw_initmac.add(kk + 1));
            }
        } else {
            #[cfg(all(not(host_ieee80211_rest_test), CONFIG_PLATFORM_INTEL_BYT))]
            let platform_ok = rtw_get_mac_addr_intel(mac.as_mut_ptr()) == 0;

            #[cfg(all(not(host_ieee80211_rest_test), CONFIG_PLATFORM_INTEL_BYT))]
            if !platform_ok && !hw_mac_addr.is_null() {
                memcpy(mac.as_mut_ptr(), hw_mac_addr, ETH_ALEN);
            }

            #[cfg(any(host_ieee80211_rest_test, not(CONFIG_PLATFORM_INTEL_BYT)))]
            if !hw_mac_addr.is_null() {
                memcpy(mac.as_mut_ptr(), hw_mac_addr, ETH_ALEN);
            }
        }

        if rtw_check_invalid_mac_address(mac.as_mut_ptr(), _TRUE as U8) == _TRUE as U8 {
            core::ptr::write(mac.as_mut_ptr().add(2) as *mut u32, rtw_random32());
            mac[0] = 0x00;
            mac[1] = 0xe0;
            mac[2] = 0x4c;
        }

        memcpy(out, mac.as_ptr(), ETH_ALEN);
    }
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

fn le_bits_to_1byte(start: *const U8, bit_offset: u32, bit_len: u32) -> U8 {
    unsafe { (*start >> bit_offset) as U8 & ((1u32 << bit_len) - 1) as U8 }
}

fn get_ht_cap_ele_chl_width(ele_start: *const U8) -> U8 {
    le_bits_to_1byte(ele_start, 1, 1)
}

fn get_ht_op_ele_pri_chl(ele_start: *const U8) -> U8 {
    le_bits_to_1byte(ele_start, 0, 8)
}

fn get_ht_op_ele_2nd_chl_offset(ele_start: *const U8) -> U8 {
    le_bits_to_1byte(unsafe { ele_start.add(1) }, 0, 2)
}

fn get_ht_op_ele_sta_chl_width(ele_start: *const U8) -> U8 {
    le_bits_to_1byte(unsafe { ele_start.add(1) }, 2, 1)
}

fn get_vht_operation_ele_chl_width(ele_start: *const U8) -> U8 {
    unsafe { *ele_start }
}

#[no_mangle]
pub extern "C" fn rtw_ies_get_chbw(
    ies: *mut U8,
    ies_len: c_int,
    ch: *mut U8,
    bw: *mut U8,
    offset: *mut U8,
    ht: U8,
    vht: U8,
) {
    if ies.is_null() || ch.is_null() || bw.is_null() || offset.is_null() {
        return;
    }

    unsafe {
        *ch = 0;
        *bw = CHANNEL_WIDTH_20;
        *offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

        let mut ie_len: Sint = 0;
        let p = rtw_get_ie(ies, _DSSET_IE_ as Sint, &mut ie_len, ies_len);
        if !p.is_null() && ie_len > 0 {
            *ch = *p.add(2);
        }

        if ht != 0 || vht != 0 {
            let mut ht_cap_ielen: Sint = 0;
            let ht_cap_ie =
                rtw_get_ie(ies, EID_HTCAPABILITY, &mut ht_cap_ielen, ies_len);
            if !ht_cap_ie.is_null() && ht_cap_ielen != 0 {
                if get_ht_cap_ele_chl_width(ht_cap_ie.add(2)) != 0 {
                    *bw = CHANNEL_WIDTH_40;
                }
            }

            let mut ht_op_ielen: Sint = 0;
            let ht_op_ie = rtw_get_ie(ies, EID_HTINFO, &mut ht_op_ielen, ies_len);
            if !ht_op_ie.is_null() && ht_op_ielen != 0 {
                let pri_ch = get_ht_op_ele_pri_chl(ht_op_ie.add(2));
                if *ch == 0 {
                    *ch = pri_ch;
                }

                if get_ht_op_ele_sta_chl_width(ht_op_ie.add(2)) == 0 {
                    *bw = CHANNEL_WIDTH_20;
                }

                if *bw == CHANNEL_WIDTH_40 {
                    match get_ht_op_ele_2nd_chl_offset(ht_op_ie.add(2)) {
                        SCA => *offset = HAL_PRIME_CHNL_OFFSET_LOWER,
                        SCB => *offset = HAL_PRIME_CHNL_OFFSET_UPPER,
                        _ => {}
                    }
                }
            }

            if vht != 0 {
                let mut vht_op_ielen: Sint = 0;
                let vht_op_ie =
                    rtw_get_ie(ies, EID_VHTOPERATION, &mut vht_op_ielen, ies_len);
                if !vht_op_ie.is_null() && vht_op_ielen != 0 {
                    if get_vht_operation_ele_chl_width(vht_op_ie.add(2)) >= 1 {
                        *bw = CHANNEL_WIDTH_80;
                    }
                }
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_bss_get_chbw(
    bss: BssPtr,
    ch: *mut U8,
    bw: *mut U8,
    offset: *mut U8,
    ht: U8,
    vht: U8,
) {
    if bss.is_null() || ch.is_null() || bw.is_null() || offset.is_null() {
        return;
    }

    unsafe {
        let ies = bss_ies(bss);
        let ie_length = *bss_ielength(bss);
        let tlv_len = ie_length.saturating_sub(NDIS_802_11_FIXED_IES_LEN as u32) as c_int;
        rtw_ies_get_chbw(
            ies.add(NDIS_802_11_FIXED_IES_LEN),
            tlv_len,
            ch,
            bw,
            offset,
            ht,
            vht,
        );

        let ds_config = *bss_dsconfig(bss) as U8;
        if *ch == 0 {
            *ch = ds_config;
        } else if *ch != ds_config {
            *ch = ds_config;
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_is_chbw_grouped(
    ch_a: U8,
    bw_a: U8,
    offset_a: U8,
    ch_b: U8,
    bw_b: U8,
    offset_b: U8,
) -> bool {
    if ch_a != ch_b {
        return false;
    }
    if (bw_a == CHANNEL_WIDTH_40 || bw_a == CHANNEL_WIDTH_80)
        && (bw_b == CHANNEL_WIDTH_40 || bw_b == CHANNEL_WIDTH_80)
        && offset_a != offset_b
    {
        return false;
    }
    true
}

#[no_mangle]
pub extern "C" fn rtw_sync_chbw(
    req_ch: *mut U8,
    req_bw: *mut U8,
    req_offset: *mut U8,
    g_ch: *mut U8,
    g_bw: *mut U8,
    g_offset: *mut U8,
) {
    if req_ch.is_null()
        || req_bw.is_null()
        || req_offset.is_null()
        || g_ch.is_null()
        || g_bw.is_null()
        || g_offset.is_null()
    {
        return;
    }

    unsafe {
        *req_ch = *g_ch;

        if *req_bw == CHANNEL_WIDTH_80 && *g_ch <= 14 {
            *req_bw = CHANNEL_WIDTH_40;
        }

        match *req_bw {
            CHANNEL_WIDTH_80 => {
                if *g_bw == CHANNEL_WIDTH_40 || *g_bw == CHANNEL_WIDTH_80 {
                    *req_offset = *g_offset;
                } else if *g_bw == CHANNEL_WIDTH_20 {
                    rtw_get_offset_by_chbw(*req_ch, *req_bw, req_offset);
                }
                if *req_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE {
                    *req_bw = CHANNEL_WIDTH_20;
                }
            }
            CHANNEL_WIDTH_40 => {
                if *g_bw == CHANNEL_WIDTH_40 || *g_bw == CHANNEL_WIDTH_80 {
                    *req_offset = *g_offset;
                } else if *g_bw == CHANNEL_WIDTH_20 {
                    rtw_get_offset_by_chbw(*req_ch, *req_bw, req_offset);
                }
                if *req_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE {
                    *req_bw = CHANNEL_WIDTH_20;
                }
            }
            CHANNEL_WIDTH_20 => {
                *req_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
            }
            _ => {}
        }

        if *req_bw > *g_bw {
            *g_bw = *req_bw;
            *g_offset = *req_offset;
        }
    }
}

// --- W3-32: frame header and HT MCS helpers ---

const RTW_IEEE80211_FCTL_FTYPE: u16 = 0x000c;
const RTW_IEEE80211_FCTL_STYPE: u16 = 0x00f0;
const RTW_IEEE80211_FCTL_FROMDS: u16 = 0x0200;
const RTW_IEEE80211_FCTL_TODS: u16 = 0x0100;
const RTW_IEEE80211_FTYPE_MGMT: u16 = 0x0000;
const RTW_IEEE80211_FTYPE_DATA: u16 = 0x0008;
const RTW_IEEE80211_FTYPE_CTL: u16 = 0x0004;
const RTW_IEEE80211_STYPE_QOS_DATA: u16 = 0x0080;
const RTW_IEEE80211_STYPE_ACTION: u16 = 0x00d0;
const RTW_IEEE80211_STYPE_CTS: u16 = 0x00c0;
const RTW_IEEE80211_STYPE_ACK: u16 = 0x00d0;
const RTW_WLAN_CATEGORY_P2P: U8 = 0x7f;
const ACT_PUBLIC_MAX: U8 = 32;

#[repr(C)]
struct RtwIeee80211Hdr3Addr {
    frame_ctl: u16,
    duration_id: u16,
    addr1: [U8; ETH_ALEN],
    addr2: [U8; ETH_ALEN],
    addr3: [U8; ETH_ALEN],
    seq_ctl: u16,
}

fn wlan_fc_get_type(fc: u16) -> u16 {
    fc & RTW_IEEE80211_FCTL_FTYPE
}

fn wlan_fc_get_stype(fc: u16) -> u16 {
    fc & RTW_IEEE80211_FCTL_STYPE
}

fn ht_cap_ele_sup_mcs_set(ht_cap: *const U8) -> *const U8 {
    unsafe { ht_cap.add(3) }
}

fn get_ht_cap_ele_tx_mcs_def(ht_cap: *const U8) -> U8 {
    le_bits_to_1byte(unsafe { ht_cap.add(15) }, 0, 1)
}

fn get_ht_cap_ele_trx_mcs_neq(ht_cap: *const U8) -> U8 {
    le_bits_to_1byte(unsafe { ht_cap.add(15) }, 1, 1)
}

fn get_ht_cap_ele_tx_max_ss(ht_cap: *const U8) -> U8 {
    le_bits_to_1byte(unsafe { ht_cap.add(15) }, 2, 2)
}

#[cfg(host_ieee80211_rest_test)]
fn rtw_ht_mcsset_to_nss_inner(supp_mcs_set: *const U8) -> U8 {
    if supp_mcs_set.is_null() {
        return 1;
    }
    unsafe {
        if *supp_mcs_set.add(3) != 0 {
            4
        } else if *supp_mcs_set.add(2) != 0 {
            3
        } else if *supp_mcs_set.add(1) != 0 {
            2
        } else if *supp_mcs_set.add(0) != 0 {
            1
        } else {
            1
        }
    }
}

#[cfg(not(host_ieee80211_rest_test))]
extern "C" {
    fn rtw_ht_mcsset_to_nss(supp_mcs_set: *mut U8) -> U8;
}

#[cfg(not(host_ieee80211_rest_test))]
fn rtw_ht_mcsset_to_nss_inner(supp_mcs_set: *const U8) -> U8 {
    unsafe { rtw_ht_mcsset_to_nss(supp_mcs_set as *mut U8) }
}

fn ht_mcs_rate_from_byte(byte: U8, idx: usize, bw_40: U8, short_gi: U8) -> u16 {
    if byte == 0 {
        return 0;
    }
    let bw40 = bw_40 != 0;
    let sgi = short_gi != 0;
    let bit = |n: u32| -> U8 { 1u8 << n };
    macro_rules! rate {
        ($b7:expr, $b6:expr, $b5:expr, $b4:expr, $b3:expr, $b2:expr, $b1:expr, $b0:expr) => {
            if byte & bit(7) != 0 {
                if bw40 { if sgi { $b7.0 } else { $b7.1 } } else if sgi { $b7.2 } else { $b7.3 }
            } else if byte & bit(6) != 0 {
                if bw40 { if sgi { $b6.0 } else { $b6.1 } } else if sgi { $b6.2 } else { $b6.3 }
            } else if byte & bit(5) != 0 {
                if bw40 { if sgi { $b5.0 } else { $b5.1 } } else if sgi { $b5.2 } else { $b5.3 }
            } else if byte & bit(4) != 0 {
                if bw40 { if sgi { $b4.0 } else { $b4.1 } } else if sgi { $b4.2 } else { $b4.3 }
            } else if byte & bit(3) != 0 {
                if bw40 { if sgi { $b3.0 } else { $b3.1 } } else if sgi { $b3.2 } else { $b3.3 }
            } else if byte & bit(2) != 0 {
                if bw40 { if sgi { $b2.0 } else { $b2.1 } } else if sgi { $b2.2 } else { $b2.3 }
            } else if byte & bit(1) != 0 {
                if bw40 { if sgi { $b1.0 } else { $b1.1 } } else if sgi { $b1.2 } else { $b1.3 }
            } else if byte & bit(0) != 0 {
                if bw40 { if sgi { $b0.0 } else { $b0.1 } } else if sgi { $b0.2 } else { $b0.3 }
            } else {
                0
            }
        };
    }
    match idx {
        3 => rate!(
            (6000, 5400, 2889, 2600),
            (5400, 4860, 2600, 2340),
            (4800, 4320, 2311, 2080),
            (3600, 3240, 1733, 1560),
            (2400, 2160, 1156, 1040),
            (1800, 1620, 867, 780),
            (1200, 1080, 578, 520),
            (600, 540, 289, 260)
        ),
        2 => rate!(
            (4500, 4050, 2167, 1950),
            (4050, 3645, 1950, 1750),
            (3600, 3240, 1733, 1560),
            (2700, 2430, 1300, 1170),
            (1800, 1620, 867, 780),
            (1350, 1215, 650, 585),
            (900, 810, 433, 390),
            (450, 405, 217, 195)
        ),
        1 => rate!(
            (3000, 2700, 1444, 1300),
            (2700, 2430, 1300, 1170),
            (2400, 2160, 1156, 1040),
            (1800, 1620, 867, 780),
            (1200, 1080, 578, 520),
            (900, 810, 433, 390),
            (600, 540, 289, 260),
            (300, 270, 144, 130)
        ),
        _ => rate!(
            (1500, 1350, 722, 650),
            (1350, 1215, 650, 585),
            (1200, 1080, 578, 520),
            (900, 810, 433, 390),
            (600, 540, 289, 260),
            (450, 405, 217, 195),
            (300, 270, 144, 130),
            (150, 135, 72, 65)
        ),
    }
}

#[no_mangle]
pub extern "C" fn ieee80211_is_empty_essid(essid: *const i8, essid_len: c_int) -> c_int {
    if essid.is_null() || essid_len <= 0 {
        return 1;
    }
    unsafe {
        if essid_len == 1 && *essid == b' ' as i8 {
            return 1;
        }
        let mut len = essid_len;
        while len > 0 {
            len -= 1;
            if *essid.add(len as usize) != 0 {
                return 0;
            }
        }
    }
    1
}

#[no_mangle]
pub extern "C" fn ieee80211_get_hdrlen(fc: u16) -> c_int {
    let mut hdrlen = 24;
    match wlan_fc_get_type(fc) {
        RTW_IEEE80211_FTYPE_DATA => {
            if fc & RTW_IEEE80211_STYPE_QOS_DATA != 0 {
                hdrlen += 2;
            }
            if fc & RTW_IEEE80211_FCTL_FROMDS != 0 && fc & RTW_IEEE80211_FCTL_TODS != 0 {
                hdrlen += 6;
            }
        }
        RTW_IEEE80211_FTYPE_CTL => {
            hdrlen = match wlan_fc_get_stype(fc) {
                RTW_IEEE80211_STYPE_CTS | RTW_IEEE80211_STYPE_ACK => 10,
                _ => 16,
            };
        }
        _ => {}
    }
    hdrlen
}

#[no_mangle]
pub extern "C" fn rtw_ht_mcs_rate(bw_40mhz: U8, short_gi: U8, mcs_rate: *mut U8) -> u16 {
    if mcs_rate.is_null() {
        return 0;
    }
    unsafe {
        for idx in (0..4).rev() {
            let byte = *mcs_rate.add(idx);
            if byte != 0 {
                return ht_mcs_rate_from_byte(byte, idx, bw_40mhz, short_gi);
            }
        }
        ht_mcs_rate_from_byte(*mcs_rate, 0, bw_40mhz, short_gi)
    }
}

#[no_mangle]
pub extern "C" fn rtw_ht_cap_get_rx_nss(ht_cap: *mut U8) -> U8 {
    if ht_cap.is_null() {
        return 1;
    }
    rtw_ht_mcsset_to_nss_inner(ht_cap_ele_sup_mcs_set(ht_cap))
}

#[no_mangle]
pub extern "C" fn rtw_ht_cap_get_tx_nss(ht_cap: *mut U8) -> U8 {
    if ht_cap.is_null() {
        return 1;
    }
    if get_ht_cap_ele_tx_mcs_def(ht_cap) != 0 && get_ht_cap_ele_trx_mcs_neq(ht_cap) != 0 {
        return get_ht_cap_ele_tx_max_ss(ht_cap) + 1;
    }
    rtw_ht_cap_get_rx_nss(ht_cap)
}

#[no_mangle]
pub extern "C" fn rtw_action_frame_parse(
    frame: *const U8,
    frame_len: u32,
    category: *mut U8,
    action: *mut U8,
) -> c_int {
    let _ = frame_len;
    if frame.is_null() {
        return _FALSE;
    }
    unsafe {
        let hdr = frame as *const RtwIeee80211Hdr3Addr;
        let fc = (*hdr).frame_ctl;
        if (fc & (RTW_IEEE80211_FCTL_FTYPE | RTW_IEEE80211_FCTL_STYPE))
            != (RTW_IEEE80211_FTYPE_MGMT | RTW_IEEE80211_STYPE_ACTION)
        {
            return _FALSE;
        }
        let frame_body = frame.add(core::mem::size_of::<RtwIeee80211Hdr3Addr>());
        let c = *frame_body;
        let mut a = ACT_PUBLIC_MAX;
        if c != RTW_WLAN_CATEGORY_P2P {
            a = *frame_body.add(1);
        }
        if !category.is_null() {
            *category = c;
        }
        if !action.is_null() {
            *action = a;
        }
    }
    _TRUE
}

#[no_mangle]
pub extern "C" fn rtw_rust_ieee80211_rest_probe() -> c_int {
    0x1e26
}
