// SPDX-License-Identifier: GPL-2.0
//! W3-82 `rtw_ap_expire_auth_list` — Rust port of `core/rtw_ap_expire_auth.c`.

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

#[cfg(host_ap_expire_auth_test)]
use core::mem::offset_of;

type U8 = u8;
type Adapter = c_void;
type StaInfo = c_void;
type StaPriv = c_void;
type IrqL = c_void;
type List = c_void;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const NUM_STA: usize = 32;

#[cfg(host_ap_expire_auth_test)]
const HOST_EXPIRE_AUTH_MAX_STA: usize = 2;

#[cfg(host_ap_expire_auth_test)]
#[repr(C)]
struct ListHost {
    next: *mut ListHost,
    prev: *mut ListHost,
}

#[cfg(host_ap_expire_auth_test)]
#[repr(C)]
struct StaInfoHost {
    expire_to: U8,
    auth_list: ListHost,
}

#[cfg(host_ap_expire_auth_test)]
#[repr(C)]
struct StaPrivHost {
    auth_list: ListHost,
    auth_list_lock: i32,
    sta_pool: [StaInfoHost; HOST_EXPIRE_AUTH_MAX_STA],
    sta_count: U8,
}

#[cfg(host_ap_expire_auth_test)]
#[repr(C)]
struct AdapterHost {
    stapriv: StaPrivHost,
}

#[cfg(host_ap_expire_auth_test)]
extern "C" {
    fn host_expire_auth_reset_flush_count();
    fn rtw_stainfo_offset(pstapriv: *mut StaPrivHost, psta: *mut StaInfoHost) -> i32;
    fn stainfo_offset_valid(offset: i32) -> U8;
    fn rtw_get_stainfo_by_offset(pstapriv: *mut StaPrivHost, offset: i32) -> *mut StaInfoHost;
    fn rtw_free_stainfo(padapter: *mut AdapterHost, psta: *mut StaInfoHost);
}

#[cfg(not(host_ap_expire_auth_test))]
extern "C" {
    fn rtw_stainfo_offset(pstapriv: *mut StaPriv, psta: *mut StaInfo) -> i32;
    fn rtw_get_stainfo_by_offset(pstapriv: *mut StaPriv, offset: i32) -> *mut StaInfo;
    fn rtw_free_stainfo(padapter: *mut Adapter, psta: *mut StaInfo);
    fn rtw_rust_expire_auth_lock(pstapriv: *mut StaPriv, irq: *mut IrqL);
    fn rtw_rust_expire_auth_unlock(pstapriv: *mut StaPriv, irq: *mut IrqL);
    fn rtw_rust_expire_auth_auth_head(pstapriv: *mut StaPriv) -> *mut List;
    fn rtw_rust_expire_auth_list_next(list: *mut List) -> *mut List;
    fn rtw_rust_expire_auth_queue_end(head: *mut List, elem: *mut List) -> U8;
    fn rtw_rust_expire_auth_sta_from_list(list: *mut List) -> *mut StaInfo;
    fn rtw_rust_expire_auth_sta_expire_to(psta: *mut StaInfo) -> U8;
    fn rtw_rust_expire_auth_sta_dec_expire_to(psta: *mut StaInfo);
    fn rtw_rust_expire_auth_stapriv(padapter: *mut Adapter) -> *mut StaPriv;
    fn rtw_rust_expire_auth_stainfo_offset_valid(offset: i32) -> U8;
    fn rtw_rust_expire_auth_warn_invalid_offset();
}

#[cfg(host_ap_expire_auth_test)]
fn sta_from_auth_list(plist: *mut ListHost) -> *mut StaInfoHost {
    unsafe { (plist as *mut u8).sub(offset_of!(StaInfoHost, auth_list)) as *mut StaInfoHost }
}

#[cfg(host_ap_expire_auth_test)]
fn expire_auth_list_host(padapter: *mut AdapterHost) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        host_expire_auth_reset_flush_count();
        let pstapriv = &mut (*padapter).stapriv;
        let phead = &mut pstapriv.auth_list as *mut ListHost;
        let mut plist = (*phead).next;
        let mut flush_list = [0i8; HOST_EXPIRE_AUTH_MAX_STA];
        let mut flush_num = 0usize;

        while plist != phead {
            let psta = sta_from_auth_list(plist);
            plist = (*plist).next;
            if (*psta).expire_to > 0 {
                (*psta).expire_to -= 1;
                if (*psta).expire_to == 0 {
                    let stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
                    if stainfo_offset_valid(stainfo_offset) != 0 {
                        flush_list[flush_num] = stainfo_offset as i8;
                        flush_num += 1;
                    }
                }
            }
        }
        for i in 0..flush_num {
            let psta = rtw_get_stainfo_by_offset(pstapriv, flush_list[i] as i32);
            rtw_free_stainfo(padapter, psta);
        }
    }
}

#[cfg(not(host_ap_expire_auth_test))]
fn expire_auth_list_kernel(padapter: *mut Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let pstapriv = rtw_rust_expire_auth_stapriv(padapter);
        let mut irq: u64 = 0;
        let irq_ptr = &mut irq as *mut u64 as *mut IrqL;
        rtw_rust_expire_auth_lock(pstapriv, irq_ptr);
        let phead = rtw_rust_expire_auth_auth_head(pstapriv);
        let mut plist = rtw_rust_expire_auth_list_next(phead);
        let mut flush_list = [0i8; NUM_STA];
        let mut flush_num = 0usize;

        while rtw_rust_expire_auth_queue_end(phead, plist) == _FALSE {
            let psta = rtw_rust_expire_auth_sta_from_list(plist);
            plist = rtw_rust_expire_auth_list_next(plist);
            if rtw_rust_expire_auth_sta_expire_to(psta) > 0 {
                rtw_rust_expire_auth_sta_dec_expire_to(psta);
                if rtw_rust_expire_auth_sta_expire_to(psta) == 0 {
                    let stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
                    if rtw_rust_expire_auth_stainfo_offset_valid(stainfo_offset) != 0 {
                        flush_list[flush_num] = stainfo_offset as i8;
                        flush_num += 1;
                    } else {
                        rtw_rust_expire_auth_warn_invalid_offset();
                    }
                }
            }
        }
        rtw_rust_expire_auth_unlock(pstapriv, irq_ptr);
        for i in 0..flush_num {
            let psta = rtw_get_stainfo_by_offset(pstapriv, flush_list[i] as i32);
            rtw_free_stainfo(padapter, psta);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_ap_expire_auth_list(padapter: *mut Adapter) {
    #[cfg(host_ap_expire_auth_test)]
    expire_auth_list_host(padapter as *mut AdapterHost);
    #[cfg(not(host_ap_expire_auth_test))]
    expire_auth_list_kernel(padapter);
}
