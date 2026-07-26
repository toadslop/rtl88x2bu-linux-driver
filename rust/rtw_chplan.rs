// SPDX-License-Identifier: GPL-2.0
//! Channel-plan lookup / DFS / country helpers — Rust port of `core/rtw_chplan.c`
//! slices (W2-17..W2-19).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_chplan_test)]
use std::os::raw::{c_int, c_uint};

#[cfg(not(host_chplan_test))]
use core::ffi::{c_int, c_uint};

const TXPWR_LMT_NONE: u8 = 0;
const RTW_CHD_2G_NULL: u8 = 0;
const RTW_CHD_5G_NULL: u8 = 0;
const RTW_CHF_DFS: u8 = 1 << 1;
const MAX_CHANNEL_NUM: usize = 59;
const _TRUE: i32 = 1;
const _FALSE: i32 = 0;

#[repr(C)]
pub struct ChplanEnt {
    pub regd_2g: u8,
    pub chd_2g: u8,
    pub regd_5g: u8,
    pub chd_5g: u8,
}

#[repr(C)]
pub struct RtChannelInfo {
    pub channel_num: u8,
    pub flags: u8,
}

#[repr(C)]
pub struct CountryChplan {
    pub alpha2: [u8; 2],
    pub chplan: u8,
    pub en_11ac: u8,
}

#[cfg(host_chplan_test)]
#[repr(C)]
struct HostRegistryPriv {
    excl_chs: [u8; MAX_CHANNEL_NUM],
}

#[cfg(not(host_chplan_test))]
mod layout {
    /// `offsetof(struct registry_priv, excl_chs)` for this driver's
    /// `include/drv_types.h` layout (verified via kbuild probe in
    /// `core/rtw_chplan_offset_probe.c`). Re-run L1 after any `registry_priv`
    /// layout change.
    pub const EXCL_CHS_OFFSET: usize = 0x43c;
}

extern "C" {
    static RTW_ChannelPlanMap: ChplanEnt;
    static RTW_ChannelPlanMap_size: c_int;
    static country_chplan_map: CountryChplan;
    static rtw_country_chplan_map_size: c_uint;

    fn rtw_freq2ch(freq: c_int) -> c_int;
    fn rtw_chbw_to_freq_range(ch: u8, bw: u8, offset: u8, hi: *mut u32, lo: *mut u32) -> bool;
    fn rtw_chplan_warn_regd_mismatch(id: u8, regd_2g: u8, regd_5g: u8);
}

fn alpha_to_upper(c: u8) -> u8 {
    if (b'a'..=b'z').contains(&c) {
        c - b'a' + b'A'
    } else {
        c
    }
}

fn chplan_map() -> &'static [ChplanEnt] {
    unsafe {
        let n = RTW_ChannelPlanMap_size as usize;
        core::slice::from_raw_parts(&RTW_ChannelPlanMap, n)
    }
}

fn country_map() -> &'static [CountryChplan] {
    unsafe {
        let n = rtw_country_chplan_map_size as usize;
        core::slice::from_raw_parts(&country_chplan_map, n)
    }
}

/// Lookup helpers index `RTW_ChannelPlanMap[id]` with no bounds check — same
/// contract as the legacy C getters (callers must validate via
/// `rtw_is_channel_plan_valid` first).
unsafe fn chplan_ent_unchecked(id: u8) -> &'static ChplanEnt {
    unsafe { chplan_map().get_unchecked(id as usize) }
}

#[cfg(host_chplan_test)]
fn excl_chs_ptr(regsty: *const u8) -> *const u8 {
    unsafe { (*regsty.cast::<HostRegistryPriv>()).excl_chs.as_ptr() }
}

#[cfg(not(host_chplan_test))]
fn excl_chs_ptr(regsty: *const u8) -> *const u8 {
    unsafe { regsty.add(layout::EXCL_CHS_OFFSET) }
}

#[no_mangle]
pub extern "C" fn rtw_rust_chplan_probe() -> c_int {
    0x7717
}

#[no_mangle]
pub extern "C" fn rtw_chplan_get_default_regd_2g(id: u8) -> u8 {
    unsafe { chplan_ent_unchecked(id).regd_2g }
}

#[no_mangle]
pub extern "C" fn rtw_chplan_get_default_regd_5g(id: u8) -> u8 {
    #[cfg(ieee80211_band_5ghz)]
    {
        unsafe { chplan_ent_unchecked(id).regd_5g }
    }
    #[cfg(not(ieee80211_band_5ghz))]
    {
        let _ = id;
        TXPWR_LMT_NONE
    }
}

#[no_mangle]
pub extern "C" fn rtw_chplan_get_default_regd(id: u8) -> u8 {
    let regd_2g = rtw_chplan_get_default_regd_2g(id);
    let regd_5g = rtw_chplan_get_default_regd_5g(id);

    if regd_2g != TXPWR_LMT_NONE && regd_5g != TXPWR_LMT_NONE {
        if regd_2g != regd_5g {
            unsafe {
                rtw_chplan_warn_regd_mismatch(id, regd_2g, regd_5g);
            }
        }
        return regd_5g;
    }
    if regd_2g != TXPWR_LMT_NONE {
        regd_2g
    } else {
        regd_5g
    }
}

#[no_mangle]
pub extern "C" fn rtw_chplan_is_empty(id: u8) -> bool {
    let ent = unsafe { chplan_ent_unchecked(id) };
    if ent.chd_2g != RTW_CHD_2G_NULL {
        return false;
    }
    #[cfg(ieee80211_band_5ghz)]
    {
        ent.chd_5g == RTW_CHD_5G_NULL
    }
    #[cfg(not(ieee80211_band_5ghz))]
    {
        true
    }
}

#[no_mangle]
pub extern "C" fn rtw_is_channel_plan_valid(id: u8) -> bool {
    (id as i32) < unsafe { RTW_ChannelPlanMap_size } && !rtw_chplan_is_empty(id)
}

#[no_mangle]
pub extern "C" fn rtw_regsty_is_excl_chs(regsty: *const u8, ch: u8) -> bool {
    if regsty.is_null() {
        return false;
    }
    let excl = excl_chs_ptr(regsty);
    for i in 0..MAX_CHANNEL_NUM {
        let slot = unsafe { *excl.add(i) };
        if slot == 0 {
            break;
        }
        if slot == ch {
            return true;
        }
    }
    false
}
