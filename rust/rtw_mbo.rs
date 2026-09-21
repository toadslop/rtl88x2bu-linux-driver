// SPDX-License-Identifier: GPL-2.0
#![allow(dead_code, non_snake_case, non_upper_case_globals)]

use std::os::raw::{c_int, c_uint};

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const VENDOR_IE: u8 = 221;
static WFA_OUI: [u8; 4] = [0x50, 0x6F, 0x9A, 0x16];

#[repr(C)]
pub struct NprefCh {
    pub op_class: u8,
    pub chs: [u8; 64],
    pub nm_of_ch: usize,
    pub preference: u8,
    pub reason: u8,
}
#[repr(C)]
pub struct NprefChRtp { pub ch_rpt: [NprefCh; 32], pub nm_of_rpt: usize }
#[repr(C)]
pub struct RfCtl { pub ch_rtp: NprefChRtp }
#[repr(C)]
pub struct PktAttrib { pub pktlen: u32 }
#[repr(C)]
pub struct WlanBssidEx {
    pub mac_address: [u8; 6],
    pub ie_length: u32,
    pub ies: [u8; 768],
}
#[repr(C)]
pub struct WlanNetwork { pub network: WlanBssidEx }
#[repr(C)]
pub struct Adapter { pub rf_ctl: RfCtl }

#[no_mangle]
pub extern "C" fn rtw_get_ie(pbuf: *const u8, index: c_int, len: *mut c_int, limit: c_int) -> *mut u8 {
    if limit < 1 || pbuf.is_null() || len.is_null() {
        return std::ptr::null_mut();
    }
    unsafe {
        let mut p = pbuf;
        let mut i = 0;
        *len = 0;
        loop {
            if *p == index as u8 {
                *len = *p.add(1) as c_int;
                return p as *mut u8;
            }
            let tmp = *p.add(1) as c_int;
            p = p.add((tmp + 2) as usize);
            i += tmp + 2;
            if i >= limit {
                break;
            }
        }
    }
    std::ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn rtw_set_fixed_ie(pbuf: *mut u8, len: c_uint, source: *mut u8, frlen: *mut c_uint) -> *mut u8 {
    if pbuf.is_null() || source.is_null() || frlen.is_null() {
        return std::ptr::null_mut();
    }
    unsafe {
        std::ptr::copy_nonoverlapping(source, pbuf, len as usize);
        *frlen += len;
        pbuf.add(len as usize)
    }
}

#[no_mangle]
pub extern "C" fn host_mbo_ie_get(pie: *mut u8, plen: *mut u32, limit: u32) -> *mut u8 {
    if pie.is_null() || plen.is_null() || limit <= 1 {
        return std::ptr::null_mut();
    }
    unsafe {
        let mut p = pie;
        let mut i = 0u32;
        *plen = 0;
        loop {
            if *p == VENDOR_IE && std::slice::from_raw_parts(p.add(2), 4) == WFA_OUI.as_slice() {
                *plen = *p.add(1) as u32;
                return p;
            }
            let tmp = *p.add(1) as u32;
            p = p.add((tmp + 2) as usize);
            i += tmp + 2;
            if i >= limit {
                break;
            }
        }
    }
    std::ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn host_mbo_attrs_get(pie: *mut u8, limit: u32, attr_id: u8, attr_len: *mut u32) -> *mut u8 {
    if pie.is_null() || attr_len.is_null() || limit <= 1 {
        return std::ptr::null_mut();
    }
    unsafe {
        let mut plen = 0u32;
        let mut p = host_mbo_ie_get(pie, &mut plen, limit);
        if p.is_null() {
            return std::ptr::null_mut();
        }
        p = p.add(6);
        plen -= 4;
        let mut alen = 0i32;
        p = rtw_get_ie(p, attr_id as c_int, &mut alen, plen as c_int);
        if p.is_null() {
            return std::ptr::null_mut();
        }
        *attr_len = alen as u32;
        p
    }
}

#[no_mangle]
pub extern "C" fn host_mbo_attr_sz_get(padapter: *mut Adapter, id: u8) -> u32 {
    if padapter.is_null() {
        return 0;
    }
    unsafe {
        let prpt = &(*padapter).rf_ctl.ch_rtp;
        match id {
            0x2 => {
                let mut len = 0u32;
                for i in 0..prpt.nm_of_rpt {
                    len += (prpt.ch_rpt[i].nm_of_ch as u32 + 3) + 2;
                }
                len
            }
            0x3 | 0x7 => 3,
            _ => 0,
        }
    }
}

#[no_mangle]
pub extern "C" fn host_mbo_build_mbo_ie_hdr(pframe: *mut *mut u8, pattrib: *mut PktAttrib, payload_len: u8) {
    if pframe.is_null() || pattrib.is_null() {
        return;
    }
    unsafe {
        let mut frame = *pframe;
        let mut eid = VENDOR_IE;
        let mut len = payload_len.saturating_add(4);
        frame = rtw_set_fixed_ie(frame, 1, &mut eid, &mut (*pattrib).pktlen);
        frame = rtw_set_fixed_ie(frame, 1, &mut len, &mut (*pattrib).pktlen);
        let mut oui = WFA_OUI;
        frame = rtw_set_fixed_ie(frame, 4, oui.as_mut_ptr(), &mut (*pattrib).pktlen);
        *pframe = frame;
    }
}

#[no_mangle]
pub extern "C" fn host_mbo_disallowed_network(pnetwork: *mut WlanNetwork) -> u8 {
    if pnetwork.is_null() {
        return _FALSE;
    }
    unsafe {
        let net = &mut *pnetwork;
        let mut alen = 0u32;
        let p = host_mbo_attrs_get(net.network.ies.as_mut_ptr(), net.network.ie_length, 0x4, &mut alen);
        if p.is_null() { _FALSE } else { _TRUE }
    }
}

#[no_mangle]
pub extern "C" fn host_mbo_non_pref_chan_exist(pch: *mut NprefCh, ch: u8) -> u8 {
    if pch.is_null() {
        return _FALSE;
    }
    unsafe {
        let p = &*pch;
        for i in 0..p.nm_of_ch {
            if p.chs[i] == ch {
                return _TRUE;
            }
        }
    }
    _FALSE
}

#[no_mangle]
pub extern "C" fn host_mbo_adapter_clear(a: *mut Adapter) {
    if !a.is_null() {
        unsafe { std::ptr::write_bytes(a, 0, 1) };
    }
}

#[no_mangle]
pub extern "C" fn host_mbo_seed_npref(
    a: *mut Adapter,
    rpt_idx: u8,
    op_class: u8,
    ch_count: u8,
    chs: *const u8,
    preference: u8,
    reason: u8,
) {
    if a.is_null() || chs.is_null() || rpt_idx as usize >= 32 {
        return;
    }
    unsafe {
        let prpt = &mut (*a).rf_ctl.ch_rtp;
        let pch = &mut prpt.ch_rpt[rpt_idx as usize];
        pch.op_class = op_class;
        pch.preference = preference;
        pch.reason = reason;
        pch.nm_of_ch = ch_count as usize;
        for i in 0..ch_count as usize {
            pch.chs[i] = *chs.add(i);
        }
        if prpt.nm_of_rpt <= rpt_idx as usize {
            prpt.nm_of_rpt = rpt_idx as usize + 1;
        }
    }
}
