// SPDX-License-Identifier: GPL-2.0
//! IEEE 802.11 IE helpers — Rust port of `core/rtw_ieee80211.c` slices (W3-03).

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

#[cfg(host_ie_test)]
use std::os::raw::c_int;

#[cfg(not(host_ie_test))]
use core::ffi::c_int;

type Sint = i32;

const _TRUE: i32 = 1;
const _FAIL: i32 = 0;
const _SUCCESS: i32 = 1;

unsafe extern "C" {
    fn memcpy(dst: *mut u8, src: *const u8, n: usize) -> *mut u8;
    fn memmove(dst: *mut u8, src: *const u8, n: usize) -> *mut u8;
    fn memcmp(s1: *const u8, s2: *const u8, n: usize) -> i32;
}

#[unsafe(no_mangle)]
pub extern "C" fn rtw_rust_ieee80211_probe() -> c_int {
    0x1e03
}

#[unsafe(no_mangle)]
pub extern "C" fn rtw_get_ie(pbuf: *const u8, index: Sint, len: *mut Sint, limit: Sint) -> *mut u8 {
    if limit < 1 {
        return core::ptr::null_mut();
    }
    unsafe {
        let mut p = pbuf;
        let mut i: Sint = 0;
        *len = 0;
        loop {
            if *p == index as u8 {
                *len = *(p.add(1)) as Sint;
                return p as *mut u8;
            }
            let tmp = *(p.add(1)) as Sint;
            p = p.add((tmp + 2) as usize);
            i += tmp + 2;
            if i >= limit {
                break;
            }
        }
    }
    core::ptr::null_mut()
}

#[unsafe(no_mangle)]
pub extern "C" fn rtw_get_ie_ex(
    in_ie: *const u8,
    in_len: u32,
    eid: u8,
    oui: *const u8,
    oui_len: u8,
    ie: *mut u8,
    ielen: *mut u32,
) -> *mut u8 {
    let mut cnt: u32 = 0;
    let mut target_ie: *const u8 = core::ptr::null();

    unsafe {
        if !ielen.is_null() {
            *ielen = 0;
        }
        if in_ie.is_null() || in_len <= 0 {
            return target_ie as *mut u8;
        }
        while cnt < in_len {
            if eid == *in_ie.add(cnt as usize)
                && (oui.is_null()
                    || memcmp(
                        in_ie.add(cnt as usize + 2),
                        oui,
                        oui_len as usize,
                    ) == 0)
            {
                target_ie = in_ie.add(cnt as usize);
                if !ie.is_null() {
                    memcpy(
                        ie,
                        in_ie.add(cnt as usize),
                        (*in_ie.add(cnt as usize + 1) as usize) + 2,
                    );
                }
                if !ielen.is_null() {
                    *ielen = (*in_ie.add(cnt as usize + 1) as u32) + 2;
                }
                break;
            }
            cnt += (*in_ie.add(cnt as usize + 1) as u32) + 2;
        }
    }
    target_ie as *mut u8
}

#[unsafe(no_mangle)]
pub extern "C" fn rtw_ies_remove_ie(
    ies: *mut u8,
    ies_len: *mut u32,
    offset: u32,
    eid: u8,
    oui: *mut u8,
    oui_len: u8,
) -> i32 {
    let mut ret = _FAIL;
    unsafe {
        if ies.is_null() || ies_len.is_null() || *ies_len <= offset {
            return ret;
        }
        let mut start = ies.add(offset as usize);
        let mut search_len = (*ies_len - offset) as u32;
        loop {
            let mut target_ielen: u32 = 0;
            let target_ie = rtw_get_ie_ex(
                start,
                search_len,
                eid,
                oui,
                oui_len,
                core::ptr::null_mut(),
                &mut target_ielen,
            );
            if !target_ie.is_null() && target_ielen != 0 {
                let remain_ies = target_ie.add(target_ielen as usize);
                let remain_len = search_len - (remain_ies.offset_from(start) as u32);
                memmove(target_ie, remain_ies, remain_len as usize);
                *ies_len -= target_ielen;
                ret = _SUCCESS;
                start = target_ie;
                search_len = remain_len;
            } else {
                break;
            }
        }
    }
    ret
}
