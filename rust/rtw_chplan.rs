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
use std::os::raw::{c_char, c_int, c_uint};

#[cfg(not(host_chplan_test))]
use core::ffi::{c_char, c_int, c_uint};

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
pub struct RegistryPriv {
    pub excl_chs: [u8; MAX_CHANNEL_NUM],
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

extern "C" {
    static RTW_ChannelPlanMap: ChplanEnt;
    static RTW_ChannelPlanMap_size: c_int;
    static country_chplan_map: CountryChplan;
    static rtw_country_chplan_map_size: c_uint;

    fn rtw_freq2ch(freq: c_int) -> c_int;
    fn rtw_chbw_to_freq_range(ch: u8, bw: u8, offset: u8, hi: *mut u32, lo: *mut u32) -> bool;
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

#[no_mangle]
pub extern "C" fn rtw_rust_chplan_probe() -> c_int {
    0x7717
}

#[no_mangle]
pub extern "C" fn rtw_chplan_get_default_regd_2g(id: u8) -> u8 {
    chplan_map()[id as usize].regd_2g
}

#[no_mangle]
pub extern "C" fn rtw_chplan_get_default_regd_5g(id: u8) -> u8 {
    chplan_map()[id as usize].regd_5g
}

#[no_mangle]
pub extern "C" fn rtw_chplan_get_default_regd(id: u8) -> u8 {
    let regd_2g = rtw_chplan_get_default_regd_2g(id);
    let regd_5g = rtw_chplan_get_default_regd_5g(id);

    if regd_2g != TXPWR_LMT_NONE && regd_5g != TXPWR_LMT_NONE {
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
    let ent = &chplan_map()[id as usize];
    ent.chd_2g == RTW_CHD_2G_NULL && ent.chd_5g == RTW_CHD_5G_NULL
}

#[no_mangle]
pub extern "C" fn rtw_is_channel_plan_valid(id: u8) -> bool {
    (id as i32) < unsafe { RTW_ChannelPlanMap_size } && !rtw_chplan_is_empty(id)
}

#[no_mangle]
pub extern "C" fn rtw_regsty_is_excl_chs(regsty: *const RegistryPriv, ch: u8) -> bool {
    if regsty.is_null() {
        return false;
    }
    let regsty = unsafe { &*regsty };
    for slot in regsty.excl_chs.iter() {
        if *slot == 0 {
            break;
        }
        if *slot == ch {
            return true;
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn rtw_chset_is_dfs_range(chset: *mut RtChannelInfo, hi: u32, lo: u32) -> bool {
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
