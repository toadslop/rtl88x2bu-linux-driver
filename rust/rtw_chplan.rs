// SPDX-License-Identifier: GPL-2.0
//! Channel-plan lookup / DFS / country helpers — Rust port of `core/rtw_chplan.c`
//! slices (W2-17..W2-20).

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
use std::os::raw::{c_char, c_int, c_uint};

#[cfg(not(host_chplan_test))]
use core::ffi::{c_char, c_int, c_uint};

const TXPWR_LMT_NONE: u8 = 0;
const RTW_CHD_2G_NULL: u8 = 0;
const RTW_CHD_5G_NULL: u8 = 0;
const RTW_CHF_DFS: u8 = 1 << 1;
const RTW_CHF_NO_IR: u8 = 1 << 0;
const MAX_CHANNEL_NUM: usize = 59;
const _TRUE: i32 = 1;
const _FALSE: i32 = 0;

const REGD_SRC_RTK_PRIV: u8 = 0;
const REGD_SRC_OS: u8 = 1;

const CLA_2G_12_14_PASSIVE: u8 = 1 << 0;
const CLA_5G_B1_PASSIVE: u8 = 1 << 0;
const CLA_5G_B2_PASSIVE: u8 = 1 << 1;
const CLA_5G_B3_PASSIVE: u8 = 1 << 2;
const CLA_5G_B4_PASSIVE: u8 = 1 << 3;
const CLA_5G_B2_DFS: u8 = 1 << 4;
const CLA_5G_B3_DFS: u8 = 1 << 5;
const CLA_5G_B4_DFS: u8 = 1 << 6;

const WIRELESS_MODE_24G: u8 = 0x0b; /* 11B|11G|11_24N */
const WIRELESS_MODE_5G: u8 = 0x54; /* 11A|11_5N|11AC */

const BAND_CAP_2G: u8 = 1 << 0;
const BAND_CAP_5G: u8 = 1 << 1;

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
    wireless_mode: u8,
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

    fn rtw_chdef_2g_len(chd: u8) -> u8;
    fn rtw_chdef_2g_ch(chd: u8, i: u8) -> u8;
    fn rtw_chdef_2g_attrib(chd: u8) -> u8;
    #[cfg(ieee80211_band_5ghz)]
    fn rtw_chdef_5g_len(chd: u8) -> u8;
    #[cfg(ieee80211_band_5ghz)]
    fn rtw_chdef_5g_ch(chd: u8, i: u8) -> u8;
    #[cfg(ieee80211_band_5ghz)]
    fn rtw_chdef_5g_attrib(chd: u8) -> u8;

    fn rtw_rust_rfctl_channel_plan(adapter: *mut u8) -> u8;
    fn rtw_rust_rfctl_regd_src(adapter: *mut u8) -> u8;
    fn rtw_rust_rfctl_channel_set(adapter: *mut u8) -> *mut RtChannelInfo;
    fn rtw_rust_rfctl_country_ent(adapter: *mut u8) -> *const CountryChplan;
    fn rtw_rust_regsty_wireless_mode(adapter: *mut u8) -> u8;
    fn rtw_rust_adapter_regsty(adapter: *mut u8) -> *mut u8;

    fn hal_chk_band_cap(adapter: *mut u8, cap: u8) -> bool;
    #[cfg(regd_src_from_os)]
    fn rtw_os_init_channel_set(adapter: *mut u8, channel_set: *mut RtChannelInfo) -> u8;
    fn memset(s: *mut u8, c: c_int, n: usize) -> *mut u8;
    fn rtw_rust_chset_set_non_ocp(chset: *mut RtChannelInfo, count: u8);
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

fn is_supported_24g(wireless_mode: u8) -> bool {
    wireless_mode & WIRELESS_MODE_24G != 0
}

fn is_supported_5g(wireless_mode: u8) -> bool {
    wireless_mode & WIRELESS_MODE_5G != 0
}

fn rtw_is_5g_band1(ch: u8) -> bool {
    (36..=48).contains(&ch)
}
fn rtw_is_5g_band2(ch: u8) -> bool {
    (52..=64).contains(&ch)
}
fn rtw_is_5g_band3(ch: u8) -> bool {
    (100..=144).contains(&ch)
}
fn rtw_is_5g_band4(ch: u8) -> bool {
    (149..=177).contains(&ch)
}

#[no_mangle]
pub extern "C" fn rtw_rust_chplan_probe() -> c_int {
    0x7720
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

#[no_mangle]
pub extern "C" fn rtw_chset_is_dfs_range(chset: *mut RtChannelInfo, hi: u32, lo: u32) -> bool {
    // Intentional hardening: legacy C dereferences chset unconditionally.
    if chset.is_null() {
        return false;
    }
    let hi_ch = unsafe { rtw_freq2ch(hi as c_int) } as u8;
    let lo_ch = unsafe { rtw_freq2ch(lo as c_int) } as u8;
    let chset = unsafe { core::slice::from_raw_parts(chset, MAX_CHANNEL_NUM) };

    for ent in chset.iter() {
        if ent.channel_num == 0 {
            break;
        }
        if ent.flags & RTW_CHF_DFS == 0 {
            continue;
        }
        if hi_ch > ent.channel_num && lo_ch < ent.channel_num {
            return true;
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn rtw_chset_is_dfs_ch(chset: *mut RtChannelInfo, ch: u8) -> bool {
    // See rtw_chset_is_dfs_range: NULL chset returns false instead of faulting.
    if chset.is_null() {
        return false;
    }
    let chset = unsafe { core::slice::from_raw_parts(chset, MAX_CHANNEL_NUM) };
    for ent in chset.iter() {
        if ent.channel_num == 0 {
            break;
        }
        if ent.channel_num == ch {
            return ent.flags & RTW_CHF_DFS != 0;
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn rtw_chset_is_dfs_chbw(
    chset: *mut RtChannelInfo,
    ch: u8,
    bw: u8,
    offset: u8,
) -> bool {
    let mut hi: u32 = 0;
    let mut lo: u32 = 0;

    if !unsafe { rtw_chbw_to_freq_range(ch, bw, offset, &mut hi, &mut lo) } {
        return false;
    }
    rtw_chset_is_dfs_range(chset, hi, lo)
}

#[no_mangle]
pub extern "C" fn rtw_get_chplan_from_country(
    country_code: *const c_char,
) -> *const CountryChplan {
    if country_code.is_null() {
        return core::ptr::null();
    }
    let c0 = unsafe { *country_code as u8 };
    let c1 = unsafe { *country_code.add(1) as u8 };
    let code = [alpha_to_upper(c0), alpha_to_upper(c1)];

    for ent in country_map() {
        if ent.alpha2 == code {
            return ent as *const CountryChplan;
        }
    }
    core::ptr::null()
}

unsafe fn init_channel_set_from_rtk_priv(adapter: *mut u8, channel_set: *mut RtChannelInfo) -> u8 {
    let channel_plan = unsafe { rtw_rust_rfctl_channel_plan(adapter) };
    let country_ent = unsafe { rtw_rust_rfctl_country_ent(adapter) };
    let regsty = unsafe { rtw_rust_adapter_regsty(adapter) };
    let wireless_mode = unsafe { rtw_rust_regsty_wireless_mode(adapter) };
    let mut chanset_size: u8 = 0;

    if !rtw_is_channel_plan_valid(channel_plan) {
        return 0;
    }

    unsafe {
        memset(
            channel_set.cast(),
            0,
            core::mem::size_of::<RtChannelInfo>() * MAX_CHANNEL_NUM,
        );
    }

    let b2_4g = is_supported_24g(wireless_mode)
        && unsafe { hal_chk_band_cap(adapter, BAND_CAP_2G) };
    let b5g = is_supported_5g(wireless_mode) && unsafe { hal_chk_band_cap(adapter, BAND_CAP_5G) };

    if !b2_4g && !b5g {
        return 0;
    }

    if b2_4g {
        let chd_2g = unsafe { chplan_ent_unchecked(channel_plan).chd_2g };
        let attrib = unsafe { rtw_chdef_2g_attrib(chd_2g) };
        let len = unsafe { rtw_chdef_2g_len(chd_2g) };
        for index in 0..len {
            let ch = unsafe { rtw_chdef_2g_ch(chd_2g, index) };
            if rtw_regsty_is_excl_chs(regsty, ch) {
                continue;
            }
            if chanset_size as usize >= MAX_CHANNEL_NUM {
                break;
            }
            let ent = unsafe { &mut *channel_set.add(chanset_size as usize) };
            ent.channel_num = ch;
            if (12..=14).contains(&ch) && attrib & CLA_2G_12_14_PASSIVE != 0 {
                ent.flags |= RTW_CHF_NO_IR;
            }
            let _ = country_ent;
            chanset_size += 1;
        }
    }

    #[cfg(ieee80211_band_5ghz)]
    if b5g {
        let chd_5g = unsafe { chplan_ent_unchecked(channel_plan).chd_5g };
        let attrib = unsafe { rtw_chdef_5g_attrib(chd_5g) };
        let len = unsafe { rtw_chdef_5g_len(chd_5g) };
        for index in 0..len {
            let ch = unsafe { rtw_chdef_5g_ch(chd_5g, index) };
            if rtw_regsty_is_excl_chs(regsty, ch) {
                continue;
            }
            let dfs = (rtw_is_5g_band2(ch) && attrib & CLA_5G_B2_DFS != 0)
                || (rtw_is_5g_band3(ch) && attrib & CLA_5G_B3_DFS != 0)
                || (rtw_is_5g_band4(ch) && attrib & CLA_5G_B4_DFS != 0);
            #[cfg(not(dfs))]
            if dfs {
                continue;
            }
            if chanset_size as usize >= MAX_CHANNEL_NUM {
                break;
            }
            let ent = unsafe { &mut *channel_set.add(chanset_size as usize) };
            ent.channel_num = ch;
            if (rtw_is_5g_band1(ch) && attrib & CLA_5G_B1_PASSIVE != 0)
                || (rtw_is_5g_band2(ch) && attrib & CLA_5G_B2_PASSIVE != 0)
                || (rtw_is_5g_band3(ch) && attrib & CLA_5G_B3_PASSIVE != 0)
                || (rtw_is_5g_band4(ch) && attrib & CLA_5G_B4_PASSIVE != 0)
            {
                ent.flags |= RTW_CHF_NO_IR;
            }
            if dfs {
                ent.flags |= RTW_CHF_DFS;
            }
            let _ = country_ent;
            chanset_size += 1;
        }

        unsafe { rtw_rust_chset_set_non_ocp(channel_set, chanset_size) };
    }

    chanset_size
}

#[no_mangle]
pub extern "C" fn init_channel_set(adapter: *mut u8) -> u8 {
    if adapter.is_null() {
        return 0;
    }
    let regd_src = unsafe { rtw_rust_rfctl_regd_src(adapter) };
    let channel_set = unsafe { rtw_rust_rfctl_channel_set(adapter) };
    if channel_set.is_null() {
        return 0;
    }
    if regd_src == REGD_SRC_RTK_PRIV {
        return unsafe { init_channel_set_from_rtk_priv(adapter, channel_set) };
    }
    #[cfg(regd_src_from_os)]
    if regd_src == REGD_SRC_OS {
        return unsafe { rtw_os_init_channel_set(adapter, channel_set) };
    }
    0
}
