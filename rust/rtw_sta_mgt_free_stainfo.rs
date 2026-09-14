// SPDX-License-Identifier: GPL-2.0
//! W3-79 `rtw_free_stainfo` host L2 oracle (see `tests/host/sta_mgt/host_sta_mgt_free_shim.c`).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use std::os::raw::{c_int, c_uint};

const _SUCCESS: u32 = 1;
const WIFI_ASOC_STATE: c_uint = 0x0000_0001;
const WIFI_AP_STATE: c_uint = 0x0000_0010;

const STA_STATE_OFF: usize = 8;
const STA_LOCK_OFF: usize = 12;
const STA_LIST_OFF: usize = 16;
const STA_HASH_LIST_OFF: usize = 32;
const STA_ST_CTL_OFF: usize = 424;

const SP_FREE_Q_OFF: usize = 936;
const SP_HASH_LOCK_OFF: usize = 1008;
const SP_ASOC_CNT_OFF: usize = 1080;
const ADAPTER_MLMEPRIV_OFF: usize = 1164;

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

extern "C" {
    fn _enter_critical_bh(plock: *mut c_int, pirql: *mut c_int);
    fn _exit_critical_bh(plock: *mut c_int, pirql: *mut c_int);
    fn rtw_mi_update_iface_status(pmlmepriv: *mut u8, flags: u8);
    fn rtw_free_stainfo_flush_xmit(padapter: *mut u8, psta: *mut u8);
    fn rtw_free_stainfo_flush_recv(padapter: *mut u8, psta: *mut u8);
    fn rtw_hal_set_odm_var(padapter: *mut u8, var: c_int, psta: *mut u8, val: u8);
    fn rtw_release_macid(padapter: *mut u8, psta: *mut u8);
    fn rtw_st_ctl_deinit(st_ctl: *mut u8);
}

unsafe fn field_mut<T>(base: *mut u8, off: usize) -> *mut T {
    base.add(off).cast()
}

unsafe fn list_delete(entry: *mut List) {
    let next = (*entry).next;
    let prev = (*entry).prev;
    (*prev).next = next;
    (*next).prev = prev;
    (*entry).next = entry;
    (*entry).prev = entry;
}

unsafe fn list_insert_tail(n: *mut List, head: *mut List) {
    let prev = (*head).prev;
    (*n).next = head;
    (*n).prev = prev;
    (*prev).next = n;
    (*head).prev = n;
}

fn mac_is_bcst(addr: *const u8) -> bool {
    unsafe {
        if addr.is_null() {
            return false;
        }
        let slice = core::slice::from_raw_parts(addr, 6);
        slice.iter().all(|&b| b == 0xff)
    }
}

#[no_mangle]
pub extern "C" fn rtw_free_stainfo(padapter: *mut u8, psta: *mut u8) -> u32 {
    unsafe {
        if psta.is_null() {
            return _SUCCESS;
        }
        let stapriv = padapter;
        let mut irq: c_int = 0;

        _enter_critical_bh(
            field_mut(stapriv, SP_HASH_LOCK_OFF),
            core::ptr::addr_of_mut!(irq),
        );
        list_delete(field_mut(psta, STA_HASH_LIST_OFF));
        *field_mut::<c_int>(stapriv, SP_ASOC_CNT_OFF) -= 1;
        _exit_critical_bh(
            field_mut(stapriv, SP_HASH_LOCK_OFF),
            core::ptr::addr_of_mut!(irq),
        );
        rtw_mi_update_iface_status(padapter.add(ADAPTER_MLMEPRIV_OFF), 0);

        _enter_critical_bh(field_mut(psta, STA_LOCK_OFF), core::ptr::addr_of_mut!(irq));
        *field_mut::<c_uint>(psta, STA_STATE_OFF) &= !WIFI_ASOC_STATE;
        _exit_critical_bh(field_mut(psta, STA_LOCK_OFF), core::ptr::addr_of_mut!(irq));

        rtw_free_stainfo_flush_xmit(padapter, psta);
        rtw_free_stainfo_flush_recv(padapter, psta);

        let state = *field_mut::<c_uint>(psta, STA_STATE_OFF);
        let mac = psta.add(2);
        if (state & WIFI_AP_STATE) == 0 && !mac_is_bcst(mac) {
            rtw_hal_set_odm_var(padapter, 0, psta, 0);
        }

        rtw_release_macid(padapter, psta);
        rtw_st_ctl_deinit(psta.add(STA_ST_CTL_OFF));

        /* host _rtw_spinlock_free is a no-op */
        _enter_critical_bh(
            field_mut(stapriv, SP_HASH_LOCK_OFF),
            core::ptr::addr_of_mut!(irq),
        );
        let fq = field_mut::<Queue>(stapriv, SP_FREE_Q_OFF);
        list_insert_tail(
            field_mut(psta, STA_LIST_OFF),
            core::ptr::addr_of_mut!((*fq).queue),
        );
        _exit_critical_bh(
            field_mut(stapriv, SP_HASH_LOCK_OFF),
            core::ptr::addr_of_mut!(irq),
        );

        _SUCCESS
    }
}
