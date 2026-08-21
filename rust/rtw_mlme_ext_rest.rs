// SPDX-License-Identifier: GPL-2.0
//! MLME ext rest helpers — Rust port of `core/rtw_mlme_ext_rest.c` chset slice (W3-54).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_mlme_ext_test)]
use std::os::raw::{c_int, c_ulong};

#[cfg(not(host_mlme_ext_test))]
use core::ffi::{c_int, c_ulong};

type U8 = u8;
type U32 = u32;
type Bool = bool;

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const MAX_CHANNEL_NUM: usize = 59;

const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;

const RTW_CHF_NO_IR: U8 = 1 << 0;
const RTW_CHF_NO_HT40U: U8 = 1 << 4;
const RTW_CHF_NO_HT40L: U8 = 1 << 5;
const RTW_CHF_NO_80MHZ: U8 = 1 << 6;
const RTW_CHF_NO_160MHZ: U8 = 1 << 7;
const RTW_CHF_NON_OCP: U8 = 1 << 3;

const HAL_PRIME_CHNL_OFFSET_DONT_CARE: U8 = 0;

#[cfg(any(dfs_master, host_mlme_ext_test))]
const NON_OCP_TIME_MS: c_int = 30 * 60 * 1000;

#[repr(C)]
pub struct RtChannelInfo {
    pub channel_num: U8,
    pub flags: U8,
    #[cfg(any(dfs_master, host_mlme_ext_test))]
    pub non_ocp_end_time: c_ulong,
}

type Systime = c_ulong;

extern "C" {
    fn rtw_ch2freq(chan: c_int) -> c_int;
    fn rtw_chbw_to_freq_range(ch: U8, bw: U8, offset: U8, hi: *mut U32, lo: *mut U32) -> Bool;
    fn rtw_get_center_ch(ch: U8, bw: U8, offset: U8) -> U8;
    fn rtw_get_op_chs_by_cch_bw(cch: U8, bw: U8, op_chs: *mut *mut U8, op_ch_num: *mut U8) -> U8;
    fn rtw_sync_chbw(
        req_ch: *mut U8,
        req_bw: *mut U8,
        req_offset: *mut U8,
        g_ch: *mut U8,
        g_bw: *mut U8,
        g_offset: *mut U8,
    );
}

#[cfg(host_mlme_ext_test)]
extern "C" {
    fn rtw_get_current_time() -> Systime;
    fn rtw_ms_to_systime(ms: c_int) -> Systime;
    fn rtw_systime_to_ms(stime: Systime) -> U32;
    fn rtw_time_after(a: Systime, b: Systime) -> Bool;
    fn rtw_warn_on(condition: c_int) -> c_int;
}

#[cfg(not(host_mlme_ext_test))]
extern "C" {
    fn _rtw_get_current_time() -> Systime;
    fn _rtw_ms_to_systime(ms: U32) -> Systime;
    fn _rtw_systime_to_ms(stime: Systime) -> U32;
    fn _rtw_time_after(a: Systime, b: Systime) -> Bool;
}

#[inline]
fn rust_warn_on(cond: bool) {
    #[cfg(host_mlme_ext_test)]
    unsafe {
        let _ = rtw_warn_on(cond as c_int);
    }
    #[cfg(not(host_mlme_ext_test))]
    let _ = cond;
}

fn current_time() -> Systime {
    unsafe {
        #[cfg(host_mlme_ext_test)]
        {
            rtw_get_current_time()
        }
        #[cfg(not(host_mlme_ext_test))]
        {
            _rtw_get_current_time()
        }
    }
}

fn ms_to_systime(ms: c_int) -> Systime {
    unsafe {
        #[cfg(host_mlme_ext_test)]
        {
            rtw_ms_to_systime(ms)
        }
        #[cfg(not(host_mlme_ext_test))]
        {
            _rtw_ms_to_systime(ms as U32)
        }
    }
}

fn systime_to_ms(stime: Systime) -> U32 {
    unsafe {
        #[cfg(host_mlme_ext_test)]
        {
            rtw_systime_to_ms(stime)
        }
        #[cfg(not(host_mlme_ext_test))]
        {
            _rtw_systime_to_ms(stime)
        }
    }
}

fn time_after(a: Systime, b: Systime) -> bool {
    unsafe {
        #[cfg(host_mlme_ext_test)]
        {
            rtw_time_after(a, b)
        }
        #[cfg(not(host_mlme_ext_test))]
        {
            _rtw_time_after(a, b)
        }
    }
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
fn ch_is_non_ocp(ent: &RtChannelInfo) -> bool {
    time_after(ent.non_ocp_end_time, current_time())
}

fn chset_slice(ch_set: *mut RtChannelInfo) -> &'static [RtChannelInfo] {
    // SAFETY: ch_set must remain valid for the duration of each exported call.
    unsafe { core::slice::from_raw_parts(ch_set, MAX_CHANNEL_NUM) }
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
fn rtw_chset_is_chbw_non_ocp_inner(ch_set: &[RtChannelInfo], ch: U8, bw: U8, offset: U8) -> bool {
    let mut hi: U32 = 0;
    let mut lo: U32 = 0;
    if unsafe { !rtw_chbw_to_freq_range(ch, bw, offset, &mut hi, &mut lo) } {
        return false;
    }
    for ent in ch_set {
        if ent.channel_num == 0 {
            break;
        }
        let freq = unsafe { rtw_ch2freq(ent.channel_num as c_int) };
        if freq == 0 {
            rust_warn_on(true);
            continue;
        }
        if !ch_is_non_ocp(ent) {
            continue;
        }
        let freq = freq as U32;
        if lo <= freq && freq <= hi {
            return true;
        }
    }
    false
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
#[no_mangle]
pub extern "C" fn rtw_chset_is_chbw_non_ocp(
    ch_set: *mut RtChannelInfo,
    ch: U8,
    bw: U8,
    offset: U8,
) -> Bool {
    if ch_set.is_null() {
        return false;
    }
    rtw_chset_is_chbw_non_ocp_inner(chset_slice(ch_set), ch, bw, offset)
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
#[no_mangle]
pub extern "C" fn rtw_chset_is_ch_non_ocp(ch_set: *mut RtChannelInfo, ch: U8) -> Bool {
    rtw_chset_is_chbw_non_ocp(
        ch_set,
        ch,
        CHANNEL_WIDTH_20,
        HAL_PRIME_CHNL_OFFSET_DONT_CARE,
    )
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
#[no_mangle]
pub extern "C" fn rtw_chset_get_ch_non_ocp_ms(
    ch_set: *mut RtChannelInfo,
    ch: U8,
    bw: U8,
    offset: U8,
) -> U32 {
    let mut ms: i32 = 0;
    let mut hi: U32 = 0;
    let mut lo: U32 = 0;
    if ch_set.is_null() || unsafe { !rtw_chbw_to_freq_range(ch, bw, offset, &mut hi, &mut lo) } {
        return 0;
    }
    let current_time = current_time();
    for ent in chset_slice(ch_set) {
        if ent.channel_num == 0 {
            break;
        }
        let freq = unsafe { rtw_ch2freq(ent.channel_num as c_int) };
        if freq == 0 {
            rust_warn_on(true);
            continue;
        }
        if !ch_is_non_ocp(ent) {
            continue;
        }
        let freq = freq as U32;
        if lo <= freq && freq <= hi {
            let remain = systime_to_ms(ent.non_ocp_end_time.wrapping_sub(current_time));
            if remain as i32 > ms {
                ms = remain as i32;
            }
        }
    }
    ms as U32
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
fn rtw_chset_update_non_ocp_inner(
    ch_set: &mut [RtChannelInfo],
    ch: U8,
    bw: U8,
    offset: U8,
    ms: c_int,
) -> bool {
    let mut hi: U32 = 0;
    let mut lo: U32 = 0;
    let mut updated = false;
    if unsafe { !rtw_chbw_to_freq_range(ch, bw, offset, &mut hi, &mut lo) } {
        return false;
    }
    for ent in ch_set.iter_mut() {
        if ent.channel_num == 0 {
            break;
        }
        let freq = unsafe { rtw_ch2freq(ent.channel_num as c_int) };
        if freq == 0 {
            rust_warn_on(true);
            continue;
        }
        let freq = freq as U32;
        if lo <= freq && freq <= hi {
            ent.non_ocp_end_time = if ms >= 0 {
                current_time().wrapping_add(ms_to_systime(ms))
            } else {
                current_time().wrapping_add(ms_to_systime(NON_OCP_TIME_MS))
            };
            ent.flags |= RTW_CHF_NON_OCP;
            updated = true;
        }
    }
    updated
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
#[no_mangle]
pub extern "C" fn rtw_chset_update_non_ocp(
    ch_set: *mut RtChannelInfo,
    ch: U8,
    bw: U8,
    offset: U8,
) -> Bool {
    if ch_set.is_null() {
        return false;
    }
    let slice = unsafe { core::slice::from_raw_parts_mut(ch_set, MAX_CHANNEL_NUM) };
    rtw_chset_update_non_ocp_inner(slice, ch, bw, offset, -1)
}

#[cfg(any(dfs_master, host_mlme_ext_test))]
#[no_mangle]
pub extern "C" fn rtw_chset_update_non_ocp_ms(
    ch_set: *mut RtChannelInfo,
    ch: U8,
    bw: U8,
    offset: U8,
    ms: c_int,
) -> Bool {
    if ch_set.is_null() {
        return false;
    }
    let slice = unsafe { core::slice::from_raw_parts_mut(ch_set, MAX_CHANNEL_NUM) };
    rtw_chset_update_non_ocp_inner(slice, ch, bw, offset, ms)
}

#[no_mangle]
pub extern "C" fn rtw_chset_search_ch(ch_set: *mut RtChannelInfo, ch: U32) -> c_int {
    if ch == 0 || ch_set.is_null() {
        return -1;
    }
    for (i, ent) in chset_slice(ch_set).iter().enumerate() {
        if ent.channel_num == 0 {
            break;
        }
        if ch == ent.channel_num as U32 {
            return i as c_int;
        }
    }
    -1
}

#[no_mangle]
pub extern "C" fn rtw_chset_is_chbw_valid(
    ch_set: *mut RtChannelInfo,
    ch: U8,
    bw: U8,
    offset: U8,
    allow_primary_passive: Bool,
    allow_passive: Bool,
) -> U8 {
    if ch_set.is_null() {
        return 0;
    }
    let cch = unsafe { rtw_get_center_ch(ch, bw, offset) };
    let mut op_chs: *mut U8 = core::ptr::null_mut();
    let mut op_ch_num: U8 = 0;
    if unsafe { rtw_get_op_chs_by_cch_bw(cch, bw, &mut op_chs, &mut op_ch_num) } == 0 {
        return 0;
    }
    let mut i = 0usize;
    while (i as U8) < op_ch_num {
        let ch_idx = rtw_chset_search_ch(ch_set, unsafe { *op_chs.add(i) } as U32);
        if ch_idx < 0 {
            break;
        }
        let idx = ch_idx as usize;
        let ent = &chset_slice(ch_set)[idx];
        if (ent.flags & RTW_CHF_NO_IR) != 0 {
            if (!allow_primary_passive && ent.channel_num == ch)
                || (!allow_passive && ent.channel_num != ch)
            {
                break;
            }
        }
        if bw >= CHANNEL_WIDTH_40 {
            if (ent.flags & RTW_CHF_NO_HT40U) != 0 && i % 2 == 0 {
                break;
            }
            if (ent.flags & RTW_CHF_NO_HT40L) != 0 && i % 2 == 1 {
                break;
            }
        }
        if bw >= 2 && (ent.flags & RTW_CHF_NO_80MHZ) != 0 {
            break;
        }
        if bw >= 3 && (ent.flags & RTW_CHF_NO_160MHZ) != 0 {
            break;
        }
        i += 1;
    }
    if op_ch_num != 0 && (i as U8) == op_ch_num {
        1
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_chset_sync_chbw(
    ch_set: *mut RtChannelInfo,
    req_ch: *mut U8,
    req_bw: *mut U8,
    req_offset: *mut U8,
    g_ch: *mut U8,
    g_bw: *mut U8,
    g_offset: *mut U8,
    allow_primary_passive: Bool,
    allow_passive: Bool,
) {
    if ch_set.is_null()
        || req_ch.is_null()
        || req_bw.is_null()
        || req_offset.is_null()
        || g_ch.is_null()
        || g_bw.is_null()
        || g_offset.is_null()
    {
        return;
    }
    let mut cur_bw = unsafe { *req_bw };
    loop {
        let mut r_ch = unsafe { *req_ch };
        let mut r_bw = cur_bw;
        let mut r_offset = unsafe { *req_offset };
        let mut u_ch = unsafe { *g_ch };
        let mut u_bw = unsafe { *g_bw };
        let mut u_offset = unsafe { *g_offset };
        unsafe {
            rtw_sync_chbw(
                &mut r_ch,
                &mut r_bw,
                &mut r_offset,
                &mut u_ch,
                &mut u_bw,
                &mut u_offset,
            );
        }
        if rtw_chset_is_chbw_valid(
            ch_set,
            r_ch,
            r_bw,
            r_offset,
            allow_primary_passive,
            allow_passive,
        ) != 0
        {
            unsafe {
                *req_ch = r_ch;
                *req_bw = r_bw;
                *req_offset = r_offset;
                *g_ch = u_ch;
                *g_bw = u_bw;
                *g_offset = u_offset;
            }
            break;
        }
        if cur_bw == CHANNEL_WIDTH_20 {
            rust_warn_on(true);
            unsafe {
                *req_ch = r_ch;
                *req_bw = r_bw;
                *req_offset = r_offset;
                *g_ch = u_ch;
                *g_bw = u_bw;
                *g_offset = u_offset;
            }
            break;
        }
        cur_bw -= 1;
    }
}
