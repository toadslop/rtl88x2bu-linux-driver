// SPDX-License-Identifier: GPL-2.0
//! W3-63 WMM IE restructure — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    unused_imports,
    unreachable_pub
)]

#[cfg(host_mlme_wmm_rsn_test)]
use std::os::raw::{c_int, c_void};

#[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
use core::ffi::{c_int, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;

const _TRUE: c_int = 1;
const ETH_ALEN: usize = 6;
const NUM_PMKID_CACHE: usize = 16;

#[repr(C)]
struct RsneInfo {
    gcs: *mut U8,
    pcs_cnt: U16,
    pcs_list: *mut U8,
    akm_cnt: U16,
    akm_list: *mut U8,
    cap: *mut U8,
    pmkid_cnt: U16,
    pmkid_list: *mut U8,
    gmcs: *mut U8,
    err: U8,
}

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn _rtw_memcpy(d: *mut c_void, s: *const c_void, n: usize) -> *mut c_void;
    fn rtw_rsne_info_parse(ie: *const U8, ie_len: U32, info: *mut RsneInfo) -> c_int;
}

#[cfg(host_mlme_wmm_rsn_test)]
mod host {
    use super::*;

    #[repr(C)]
    pub struct RtkPmkidList {
        pub b_used: U8,
        pub bssid: [U8; ETH_ALEN],
        pub pmkid: [U8; 16],
    }

    #[repr(C)]
    pub struct QosPriv {
        pub uapsd_max_sp_len: U8,
        pub uapsd_tid: U16,
    }

    #[repr(C)]
    pub struct SecurityPriv {
        pub pmkid_list: [RtkPmkidList; NUM_PMKID_CACHE],
    }

    #[repr(C)]
    pub struct MlmePriv {
        pub qospriv: QosPriv,
    }

    #[repr(C)]
    pub struct Adapter {
        pub mlmepriv: MlmePriv,
        pub securitypriv: SecurityPriv,
        pub scratch: [U8; 256],
    }
}

#[cfg(host_mlme_wmm_rsn_test)]
use host::Adapter;

#[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
type Adapter = c_void;

#[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
extern "C" {
    fn rtw_mlme_wmm_rsn_qos(a: *mut Adapter) -> *mut U8;
    fn rtw_mlme_wmm_rsn_pmkid(a: *mut Adapter, i: c_int) -> *mut c_void;
}

#[inline]
fn memcmp_eq(a: *const U8, b: *const U8, n: usize) -> bool {
    unsafe { _rtw_memcmp(a as *const c_void, b as *const c_void, n) == _TRUE }
}

fn memcpy_bytes(dst: *mut U8, src: *const U8, n: usize) {
    unsafe {
        _rtw_memcpy(dst as *mut c_void, src as *const c_void, n);
    }
}

fn put_le16(p: *mut U8, v: U16) {
    unsafe {
        *p = (v & 0xff) as U8;
        *p.add(1) = ((v >> 8) & 0xff) as U8;
    }
}

#[no_mangle]
pub extern "C" fn rtw_restruct_wmm_ie(
    adapter: *mut Adapter,
    in_ie: *mut U8,
    out_ie: *mut U8,
    in_len: U32,
    initial_out_len: U32,
) -> U32 {
    if adapter.is_null() || in_ie.is_null() || out_ie.is_null() {
        return initial_out_len;
    }
    let mut ielength = initial_out_len;
    let mut i = 12u32;
    let mut qos_info = 0u8;
    while i < in_len {
        ielength = initial_out_len;
        unsafe {
            if *in_ie.add(i as usize) == 0xDD
                && *in_ie.add(i as usize + 2) == 0x00
                && *in_ie.add(i as usize + 3) == 0x50
                && *in_ie.add(i as usize + 4) == 0xF2
                && *in_ie.add(i as usize + 5) == 0x02
                && i + 5 < in_len
            {
                for j in i..i + 9 {
                    *out_ie.add(ielength as usize) = *in_ie.add(j as usize);
                    ielength += 1;
                }
                *out_ie.add(initial_out_len as usize + 1) = 0x07;
                *out_ie.add(initial_out_len as usize + 6) = 0x00;
                let (max_sp, tid) = qos_fields(adapter);
                match max_sp {
                    1 => qos_info |= 1 << 5,
                    2 => qos_info |= 1 << 6,
                    3 => {
                        qos_info |= 1 << 5;
                        qos_info |= 1 << 6;
                    }
                    _ => {}
                }
                if (tid & 0x80 != 0) && (tid & 0x40 != 0) {
                    qos_info |= 1;
                }
                if (tid & 0x20 != 0) && (tid & 0x10 != 0) {
                    qos_info |= 2;
                }
                if (tid & 0x04 != 0) && (tid & 0x02 != 0) {
                    qos_info |= 4;
                }
                if (tid & 0x08 != 0) && (tid & 0x01 != 0) {
                    qos_info |= 8;
                }
                *out_ie.add(initial_out_len as usize + 8) = qos_info;
                break;
            }
            i += (*in_ie.add(i as usize + 1) as u32) + 2;
        }
    }
    ielength
}

fn qos_fields(adapter: *mut Adapter) -> (U8, U16) {
    unsafe {
        #[cfg(host_mlme_wmm_rsn_test)]
        {
            let pq = &(*adapter).mlmepriv.qospriv;
            (pq.uapsd_max_sp_len, pq.uapsd_tid)
        }
        #[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
        {
            let q = rtw_mlme_wmm_rsn_qos(adapter);
            (*q, *(q.add(2) as *const U16))
        }
    }
}

fn pmkid_entry(adapter: *mut Adapter, idx: usize) -> (U8, *const U8) {
    unsafe {
        #[cfg(host_mlme_wmm_rsn_test)]
        {
            let ent = &(*adapter).securitypriv.pmkid_list[idx];
            (ent.b_used, ent.bssid.as_ptr())
        }
        #[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
        {
            let ent = rtw_mlme_wmm_rsn_pmkid(adapter, idx as c_int) as *mut U8;
            (ent.read(), ent.add(1))
        }
    }
}

fn pmkid_copy(adapter: *mut Adapter, idx: usize, dst: *mut U8) {
    unsafe {
        #[cfg(host_mlme_wmm_rsn_test)]
        {
            memcpy_bytes(
                dst,
                (*adapter).securitypriv.pmkid_list[idx].pmkid.as_ptr(),
                16,
            );
        }
        #[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
        {
            let ent = rtw_mlme_wmm_rsn_pmkid(adapter, idx as c_int) as *mut U8;
            memcpy_bytes(dst, ent.add(1 + ETH_ALEN), 16);
        }
    }
}

fn sec_is_in_pmkid_list(adapter: *mut Adapter, bssid: *mut U8) -> c_int {
    if adapter.is_null() || bssid.is_null() {
        return -1;
    }
    for i in 0..NUM_PMKID_CACHE {
        let (used, ent_bssid) = pmkid_entry(adapter, i);
        if used != 0 && memcmp_eq(ent_bssid, bssid, ETH_ALEN) {
            return i as c_int;
        }
    }
    -1
}

#[no_mangle]
pub extern "C" fn rtw_cached_pmkid(adapter: *mut Adapter, bssid: *mut U8) -> c_int {
    sec_is_in_pmkid_list(adapter, bssid)
}

#[no_mangle]
pub extern "C" fn rtw_rsn_sync_pmkid(
    adapter: *mut Adapter,
    ie: *mut U8,
    mut ie_len: U32,
    i_ent: c_int,
) -> U32 {
    if adapter.is_null() || ie.is_null() {
        return 0;
    }
    let mut info = RsneInfo {
        gcs: core::ptr::null_mut(),
        pcs_cnt: 0,
        pcs_list: core::ptr::null_mut(),
        akm_cnt: 0,
        akm_list: core::ptr::null_mut(),
        cap: core::ptr::null_mut(),
        pmkid_cnt: 0,
        pmkid_list: core::ptr::null_mut(),
        gmcs: core::ptr::null_mut(),
        err: 0,
    };
    let mut gm_cs = [0u8; 4];
    unsafe {
        if rtw_rsne_info_parse(ie, ie_len, &mut info) != _TRUE || info.err != 0 {
            return 0;
        }
        if i_ent < 0 && info.pmkid_cnt == 0 {
            return ie_len;
        }
        if info.pmkid_list.is_null() {
            return ie_len;
        }
        if i_ent >= 0 && info.pmkid_cnt == 1 {
            let mut cur = [0u8; 16];
            pmkid_copy(adapter, i_ent as usize, cur.as_mut_ptr());
            if memcmp_eq(info.pmkid_list, cur.as_ptr(), 16) {
                return ie_len;
            }
        }
        if !info.gmcs.is_null() {
            memcpy_bytes(gm_cs.as_mut_ptr(), info.gmcs, 4);
        }
        if i_ent >= 0 {
            info.pmkid_cnt = 1;
            pmkid_copy(adapter, i_ent as usize, info.pmkid_list);
        } else {
            info.pmkid_cnt = 0;
        }
        put_le16(info.pmkid_list.sub(2), info.pmkid_cnt);
        if !info.gmcs.is_null() {
            memcpy_bytes(
                info.pmkid_list.add(16 * info.pmkid_cnt as usize),
                gm_cs.as_ptr(),
                4,
            );
        }
        ie_len = 1 + 1 + 2 + 4 + 2 + 4 * info.pcs_cnt as u32 + 2 + 4 * info.akm_cnt as u32 + 2
            + 2
            + 16 * info.pmkid_cnt as u32
            + if !info.gmcs.is_null() { 4 } else { 0 };
        *ie.add(1) = (ie_len - 2) as U8;
    }
    ie_len
}
