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
const RTW_SCAN_SPARSE_CH_NUM_MIRACAST: U8 = 1;
const RTW_SCAN_SPARSE_CH_NUM_BG: U8 = 4;
const RTW_SCAN_SPARSE_BG_INTERVAL_MS: U32 = 12000;
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

/// Host L2 `mi_state` is a 6-byte packed stub. Kernel `struct mi_state`
/// inserts `lg_sta_num` (and optional TDLS/AP/mesh/cfg80211/P2P fields)
/// and `rtw_mi_status` memsets the full sizeof — overlaying the stub on
/// the kernel path is a stack smash plus wrong AP/mesh counts.
#[cfg(host_mlme_ext_scan_test)]
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
    fn rtw_rust_scan_acs_adv_ms(a: Adapter) -> U16;
}

#[cfg(host_mlme_ext_scan_test)]
extern "C" {
    fn rtw_mi_status(a: Adapter, m: *mut MiState);
}

/* Kernel path: C owns `struct mi_state` stride via `sizeof` / `MSTATE_*`. */
#[cfg(rust_mlme_ext_scan)]
extern "C" {
    fn rtw_rust_scan_mi_counts(
        a: Adapter,
        sta_num: *mut U8,
        ld_sta_num: *mut U8,
        ap_num: *mut U8,
        ld_ap_num: *mut U8,
        mesh_num: *mut U8,
        ld_mesh_num: *mut U8,
    );
}

#[cfg(config_rtw_mesh)]
extern "C" {
    fn rtw_rust_scan_backop_flags_mesh(a: Adapter) -> U8;
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
    #[cfg(config_scan_sparse_miracast)]
    if unsafe { rtw_mi_check_miracast_enabled(a) && rtw_mi_busy_traffic_check(a) } {
        cap = RTW_SCAN_SPARSE_CH_NUM_MIRACAST;
    }
    #[cfg(config_scan_sparse_bg)]
    if pass_ms(last) > RTW_SCAN_SPARSE_BG_INTERVAL_MS {
        cap = if cap == SCAN_SPARSE_CH_NUM_INVALID {
            RTW_SCAN_SPARSE_CH_NUM_BG
        } else {
            core::cmp::min(cap, RTW_SCAN_SPARSE_CH_NUM_BG)
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

fn mi_counts(a: Adapter) -> (U8, U8, U8, U8, U8, U8) {
    #[cfg(host_mlme_ext_scan_test)]
    {
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
        (
            m.sta_num,
            m.ld_sta_num,
            m.ap_num,
            m.ld_ap_num,
            m.mesh_num,
            m.ld_mesh_num,
        )
    }
    #[cfg(rust_mlme_ext_scan)]
    {
        let mut sta_num = 0u8;
        let mut ld_sta_num = 0u8;
        let mut ap_num = 0u8;
        let mut ld_ap_num = 0u8;
        let mut mesh_num = 0u8;
        let mut ld_mesh_num = 0u8;
        unsafe {
            rtw_rust_scan_mi_counts(
                a,
                &mut sta_num,
                &mut ld_sta_num,
                &mut ap_num,
                &mut ld_ap_num,
                &mut mesh_num,
                &mut ld_mesh_num,
            );
        }
        (
            sta_num,
            ld_sta_num,
            ap_num,
            ld_ap_num,
            mesh_num,
            ld_mesh_num,
        )
    }
}

#[no_mangle]
pub extern "C" fn rtw_scan_backop_decision(a: Adapter) -> U8 {
    if a.is_null() {
        return 0;
    }
    let (sta_num, ld_sta_num, ap_num, ld_ap_num, mesh_num, ld_mesh_num) = mi_counts(a);
    let mut out = 0u8;
    let fs = unsafe { rtw_rust_scan_backop_flags_sta(a) };
    if (ld_sta_num != 0 && fs & SS_BACKOP_EN != 0) || (sta_num != 0 && fs & SS_BACKOP_EN_NL != 0) {
        out |= fs;
    }
    let fa = unsafe { rtw_rust_scan_backop_flags_ap(a) };
    if (ld_ap_num != 0 && fa & SS_BACKOP_EN != 0) || (ap_num != 0 && fa & SS_BACKOP_EN_NL != 0) {
        out |= fa;
    }
    #[cfg(config_rtw_mesh)]
    {
        let fm = unsafe { rtw_rust_scan_backop_flags_mesh(a) };
        if (ld_mesh_num != 0 && fm & SS_BACKOP_EN != 0)
            || (mesh_num != 0 && fm & SS_BACKOP_EN_NL != 0)
        {
            out |= fm;
        }
    }
    #[cfg(not(config_rtw_mesh))]
    {
        let _ = (mesh_num, ld_mesh_num);
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
    let scan_ms = if dur != 0 {
        dur
    } else {
        #[cfg(all(config_rtw_acs, config_rtw_acs_dbg))]
        {
            let acs_ms = unsafe { rtw_rust_scan_acs_adv_ms(a) };
            if acs_ms != 0 {
                acs_ms
            } else {
                ch_ms
            }
        }
        #[cfg(not(all(config_rtw_acs, config_rtw_acs_dbg)))]
        {
            ch_ms
        }
    };
    let t = scan_ms as U32 * max_ch as U32 + back + SCANNING_TIMEOUT_EX;
    unsafe {
        rtw_rust_scan_set_timeout_ms(a, t);
    }
    t
}

#[cfg(rust_mlme_ext_scan_ch)]
mod scan_ch {
    use super::*;

    const RTW_IEEE80211_CHAN_DISABLED: U32 = 1 << 0;
    const RTW_IEEE80211_CHAN_PASSIVE_SCAN: U32 = 1 << 1;
    const RTW_CHF_NO_IR: U8 = 1 << 0;
    const RTW_CHF_DFS: U8 = 1 << 1;
    const RTW_AUTO_SCAN_REASON_ROAM: i32 = 1;

    extern "C" {
        fn rtw_mlme_band_check(a: Adapter, ch: U32) -> bool;
        fn rtw_mlme_ignore_chan(a: Adapter, ch: U32) -> bool;
        fn rtw_chset_search_ch(ch_set: *mut c_void, ch: U32) -> i32;
        fn rtw_rust_scan_regsty_wifi_spec(a: Adapter) -> U8;
        fn rtw_rust_scan_max_chan_nums(a: Adapter) -> U8;
        fn rtw_rust_scan_chset_channel_num(a: Adapter, idx: i32) -> U8;
        fn rtw_rust_scan_chset_flags(a: Adapter, idx: i32) -> U8;
        fn rtw_rust_scan_chset_clear_hidden_bss(a: Adapter, idx: i32);
        fn rtw_rust_scan_chset(a: Adapter) -> *mut c_void;
    }

    #[cfg(config_rtw_roam_quickscan)]
    extern "C" {
        fn rtw_rust_scan_roam_quickscan_next(a: Adapter) -> U8;
        fn rtw_rust_scan_roam_clear_quickscan_next(a: Adapter);
        fn rtw_rust_scan_roam_ch_num(a: Adapter) -> U8;
        fn rtw_rust_scan_roam_copy_ch(a: Adapter, out: *mut RtwIeee80211Channel, out_num: U32);
    }

    #[no_mangle]
    pub extern "C" fn rtw_scan_ch_decision(
        padapter: Adapter,
        out: *mut RtwIeee80211Channel,
        out_num: U32,
        input: *mut RtwIeee80211Channel,
        in_num: U32,
        no_sparse: bool,
        _reason: i32,
    ) -> i32 {
        let reason = _reason;
        if padapter.is_null() || out.is_null() {
            return 0;
        }
        #[cfg(not(config_rtw_roam_quickscan))]
        let _ = reason;
        let out_slice =
            unsafe { core::slice::from_raw_parts_mut(out, out_num as usize) };
        for ch in out_slice.iter_mut() {
            *ch = RtwIeee80211Channel {
                hw_value: 0,
                flags: 0,
            };
        }

        #[cfg(config_rtw_roam_quickscan)]
        {
            if reason == RTW_AUTO_SCAN_REASON_ROAM
                && unsafe { rtw_rust_scan_roam_quickscan_next(padapter) } != 0
            {
                unsafe {
                    rtw_rust_scan_roam_clear_quickscan_next(padapter);
                    rtw_rust_scan_roam_copy_ch(padapter, out, out_num);
                }
                return unsafe { rtw_rust_scan_roam_ch_num(padapter) as i32 };
            }
        }

        let mut j: i32 = 0;
        if !input.is_null() && in_num > 0 {
            let in_slice = unsafe { core::slice::from_raw_parts(input, in_num as usize) };
            for i in 0..in_num as usize {
                let hw = in_slice[i].hw_value as U32;
                if hw == 0 || (in_slice[i].flags & RTW_IEEE80211_CHAN_DISABLED) != 0 {
                    continue;
                }
                if !unsafe { rtw_mlme_band_check(padapter, hw) } {
                    continue;
                }
                let chset = unsafe { rtw_rust_scan_chset(padapter) };
                let set_idx = unsafe { rtw_chset_search_ch(chset, hw) };
                if set_idx >= 0 {
                    if j as U32 >= out_num {
                        break;
                    }
                    out_slice[j as usize] = in_slice[i];
                    let flags = unsafe { rtw_rust_scan_chset_flags(padapter, set_idx) };
                    if (flags & (RTW_CHF_NO_IR | RTW_CHF_DFS)) != 0 {
                        out_slice[j as usize].flags |= RTW_IEEE80211_CHAN_PASSIVE_SCAN;
                    }
                    j += 1;
                }
                if j as U32 >= out_num {
                    break;
                }
            }
        }

        if j == 0 {
            let max = unsafe { rtw_rust_scan_max_chan_nums(padapter) } as i32;
            for i in 0..max {
                let chan = unsafe { rtw_rust_scan_chset_channel_num(padapter, i) } as U32;
                if chan == 0 {
                    continue;
                }
                if !unsafe { rtw_mlme_band_check(padapter, chan) } {
                    continue;
                }
                if unsafe { rtw_mlme_ignore_chan(padapter, chan) } {
                    continue;
                }
                if j as U32 >= out_num {
                    break;
                }
                out_slice[j as usize].hw_value = chan as U16;
                let flags = unsafe { rtw_rust_scan_chset_flags(padapter, i) };
                if (flags & (RTW_CHF_NO_IR | RTW_CHF_DFS)) != 0 {
                    out_slice[j as usize].flags |= RTW_IEEE80211_CHAN_PASSIVE_SCAN;
                }
                j += 1;
            }
        }

        if !no_sparse
            && unsafe { rtw_rust_scan_regsty_wifi_spec(padapter) } == 0
            && j > 6
        {
            j = super::rtw_scan_sparse(padapter, out, j as U8) as i32;
        }
        j
    }
}
