// SPDX-License-Identifier: GPL-2.0
//! W3-66 802.11d country IE processing — host L2 oracle and kernel port.
#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    missing_docs
)]
#[cfg(all(not(host_mlme_80211d_test), rust_mlme_80211d))]
use core::ffi::{c_int, c_void};
#[cfg(host_mlme_80211d_test)]
use std::os::raw::c_int;

type U8 = u8;
const MAX_CH: usize = 59;
const FIXED_IE: u32 = 12;
const COUNTRY_IE: c_int = 7;
const NO_IR: U8 = 0x01;
const MODE_G: U8 = 0x02;
const MODE_A: U8 = 0x04;

#[rustfmt::skip]
#[repr(C)]
#[derive(Clone, Copy)]
struct ChInfo { num: U8, flags: U8 }
#[rustfmt::skip]
#[repr(C)]
struct ChPlan { ch: [U8; MAX_CH], len: U8 }

#[cfg(host_mlme_80211d_test)]
#[rustfmt::skip]
#[repr(C)]
struct RegistryPriv { enable80211d: U8, wireless_mode: U8 }
#[cfg(host_mlme_80211d_test)]
#[rustfmt::skip]
#[repr(C)]
struct MlmeExtPriv { update_channel_plan_by_ap_done: U8 }
#[cfg(host_mlme_80211d_test)]
#[rustfmt::skip]
#[repr(C)]
struct RfCtl { channel_set: [ChInfo; MAX_CH] }
#[cfg(host_mlme_80211d_test)]
#[rustfmt::skip]
#[repr(C)]
struct Ssid { len: u32, ssid: [U8; 32] }
#[cfg(host_mlme_80211d_test)]
#[repr(C)]
struct WlanBssidEx {
    length: u32,
    mac: [U8; 6],
    reserved: [U8; 2],
    ssid: Ssid,
    ie_length: u32,
    ies: [U8; 768],
}
#[cfg(host_mlme_80211d_test)]
#[repr(C)]
struct Adapter {
    registrypriv: RegistryPriv,
    mlmeextpriv: MlmeExtPriv,
    rfctl: RfCtl,
}

#[cfg(host_mlme_80211d_test)]
type Pad = *mut Adapter;
#[cfg(host_mlme_80211d_test)]
type Pbs = *mut WlanBssidEx;
#[cfg(not(host_mlme_80211d_test))]
type Pad = *mut c_void;
#[cfg(not(host_mlme_80211d_test))]
type Pbs = *mut c_void;

#[cfg(host_mlme_80211d_test)]
extern "C" {
    fn rtw_get_ie(p: *const U8, id: c_int, len: *mut c_int, lim: c_int) -> *mut U8;
}
#[cfg(not(host_mlme_80211d_test))]
extern "C" {
    fn _rtw_memset(s: *mut c_void, c: c_int, n: usize) -> *mut c_void;
    fn _rtw_memcpy(d: *mut c_void, s: *const c_void, n: usize) -> *mut c_void;
    fn rtw_rust_80211d_get_ie(p: *const U8, id: c_int, len: *mut c_int, lim: c_int) -> *mut U8;
    fn rtw_rust_80211d_enable(a: Pad) -> U8;
    fn rtw_rust_80211d_wireless_mode(a: Pad) -> U8;
    fn rtw_rust_80211d_update_done(a: Pad) -> U8;
    fn rtw_rust_80211d_set_update_done(a: Pad, v: U8);
    fn rtw_rust_80211d_ch_num(a: Pad, i: U8) -> U8;
    fn rtw_rust_80211d_ch_flags(a: Pad, i: U8) -> U8;
    fn rtw_rust_80211d_zero_chset(a: Pad);
    fn rtw_rust_80211d_set_ch(a: Pad, i: U8, num: U8, flags: U8);
    fn rtw_rust_80211d_reg_change(a: Pad);
    fn rtw_rust_80211d_bss_ies(b: Pbs) -> *mut U8;
    fn rtw_rust_80211d_bss_ie_len(b: Pbs) -> u32;
}

#[rustfmt::skip]
fn mem_cpy(d: *mut u8, s: *const u8, n: usize) {
    #[cfg(host_mlme_80211d_test)]
    unsafe { core::ptr::copy_nonoverlapping(s, d, n) }
    #[cfg(not(host_mlme_80211d_test))]
    unsafe { let _ = _rtw_memcpy(d as *mut c_void, s as *const c_void, n); }
}
#[rustfmt::skip]
fn mem_set(d: *mut u8, c: u8, n: usize) {
    #[cfg(host_mlme_80211d_test)]
    unsafe { core::ptr::write_bytes(d, c, n) }
    #[cfg(not(host_mlme_80211d_test))]
    unsafe { let _ = _rtw_memset(d as *mut c_void, c as c_int, n); }
}

#[rustfmt::skip]
fn parse_ap(p: *const U8, end: *const U8, ap: &mut ChPlan) {
    let mut i = 0usize;
    let mut cur = p;
    unsafe {
        while (end as usize).saturating_sub(cur as usize) >= 3 {
            let (fcn, noc) = (*cur, *cur.add(1));
            cur = cur.add(3);
            for j in 0..noc {
                let ch = if fcn <= 14 { fcn.wrapping_add(j) } else { fcn.wrapping_add(j * 4) };
                if i >= MAX_CH { break; }
                ap.ch[i] = ch;
                i += 1;
            }
        }
    }
    ap.len = i as U8;
}

fn push_no_ir(dst: &mut ChInfo, src: &ChInfo) {
    dst.num = src.num;
    dst.flags |= NO_IR;
}

#[rustfmt::skip]
fn merge_2g(sta: &[ChInfo], ap: &ChPlan, new: &mut [ChInfo], g: bool, i: &mut usize, j: &mut usize, k: &mut usize) {
    if g {
        loop {
            if *k >= MAX_CH { break; }
            if *i >= MAX_CH || sta[*i].num == 0 || sta[*i].num > 14 { break; }
            if *j >= ap.len as usize || ap.ch[*j] > 14 { break; }
            if sta[*i].num == ap.ch[*j] {
                new[*k].num = ap.ch[*j]; *i += 1; *j += 1; *k += 1;
            } else if sta[*i].num < ap.ch[*j] {
                push_no_ir(&mut new[*k], &sta[*i]); *i += 1; *k += 1;
            } else { new[*k].num = ap.ch[*j]; *j += 1; *k += 1; }
        }
        while *k < MAX_CH && *i < MAX_CH && sta[*i].num != 0 && sta[*i].num <= 14 {
            push_no_ir(&mut new[*k], &sta[*i]); *i += 1; *k += 1;
        }
        while *k < MAX_CH && *j < ap.len as usize && ap.ch[*j] <= 14 { new[*k].num = ap.ch[*j]; *j += 1; *k += 1; }
    } else {
        while *k < MAX_CH && *i < MAX_CH && sta[*i].num != 0 && sta[*i].num <= 14 {
            new[*k].num = sta[*i].num;
            if sta[*i].flags & NO_IR != 0 { new[*k].flags |= NO_IR; }
            *i += 1; *k += 1;
        }
        while *j < ap.len as usize && ap.ch[*j] <= 14 { *j += 1; }
    }
}

#[rustfmt::skip]
fn merge_5g(sta: &[ChInfo], ap: &ChPlan, new: &mut [ChInfo], a: bool, i: &mut usize, j: &mut usize, k: &mut usize) {
    if a {
        loop {
            if *k >= MAX_CH { break; }
            if *i >= MAX_CH || sta[*i].num == 0 { break; }
            if *j >= ap.len as usize || ap.ch[*j] == 0 { break; }
            if sta[*i].num == ap.ch[*j] {
                new[*k].num = ap.ch[*j]; *i += 1; *j += 1; *k += 1;
            } else if sta[*i].num < ap.ch[*j] {
                push_no_ir(&mut new[*k], &sta[*i]); *i += 1; *k += 1;
            } else { new[*k].num = ap.ch[*j]; *j += 1; *k += 1; }
        }
        while *k < MAX_CH && *i < MAX_CH && sta[*i].num != 0 { push_no_ir(&mut new[*k], &sta[*i]); *i += 1; *k += 1; }
        while *k < MAX_CH && *j < ap.len as usize && ap.ch[*j] != 0 { new[*k].num = ap.ch[*j]; *j += 1; *k += 1; }
    } else {
        while *k < MAX_CH && *i < MAX_CH && sta[*i].num != 0 {
            new[*k].num = sta[*i].num;
            if sta[*i].flags & NO_IR != 0 { new[*k].flags |= NO_IR; }
            *i += 1; *k += 1;
        }
    }
}

#[rustfmt::skip]
#[no_mangle]
pub extern "C" fn process_80211d(padapter: Pad, bssid: Pbs) {
    if padapter.is_null() || bssid.is_null() { return; }
    let (en, done, mode, ies, lim) = unsafe {
        #[cfg(host_mlme_80211d_test)]
        {
            let a = &*padapter;
            let b = &*bssid;
            (a.registrypriv.enable80211d != 0, a.mlmeextpriv.update_channel_plan_by_ap_done != 0,
             a.registrypriv.wireless_mode, b.ies.as_ptr(),
             b.ie_length.saturating_sub(FIXED_IE) as c_int)
        }
        #[cfg(not(host_mlme_80211d_test))]
        {
            (rtw_rust_80211d_enable(padapter) != 0, rtw_rust_80211d_update_done(padapter) != 0,
             rtw_rust_80211d_wireless_mode(padapter), rtw_rust_80211d_bss_ies(bssid),
             (rtw_rust_80211d_bss_ie_len(bssid).saturating_sub(FIXED_IE)) as c_int)
        }
    };
    if !en || done { return; }
    let mut len = 0;
    let ie = unsafe {
        let p = ies.add(FIXED_IE as usize);
        #[cfg(host_mlme_80211d_test)]
        { rtw_get_ie(p, COUNTRY_IE, &mut len, lim) }
        #[cfg(not(host_mlme_80211d_test))]
        { rtw_rust_80211d_get_ie(p, COUNTRY_IE, &mut len, lim) }
    };
    if ie.is_null() || len < 6 { return; }
    let mut ap = ChPlan { ch: [0; MAX_CH], len: 0 };
    unsafe { parse_ap(ie.add(5), ie.add(2 + len as usize), &mut ap) }
    /* Packed {num,flags} working copy. Kernel RT_CHANNEL_INFO is 32 bytes
     * (FIND_BEST_CHANNEL + DFS_MASTER + cfg80211 os_chan); never memcpy it. */
    let mut sta_buf = [ChInfo { num: 0, flags: 0 }; MAX_CH];
    let mut new_buf = [ChInfo { num: 0, flags: 0 }; MAX_CH];
    unsafe {
        #[cfg(host_mlme_80211d_test)]
        {
            sta_buf.copy_from_slice(&(*padapter).rfctl.channel_set);
        }
        #[cfg(not(host_mlme_80211d_test))]
        {
            for n in 0..MAX_CH {
                sta_buf[n].num = rtw_rust_80211d_ch_num(padapter, n as U8);
                sta_buf[n].flags = rtw_rust_80211d_ch_flags(padapter, n as U8);
            }
        }
    }
    let (mut i, mut j, mut k) = (0usize, 0usize, 0usize);
    merge_2g(&sta_buf, &ap, &mut new_buf, mode & MODE_G != 0, &mut i, &mut j, &mut k);
    merge_5g(&sta_buf, &ap, &mut new_buf, mode & MODE_A != 0, &mut i, &mut j, &mut k);
    unsafe {
        #[cfg(host_mlme_80211d_test)]
        {
            (*padapter).rfctl.channel_set = new_buf;
            (*padapter).mlmeextpriv.update_channel_plan_by_ap_done = 1;
        }
        #[cfg(not(host_mlme_80211d_test))]
        {
            rtw_rust_80211d_zero_chset(padapter);
            for n in 0..MAX_CH {
                rtw_rust_80211d_set_ch(padapter, n as U8, new_buf[n].num, new_buf[n].flags);
            }
            rtw_rust_80211d_set_update_done(padapter, 1);
            rtw_rust_80211d_reg_change(padapter);
        }
    }
}
