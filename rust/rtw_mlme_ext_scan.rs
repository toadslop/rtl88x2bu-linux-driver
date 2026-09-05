// SPDX-License-Identifier: GPL-2.0
//! W3-70 PR4: scan sparse/backop/timeout — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    unreachable_pub,
    missing_docs
)]

#[cfg(host_mlme_ext_scan_test)]
use std::os::raw::{c_ulong, c_void};

#[cfg(rust_mlme_ext_scan)]
use core::ffi::{c_ulong, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;
type Systime = c_ulong;
type Adapter = *mut c_void;

const SS_BACKOP_EN: U8 = 1;
const SS_BACKOP_EN_NL: U8 = 2;
const SUPPORTED_24G: U32 = (1 << 0) | (1 << 1) | (1 << 3);
const SUPPORTED_5G: U32 = (1 << 2) | (1 << 4) | (1 << 6);
const SCAN_SPARSE_CH_NUM_INVALID: U8 = 255;
const SCANNING_TIMEOUT_EX: U32 = 2000;
const MAX_CHANNEL_NUM: U8 = 59;
const MAX_CHANNEL_NUM_2G: U8 = 14;

static mut SCAN_SPARSE_TOKEN: U8 = 255;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct RtwIeee80211Channel {
    pub hw_value: U16,
    pub flags: U32,
}

#[repr(C)]
struct MiState {
    sta_num: U8,
    ld_sta_num: U8,
    ap_num: U8,
    ld_ap_num: U8,
    mesh_num: U8,
    ld_mesh_num: U8,
}

extern "C" {
    fn rtw_mi_busy_traffic_check(a: Adapter) -> bool;
    fn rtw_mi_check_miracast_enabled(a: Adapter) -> bool;
    fn rtw_mi_status(a: Adapter, m: *mut MiState);
    fn rtw_rust_scan_last_scan_time(a: Adapter) -> Systime;
    fn rtw_rust_scan_set_last_scan_time(a: Adapter, t: Systime);
    fn rtw_rust_scan_wireless_mode(a: Adapter) -> U32;
    fn rtw_rust_scan_ch_ms(a: Adapter) -> U16;
    fn rtw_rust_scan_duration(a: Adapter) -> U16;
    fn rtw_rust_scan_cnt_max(a: Adapter) -> U8;
    fn rtw_rust_scan_backop_ms(a: Adapter) -> U16;
    fn rtw_rust_scan_set_timeout_ms(a: Adapter, ms: U32);
    fn rtw_rust_scan_backop_flags_sta(a: Adapter) -> U8;
    fn rtw_rust_scan_backop_flags_ap(a: Adapter) -> U8;
}

#[cfg(host_mlme_ext_scan_test)]
extern "C" {
    fn rtw_get_current_time() -> Systime;
    fn rtw_get_passing_time_ms(s: Systime) -> U32;
}

#[cfg(rust_mlme_ext_scan)]
extern "C" {
    fn _rtw_get_current_time() -> Systime;
    fn _rtw_get_passing_time_ms(s: Systime) -> U32;
}

#[inline]
fn now() -> Systime {
    unsafe {
        #[cfg(host_mlme_ext_scan_test)]
        {
            rtw_get_current_time()
        }
        #[cfg(rust_mlme_ext_scan)]
        {
            _rtw_get_current_time()
        }
    }
}

#[inline]
fn pass_ms(s: Systime) -> U32 {
    unsafe {
        #[cfg(host_mlme_ext_scan_test)]
        {
            rtw_get_passing_time_ms(s)
        }
        #[cfg(rust_mlme_ext_scan)]
        {
            _rtw_get_passing_time_ms(s)
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_scan_sparse(a: Adapter, ch: *mut RtwIeee80211Channel, n: U8) -> U8 {
    if a.is_null() || ch.is_null() {
        return n;
    }
    let mut last = unsafe { rtw_rust_scan_last_scan_time(a) };
    if last == 0 {
        last = now();
        unsafe {
            rtw_rust_scan_set_last_scan_time(a, last);
        }
    }
    let mut cap = SCAN_SPARSE_CH_NUM_INVALID;
    if unsafe { rtw_mi_check_miracast_enabled(a) && rtw_mi_busy_traffic_check(a) } {
        cap = 1;
    }
    if pass_ms(last) > 12000 {
        cap = if cap == SCAN_SPARSE_CH_NUM_INVALID {
            4
        } else {
            core::cmp::min(cap, 4)
        };
    }
    if cap == SCAN_SPARSE_CH_NUM_INVALID {
        return n;
    }
    let div = n / cap + u8::from(n % cap != 0);
    let tok = unsafe {
        let t = (SCAN_SPARSE_TOKEN as u16 + 1) % div as u16;
        SCAN_SPARSE_TOKEN = t as U8;
        SCAN_SPARSE_TOKEN
    };
    let chs = unsafe { core::slice::from_raw_parts_mut(ch, n as usize + 1) };
    let mut k = 0usize;
    for i in 0..n as usize {
        if chs[i].hw_value != 0 && (i as U8 % div) == tok {
            if i != k {
                chs[k].hw_value = chs[i].hw_value;
                chs[k].flags = chs[i].flags;
            }
            k += 1;
        }
    }
    chs[k] = RtwIeee80211Channel {
        hw_value: 0,
        flags: 0,
    };
    unsafe {
        rtw_rust_scan_set_last_scan_time(a, now());
    }
    k as U8
}

#[no_mangle]
pub extern "C" fn rtw_scan_backop_decision(a: Adapter) -> U8 {
    if a.is_null() {
        return 0;
    }
    let mut m = MiState {
        sta_num: 0,
        ld_sta_num: 0,
        ap_num: 0,
        ld_ap_num: 0,
        mesh_num: 0,
        ld_mesh_num: 0,
    };
    unsafe {
        rtw_mi_status(a, &mut m);
    }
    let mut out = 0u8;
    let fs = unsafe { rtw_rust_scan_backop_flags_sta(a) };
    if (m.ld_sta_num != 0 && fs & SS_BACKOP_EN != 0) || (m.sta_num != 0 && fs & SS_BACKOP_EN_NL != 0) {
        out |= fs;
    }
    let fa = unsafe { rtw_rust_scan_backop_flags_ap(a) };
    if (m.ld_ap_num != 0 && fa & SS_BACKOP_EN != 0) || (m.ap_num != 0 && fa & SS_BACKOP_EN_NL != 0) {
        out |= fa;
    }
    out
}

#[no_mangle]
pub extern "C" fn rtw_scan_timeout_decision(a: Adapter) -> U32 {
    if a.is_null() {
        return 0;
    }
    let mode = unsafe { rtw_rust_scan_wireless_mode(a) };
    let ch_ms = unsafe { rtw_rust_scan_ch_ms(a) };
    let dur = unsafe { rtw_rust_scan_duration(a) };
    let cnt_max = unsafe { rtw_rust_scan_cnt_max(a) };
    let bop_ms = unsafe { rtw_rust_scan_backop_ms(a) };
    let max_ch = if (mode & SUPPORTED_5G) != 0 && (mode & SUPPORTED_24G) != 0 {
        MAX_CHANNEL_NUM
    } else {
        MAX_CHANNEL_NUM_2G
    };
    let back = if rtw_scan_backop_decision(a) != 0 {
        max_ch as U32 / cnt_max as U32 * bop_ms as U32
    } else {
        0
    };
    let scan_ms = if dur != 0 { dur } else { ch_ms };
    let t = scan_ms as U32 * max_ch as U32 + back + SCANNING_TIMEOUT_EX;
    unsafe {
        rtw_rust_scan_set_timeout_ms(a, t);
    }
    t
}
