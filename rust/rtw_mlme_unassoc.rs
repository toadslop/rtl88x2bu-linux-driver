// SPDX-License-Identifier: GPL-2.0
//! W3-62 unassoc STA queue helpers — host L2 oracle and kernel port.

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

#[cfg(host_mlme_unassoc_test)]
use std::os::raw::{c_int, c_schar, c_ulong, c_void};

#[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
use core::ffi::{c_int, c_schar, c_ulong, c_void};

type U8 = u8;
type U32 = u32;
type Systime = c_ulong;
type IrqL = c_ulong;
type S8 = c_schar;

const ETH_ALEN: usize = 6;
const UNASOC_STA_SRC_NUM: usize = 2;
const UNASOC_STA_DEL_CHK_SKIP: U8 = 0;
const UNASOC_STA_DEL_CHK_ALIVE: U8 = 1;
const UNASOC_STA_DEL_CHK_DELETED: U8 = 2;
const UNASOC_STA_MODE_INTERESTED: U8 = 1;
const UNASOC_STA_MODE_ALL: U8 = 2;
const UNASSOC_STA_LIFETIME_MS: c_int = 60000;
const _TRUE: c_int = 1;

#[repr(C)]
pub struct List {
    next: *mut List,
    prev: *mut List,
}

#[repr(C)]
pub struct Queue {
    queue: List,
    lock: c_int,
}

#[repr(C)]
pub struct UnassocStaInfo {
    list: List,
    addr: [U8; ETH_ALEN],
    interested: U8,
    recv_signal_power: S8,
    time: Systime,
}

#[repr(C)]
pub struct MlmePriv {
    unassoc_sta_mode_of_stype: [U8; UNASOC_STA_SRC_NUM],
    unassoc_sta_queue: Queue,
    free_unassoc_sta_queue: Queue,
    free_unassoc_sta_buf: *mut U8,
    interested_unassoc_sta_cnt: U32,
    max_unassoc_sta_cnt: U32,
}

#[repr(C)]
pub struct Adapter {
    mlmepriv: MlmePriv,
}

const _: () = assert!(core::mem::offset_of!(UnassocStaInfo, list) == 0);

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn rtw_run_in_thread_cmd(a: *mut Adapter, f: *mut c_void, c: *mut Adapter);
    fn rtw_hal_rcr_set_chk_bssid_act_non(a: *mut Adapter);
}

#[cfg(host_mlme_unassoc_test)]
extern "C" {
    fn rtw_get_current_time() -> Systime;
    fn rtw_ms_to_systime(ms: c_int) -> Systime;
    fn rtw_time_before(a: Systime, b: Systime) -> bool;
    fn rtw_time_after(a: Systime, b: Systime) -> bool;
}

#[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
extern "C" {
    fn _rtw_get_current_time() -> Systime;
    fn _rtw_ms_to_systime(ms: U32) -> Systime;
    fn _rtw_time_after(a: Systime, b: Systime) -> bool;
}

#[inline]
fn current_time() -> Systime {
    unsafe {
        #[cfg(host_mlme_unassoc_test)]
        {
            rtw_get_current_time()
        }
        #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
        {
            _rtw_get_current_time()
        }
    }
}

#[inline]
fn ms_to_systime(ms: c_int) -> Systime {
    unsafe {
        #[cfg(host_mlme_unassoc_test)]
        {
            rtw_ms_to_systime(ms)
        }
        #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
        {
            _rtw_ms_to_systime(ms as U32)
        }
    }
}

#[inline]
fn time_before(a: Systime, b: Systime) -> bool {
    unsafe {
        #[cfg(host_mlme_unassoc_test)]
        {
            rtw_time_before(a, b)
        }
        #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
        {
            _rtw_time_after(b, a)
        }
    }
}

#[inline]
fn time_after(a: Systime, b: Systime) -> bool {
    unsafe {
        #[cfg(host_mlme_unassoc_test)]
        {
            rtw_time_after(a, b)
        }
        #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
        {
            _rtw_time_after(a, b)
        }
    }
}

#[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_queue_enter_critical_bh(lock: *mut c_int, irql: *mut IrqL);
        pub fn rtw_rust_queue_exit_critical_bh(lock: *mut c_int, irql: *mut IrqL);
        pub fn rtw_rust_mlme_unassoc_adapter_mlme(a: *mut c_void) -> *mut MlmePriv;
        pub fn rtw_rust_mlme_unassoc_primary(a: *mut Adapter) -> *mut Adapter;
    }
}

#[inline]
pub fn memcmp_eq(a: *const U8, b: *const U8, n: usize) -> bool {
    unsafe { _rtw_memcmp(a as *const c_void, b as *const c_void, n) == _TRUE }
}

#[inline]
pub fn list_empty(h: &List) -> bool {
    h.next == h as *const List as *mut List
}

#[inline]
pub fn list_insert_tail(n: &mut List, head: &mut List) {
    unsafe {
        let prev = head.prev;
        n.next = head as *mut List;
        n.prev = prev;
        (*prev).next = n as *mut List;
        head.prev = n as *mut List;
    }
}

#[inline]
pub fn list_delete(e: &mut List) {
    unsafe {
        (*e.prev).next = e.next;
        (*e.next).prev = e.prev;
        e.next = e as *mut List;
        e.prev = e as *mut List;
    }
}

#[inline]
pub fn enter_bh(q: &mut Queue, irq: &mut IrqL) {
    #[cfg(host_mlme_unassoc_test)]
    let _ = (q, irq);
    #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
    unsafe {
        kernel::rtw_rust_queue_enter_critical_bh(&mut q.lock, irq);
    }
}

#[inline]
pub fn exit_bh(q: &mut Queue, irq: &mut IrqL) {
    #[cfg(host_mlme_unassoc_test)]
    let _ = (q, irq);
    #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
    unsafe {
        kernel::rtw_rust_queue_exit_critical_bh(&mut q.lock, irq);
    }
}

#[inline]
pub fn adapter_of(a: *mut Adapter) -> *mut Adapter {
    if a.is_null() {
        return core::ptr::null_mut();
    }
    #[cfg(host_mlme_unassoc_test)]
    return a;
    #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
    unsafe {
        kernel::rtw_rust_mlme_unassoc_primary(a)
    }
}

#[inline]
pub fn mlme_of(a: *mut Adapter) -> *mut MlmePriv {
    if a.is_null() {
        return core::ptr::null_mut();
    }
    #[cfg(host_mlme_unassoc_test)]
    unsafe {
        &mut (*a).mlmepriv
    }
    #[cfg(all(not(host_mlme_unassoc_test), rust_mlme_unassoc, config_rtw_multi_ap))]
    unsafe {
        kernel::rtw_rust_mlme_unassoc_adapter_mlme(a as *mut c_void)
    }
}

#[inline]
pub fn mlme_to_adapter(m: *mut MlmePriv) -> *mut Adapter {
    unsafe {
        (m as *mut u8).offset(-(core::mem::offset_of!(Adapter, mlmepriv) as isize)) as *mut Adapter
    }
}

pub fn del_unassoc_sta(mlmepriv: *mut MlmePriv, unassoc_sta: *mut UnassocStaInfo) {
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
        let mut irq = 0usize as IrqL;
        enter_bh(fq, &mut irq);
        list_delete(&mut sta.list);
        list_insert_tail(&mut sta.list, &mut fq.queue);
        exit_bh(fq, &mut irq);
    }
}

pub fn del_unassoc_sta_chk(mlmepriv: *mut MlmePriv, sta: *mut UnassocStaInfo) -> U8 {
    if sta.is_null() {
        return UNASOC_STA_DEL_CHK_SKIP;
    }
    unsafe {
        if (*sta).interested != 0 {
            return UNASOC_STA_DEL_CHK_SKIP;
        }
        let cur = current_time();
        let lifetime = (*sta)
            .time
            .wrapping_add(ms_to_systime(UNASSOC_STA_LIFETIME_MS));
        if time_before(cur, lifetime) {
            return UNASOC_STA_DEL_CHK_ALIVE;
        }
        del_unassoc_sta(mlmepriv, sta);
        UNASOC_STA_DEL_CHK_DELETED
    }
}

pub fn alloc_unassoc_sta(mlmepriv: *mut MlmePriv) -> *mut UnassocStaInfo {
    if mlmepriv.is_null() {
        return core::ptr::null_mut();
    }
    unsafe {
        let fq = &mut (*mlmepriv).free_unassoc_sta_queue;
        let mut irq = 0usize as IrqL;
        enter_bh(fq, &mut irq);
        if list_empty(&fq.queue) {
            exit_bh(fq, &mut irq);
            return core::ptr::null_mut();
        }
        let list = fq.queue.next;
        let sta = list as *mut UnassocStaInfo;
        list_delete(&mut (*sta).list);
        core::ptr::write_bytes((*sta).addr.as_mut_ptr(), 0, ETH_ALEN);
        (*sta).recv_signal_power = 0;
        (*sta).time = 0;
        (*sta).interested = 0;
        exit_bh(fq, &mut irq);
        sta
    }
}

#[no_mangle]
pub extern "C" fn rtw_del_unassoc_sta_queue(adapter: *mut Adapter) {
    let adapter = adapter_of(adapter);
    if adapter.is_null() {
        return;
    }
    unsafe {
        let mp = mlme_of(adapter);
        let q = &mut (*mp).unassoc_sta_queue;
        let mut irq = 0usize as IrqL;
        enter_bh(q, &mut irq);
        let head = &mut q.queue as *mut List;
        let mut list = (*head).next;
        while list != head {
            let sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            del_unassoc_sta(mp, sta);
        }
        exit_bh(q, &mut irq);
    }
}

#[no_mangle]
pub extern "C" fn rtw_del_unassoc_sta(adapter: *mut Adapter, addr: *mut U8) {
    let adapter = adapter_of(adapter);
    if adapter.is_null() || addr.is_null() {
        return;
    }
    unsafe {
        let mp = mlme_of(adapter);
        let q = &mut (*mp).unassoc_sta_queue;
        let mut irq = 0usize as IrqL;
        enter_bh(q, &mut irq);
        let head = &mut q.queue as *mut List;
        let mut list = (*head).next;
        while list != head {
            let sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            if memcmp_eq(addr, (*sta).addr.as_ptr(), ETH_ALEN) {
                del_unassoc_sta(mp, sta);
                break;
            }
        }
        exit_bh(q, &mut irq);
    }
}

#[no_mangle]
pub extern "C" fn rtw_search_unassoc_sta(
    adapter: *mut Adapter,
    addr: *mut U8,
    ret_sta: *mut UnassocStaInfo,
) -> U8 {
    let adapter = adapter_of(adapter);
    if adapter.is_null() || addr.is_null() || ret_sta.is_null() {
        return 0;
    }
    unsafe {
        let q = &mut (*mlme_of(adapter)).unassoc_sta_queue;
        let mut irq = 0usize as IrqL;
        enter_bh(q, &mut irq);
        let head = &mut q.queue as *mut List;
        let mut list = (*head).next;
        while list != head {
            let sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            if memcmp_eq(addr, (*sta).addr.as_ptr(), ETH_ALEN) {
                core::ptr::copy_nonoverlapping(sta, ret_sta, 1);
                exit_bh(q, &mut irq);
                return 1;
            }
        }
        exit_bh(q, &mut irq);
        0
    }
}

#[path = "rtw_mlme_unassoc_rx.rs"]
mod rx;
