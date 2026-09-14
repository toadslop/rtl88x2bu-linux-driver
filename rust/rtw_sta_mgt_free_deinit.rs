// SPDX-License-Identifier: GPL-2.0
//! W3-79 `_rtw_free_sta_priv` host L2 oracle (see `core/rtw_sta_mgt_free.c`).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use std::os::raw::{c_int, c_ulong};

const _SUCCESS: u32 = 1;
const _FALSE: u32 = 0;
const NUM_STA: usize = 4;
const RTW_ACL_PERIOD_NUM: usize = 2;
const STA_INFO_SIZE: usize = 976;
const STA_INFO_HASH_LIST_OFF: usize = 32;
const STA_INFO_RECVREORDER_OFF: usize = 464;
const RECV_REORDER_CTRL_SIZE: usize = 32;
const REORDER_TIMER_OFF: usize = 0;
const MEM_ALIGNMENT_OFFSET: usize = core::mem::size_of::<usize>();
const SP_PADAPTER: usize = 832;
const SP_PALLOC: usize = 920;
const SP_STA_AID: usize = 840;
const SP_MAX_AID: usize = 848;
const SP_HASH_LOCK: usize = 1008;
const SP_STA_HASH: usize = 1016;
const SP_AID_BMP_LEN: usize = 1086;
const SP_DZ_BMP: usize = 1088;
const SP_TIM_BMP: usize = 1096;

#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}

extern "C" {
    fn _enter_critical_bh(plock: *mut c_int, pirql: *mut c_ulong);
    fn _exit_critical_bh(plock: *mut c_int, pirql: *mut c_ulong);
    fn _cancel_timer_ex(timer: *mut u8);
    fn rtw_mfree_sta_priv_lock(stapriv: *mut u8);
    fn rtw_macaddr_acl_deinit(adapter: *mut u8, index: c_int);
    fn rtw_pre_link_sta_ctl_deinit(stapriv: *mut u8);
    fn rtw_vmfree(p: *mut u8, sz: u32);
    fn rtw_mfree(p: *mut u8, sz: u32);
}

unsafe fn field_mut<T>(base: *mut u8, off: usize) -> *mut T {
    base.add(off).cast()
}

unsafe fn list_containor_sta(plist: *mut List) -> *mut u8 {
    plist.cast::<u8>().sub(STA_INFO_HASH_LIST_OFF)
}

#[no_mangle]
pub extern "C" fn _rtw_free_sta_priv(stapriv: *mut u8) -> u32 {
    unsafe {
        if stapriv.is_null() {
            return _SUCCESS;
        }
        let sp = stapriv;
        let mut irql: c_ulong = 0;
        let hash_lock = field_mut::<c_int>(sp, SP_HASH_LOCK);
        _enter_critical_bh(hash_lock, &mut irql);
        for index in 0..NUM_STA {
            let phead = field_mut::<List>(sp, SP_STA_HASH + index * core::mem::size_of::<List>());
            let mut plist = (*phead).next;
            while plist != phead {
                let psta = list_containor_sta(plist);
                plist = (*plist).next;
                for i in 0..16usize {
                    let ctrl = psta.add(STA_INFO_RECVREORDER_OFF + i * RECV_REORDER_CTRL_SIZE);
                    _cancel_timer_ex(ctrl.add(REORDER_TIMER_OFF));
                }
            }
        }
        _exit_critical_bh(hash_lock, &mut irql);

        rtw_mfree_sta_priv_lock(sp);

        let adapter = *field_mut::<*mut u8>(sp, SP_PADAPTER);
        for index in 0..RTW_ACL_PERIOD_NUM as c_int {
            rtw_macaddr_acl_deinit(adapter, index);
        }
        rtw_pre_link_sta_ctl_deinit(sp);

        let palloc = *field_mut::<*mut u8>(sp, SP_PALLOC);
        if !palloc.is_null() {
            rtw_vmfree(
                palloc,
                (STA_INFO_SIZE * NUM_STA + MEM_ALIGNMENT_OFFSET) as u32,
            );
        }
        let max_aid = *field_mut::<u16>(sp, SP_MAX_AID);
        let sta_aid = *field_mut::<*mut u8>(sp, SP_STA_AID);
        if !sta_aid.is_null() {
            rtw_mfree(
                sta_aid,
                max_aid as u32 * core::mem::size_of::<*mut u8>() as u32,
            );
        }
        let bmp_len = *field_mut::<u16>(sp, SP_AID_BMP_LEN);
        let dz = *field_mut::<*mut u8>(sp, SP_DZ_BMP);
        if !dz.is_null() {
            rtw_mfree(dz, bmp_len as u32);
        }
        let tim = *field_mut::<*mut u8>(sp, SP_TIM_BMP);
        if !tim.is_null() {
            rtw_mfree(tim, bmp_len as u32);
        }

        _SUCCESS
    }
}
