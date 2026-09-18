// SPDX-License-Identifier: GPL-2.0
//! W3-75 beacon add/remove IE — Rust port (PR5); TIM + kernel swap in PR6.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type Sint = i32;
type Uint = u32;
type NetPtr = *mut c_void;
type AdapterPtr = *mut c_void;

#[cfg(host_ap_bcn_ie_test)]
#[repr(C)]
struct WlanBssidEx {
    length: U32,
    mac_address: [U8; 6],
    reserved: [U8; 2],
    ssid: [U8; 32],
    mesh_id: [U8; 32],
    privacy: U32,
    rssi: Sint,
    configuration: [U8; 16],
    infrastructure_mode: U32,
    supported_rates: [U8; 16],
    phy_info: [U8; 4],
    ie_length: U32,
    ies: [U8; 256],
}

#[repr(C)]
struct Ndis80211VariableIes {
    element_id: U8,
    length: U8,
}

extern "C" {
    fn rtw_get_ie(pbuf: *const U8, index: Sint, len: *mut Sint, limit: Sint) -> *mut U8;
    fn rtw_malloc(sz: usize) -> *mut c_void;
    fn rtw_mfree(p: *mut c_void, sz: usize);
}

fn net_ies(net: NetPtr) -> *mut U8 {
    unsafe { (&mut *(net as *mut WlanBssidEx)).ies.as_mut_ptr() }
}

fn net_ie_len(net: NetPtr) -> *mut U32 {
    unsafe { &mut (*(net as *mut WlanBssidEx)).ie_length }
}

fn splice_tail(
    pie: *mut U8,
    net: NetPtr,
    dst_ie: *mut U8,
    premainder_ie: *mut U8,
    remainder_ielen: Uint,
    insert: &[U8],
) {
    unsafe {
        let mut pbackup: *mut U8 = core::ptr::null_mut();
        if remainder_ielen > 0 {
            pbackup = rtw_malloc(remainder_ielen as usize) as *mut U8;
            if !pbackup.is_null() {
                core::ptr::copy_nonoverlapping(premainder_ie, pbackup, remainder_ielen as usize);
            }
        }
        let tail = if insert.is_empty() {
            dst_ie
        } else {
            core::ptr::copy_nonoverlapping(insert.as_ptr(), dst_ie, insert.len());
            dst_ie.add(insert.len())
        };
        if !pbackup.is_null() {
            core::ptr::copy_nonoverlapping(pbackup, tail, remainder_ielen as usize);
            rtw_mfree(pbackup as *mut c_void, remainder_ielen as usize);
        }
        *net_ie_len(net) = tail.offset_from(pie) as Uint + remainder_ielen;
    }
}

#[no_mangle]
pub extern "C" fn rtw_add_bcn_ie(
    _adapter: AdapterPtr,
    pnetwork: NetPtr,
    index: U8,
    data: *mut U8,
    len: U8,
) {
    if pnetwork.is_null() || data.is_null() {
        return;
    }
    unsafe {
        let pie = net_ies(pnetwork);
        let ie_length = *net_ie_len(pnetwork);
        let mut i = 12usize;
        let mut bmatch = false;
        let mut p: *mut U8 = core::ptr::null_mut();
        let mut ielen: Uint = 0;

        while i < ie_length as usize {
            let p_ie = pie.add(i) as *mut Ndis80211VariableIes;
            if (*p_ie).element_id > index {
                break;
            } else if (*p_ie).element_id == index {
                p = p_ie as *mut U8;
                ielen = (*p_ie).length as Uint;
                bmatch = true;
                break;
            }
            p = p_ie as *mut U8;
            ielen = (*p_ie).length as Uint;
            i += (*p_ie).length as usize + 2;
        }

        let mut dst_ie: *mut U8 = core::ptr::null_mut();
        let mut remainder_ielen: Uint = 0;
        let mut premainder_ie: *mut U8 = core::ptr::null_mut();

        if !p.is_null() && ielen > 0 {
            ielen += 2;
            premainder_ie = p.add(ielen as usize);
            remainder_ielen = ie_length - (p.offset_from(pie) as Uint) - ielen;
            dst_ie = if bmatch { p } else { p.add(ielen as usize) };
        }

        if dst_ie.is_null() {
            return;
        }

        let mut insert = [0u8; 258];
        insert[0] = index;
        insert[1] = len;
        core::ptr::copy_nonoverlapping(data, insert.as_mut_ptr().add(2), len as usize);
        splice_tail(
            pie,
            pnetwork,
            dst_ie,
            premainder_ie,
            remainder_ielen,
            &insert[..2 + len as usize],
        );
    }
}

#[no_mangle]
pub extern "C" fn rtw_remove_bcn_ie(_adapter: AdapterPtr, pnetwork: NetPtr, index: U8) {
    if pnetwork.is_null() {
        return;
    }
    unsafe {
        let pie = net_ies(pnetwork);
        let ie_length = *net_ie_len(pnetwork);
        let mut ielen: Sint = 0;
        let p = rtw_get_ie(pie.add(12), index as Sint, &mut ielen, ie_length as Sint - 12);
        if p.is_null() || ielen <= 0 {
            return;
        }
        let ielen_u = (ielen + 2) as Uint;
        splice_tail(
            pie,
            pnetwork,
            p,
            p.add(ielen_u as usize),
            ie_length - (p.offset_from(pie) as Uint) - ielen_u,
            &[],
        );
    }
}
