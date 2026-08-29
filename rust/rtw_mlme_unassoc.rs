// SPDX-License-Identifier: GPL-2.0
//! W3-62 unassoc STA del/search queue helpers — host L2 oracle (PR1).

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    unused_imports
)]

use std::os::raw::{c_int, c_schar, c_ulong, c_void};

type U8 = u8;
type U32 = u32;
type Systime = c_ulong;
type IrqL = c_ulong;

const ETH_ALEN: usize = 6;
const _TRUE: c_int = 1;

#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}

#[repr(C)]
struct Queue {
    queue: List,
    lock: c_int,
}

#[repr(C)]
struct UnassocStaInfo {
    list: List,
    addr: [U8; ETH_ALEN],
    interested: U8,
    recv_signal_power: i8,
    time: Systime,
}

#[repr(C)]
struct MlmePriv {
    unassoc_sta_mode_of_stype: [U8; 2],
    unassoc_sta_queue: Queue,
    free_unassoc_sta_queue: Queue,
    free_unassoc_sta_buf: *mut U8,
    interested_unassoc_sta_cnt: U32,
    max_unassoc_sta_cnt: U32,
}

#[repr(C)]
struct Adapter {
    mlmepriv: MlmePriv,
}

const _: () = assert!(core::mem::offset_of!(UnassocStaInfo, list) == 0);

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn rtw_run_in_thread_cmd(a: *mut Adapter, f: *mut c_void, c: *mut Adapter);
    fn rtw_hal_rcr_set_chk_bssid_act_non(a: *mut Adapter);
}

#[inline]
fn memcmp_eq(a: *const U8, b: *const U8, n: usize) -> bool {
    unsafe { _rtw_memcmp(a as *const c_void, b as *const c_void, n) == _TRUE }
}

#[inline]
fn list_insert_tail(n: &mut List, head: &mut List) {
    unsafe {
        let prev = head.prev;
        n.next = head as *mut List;
        n.prev = prev;
        (*prev).next = n as *mut List;
        head.prev = n as *mut List;
    }
}

#[inline]
fn list_delete(e: &mut List) {
    unsafe {
        (*e.prev).next = e.next;
        (*e.next).prev = e.prev;
        e.next = e as *mut List;
        e.prev = e as *mut List;
    }
}

#[inline]
fn mlme_to_adapter(m: *mut MlmePriv) -> *mut Adapter {
    unsafe {
        (m as *mut u8).offset(-(core::mem::offset_of!(Adapter, mlmepriv) as isize)) as *mut Adapter
    }
}

fn del_unassoc_sta(mlmepriv: *mut MlmePriv, unassoc_sta: *mut UnassocStaInfo) {
    if mlmepriv.is_null() || unassoc_sta.is_null() {
        return;
    }
    unsafe {
        let sta = &mut *unassoc_sta;
        let mp = &mut *mlmepriv;
        let fq = &mut mp.free_unassoc_sta_queue;
        if sta.interested != 0 {
            mp.interested_unassoc_sta_cnt = mp.interested_unassoc_sta_cnt.saturating_sub(1);
            if mp.interested_unassoc_sta_cnt == 0 {
                rtw_run_in_thread_cmd(
                    mlme_to_adapter(mlmepriv),
                    rtw_hal_rcr_set_chk_bssid_act_non as *mut c_void,
                    mlme_to_adapter(mlmepriv),
                );
            }
        }
        list_delete(&mut sta.list);
        list_insert_tail(&mut sta.list, &mut fq.queue);
    }
}

#[no_mangle]
pub extern "C" fn rtw_del_unassoc_sta_queue(adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let mlmepriv = &mut (*adapter).mlmepriv;
        let head = &mut mlmepriv.unassoc_sta_queue.queue as *mut List;
        let mut list = (*head).next;
        while list != head {
            let unassoc_sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            del_unassoc_sta(mlmepriv, unassoc_sta);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_del_unassoc_sta(adapter: *mut Adapter, addr: *mut U8) {
    if adapter.is_null() || addr.is_null() {
        return;
    }
    unsafe {
        let mlmepriv = &mut (*adapter).mlmepriv;
        let head = &mut mlmepriv.unassoc_sta_queue.queue as *mut List;
        let mut list = (*head).next;
        while list != head {
            let unassoc_sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            if memcmp_eq(addr, (*unassoc_sta).addr.as_ptr(), ETH_ALEN) {
                del_unassoc_sta(mlmepriv, unassoc_sta);
                break;
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_search_unassoc_sta(
    adapter: *mut Adapter,
    addr: *mut U8,
    ret_sta: *mut UnassocStaInfo,
) -> U8 {
    if adapter.is_null() || addr.is_null() || ret_sta.is_null() {
        return 0;
    }
    unsafe {
        let head = &mut (*adapter).mlmepriv.unassoc_sta_queue.queue as *mut List;
        let mut list = (*head).next;
        while list != head {
            let unassoc_sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            if memcmp_eq(addr, (*unassoc_sta).addr.as_ptr(), ETH_ALEN) {
                core::ptr::copy_nonoverlapping(unassoc_sta, ret_sta, 1);
                return 1;
            }
        }
        0
    }
}
