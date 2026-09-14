// SPDX-License-Identifier: GPL-2.0
//! W3-79 `_rtw_init_sta_priv` host L2 oracle (see `core/rtw_sta_mgt_free.c`).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use std::os::raw::c_int;

const _SUCCESS: u32 = 1;
const _FAIL: u32 = 0;
const NUM_STA: usize = 4;
const RTW_ACL_PERIOD_NUM: usize = 2;
const MEM_ALIGNMENT_OFFSET: usize = core::mem::size_of::<usize>();
const MEM_ALIGNMENT_PADDING: usize = MEM_ALIGNMENT_OFFSET - 1;
const STA_INFO_SIZE: usize = 976;
const MACID_CTL_NUM_OFF: usize = 1160;
const SP_PADAPTER: usize = 832;
const SP_PALLOC: usize = 920;
const SP_PBUF: usize = 928;
const SP_FREE_Q: usize = 936;
const SP_HASH_LOCK: usize = 1008;
const SP_STA_HASH: usize = 1016;
const SP_ASOC_CNT: usize = 1080;
const SP_ADHOC_EXP: usize = 1084;
const SP_MAX_AID: usize = 848;
const SP_RR_AID: usize = 852;
const SP_STARTED_AID: usize = 850;
const SP_MAX_NUM_STA: usize = 854;
const SP_STA_AID: usize = 840;
const SP_AID_BMP_LEN: usize = 1086;
const SP_DZ_BMP: usize = 1088;
const SP_TIM_BMP: usize = 1096;
const SP_ASOC_LIST: usize = 1104;
const SP_AUTH_LIST: usize = 1120;
const SP_ASOC_LCK: usize = 1136;
const SP_AUTH_LCK: usize = 1140;
const SP_ASOC_LCNT: usize = 1144;
const SP_AUTH_LCNT: usize = 1148;
const SP_AUTH_TO: usize = 1152;
const SP_ASSOC_TO: usize = 1153;
const SP_EXPIRE_TO: usize = 1154;

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
    fn _rtw_init_stainfo(psta: *mut u8);
    fn rtw_zvmalloc(sz: u32) -> *mut u8;
    fn rtw_vmfree(p: *mut u8, sz: u32);
    fn rtw_zmalloc(sz: u32) -> *mut u8;
    fn rtw_mfree(p: *mut u8, sz: u32);
    fn rtw_macaddr_acl_init(adapter: *mut u8, index: c_int);
    fn rtw_pre_link_sta_ctl_init(stapriv: *mut u8);
    fn rtw_set_rx_chk_limit(adapter: *mut u8, limit: c_int);
}

fn aid_bmp_len(aid: u16) -> u16 {
    (aid + 7) / 8
}
unsafe fn field_mut<T>(base: *mut u8, off: usize) -> *mut T {
    base.add(off).cast()
}
unsafe fn init_listhead(list: *mut List) {
    (*list).next = list;
    (*list).prev = list;
}
unsafe fn init_queue(queue: *mut Queue) {
    init_listhead(core::ptr::addr_of_mut!((*queue).queue));
    (*queue).lock = 0;
}
unsafe fn list_insert_tail(entry: *mut List, head: *mut List) {
    let prev = (*head).prev;
    (*entry).next = head;
    (*entry).prev = prev;
    (*prev).next = entry;
    (*head).prev = entry;
}
unsafe fn init_fail(stapriv: *mut u8, max_aid: u16) {
    let sp = stapriv;
    let palloc = *field_mut::<*mut u8>(sp, SP_PALLOC);
    if !palloc.is_null() {
        rtw_vmfree(
            palloc,
            (STA_INFO_SIZE * NUM_STA + MEM_ALIGNMENT_OFFSET) as u32,
        );
        *field_mut::<*mut u8>(sp, SP_PALLOC) = core::ptr::null_mut();
    }
    let sta_aid = *field_mut::<*mut u8>(sp, SP_STA_AID);
    if !sta_aid.is_null() {
        rtw_mfree(
            sta_aid,
            max_aid as u32 * core::mem::size_of::<*mut u8>() as u32,
        );
        *field_mut::<*mut u8>(sp, SP_STA_AID) = core::ptr::null_mut();
    }
    let dz = *field_mut::<*mut u8>(sp, SP_DZ_BMP);
    if !dz.is_null() {
        rtw_mfree(dz, *field_mut::<u16>(sp, SP_AID_BMP_LEN) as u32);
        *field_mut::<*mut u8>(sp, SP_DZ_BMP) = core::ptr::null_mut();
    }
}

#[no_mangle]
pub extern "C" fn _rtw_init_sta_priv(stapriv: *mut u8) -> u32 {
    unsafe {
        if stapriv.is_null() {
            return _FAIL;
        }
        let sp = stapriv;
        let adapter = sp;
        *field_mut::<*mut u8>(sp, SP_PADAPTER) = adapter;
        let max_aid = *adapter.add(MACID_CTL_NUM_OFF) as u16;
        let palloc = rtw_zvmalloc((STA_INFO_SIZE * NUM_STA + MEM_ALIGNMENT_OFFSET) as u32);
        if palloc.is_null() {
            return _FAIL;
        }
        *field_mut::<*mut u8>(sp, SP_PALLOC) = palloc;
        let mut pbuf = palloc;
        let buf_ptr = pbuf as usize;
        if buf_ptr & MEM_ALIGNMENT_PADDING != 0 {
            pbuf = (buf_ptr + MEM_ALIGNMENT_OFFSET - (buf_ptr & MEM_ALIGNMENT_PADDING)) as *mut u8;
        }
        *field_mut::<*mut u8>(sp, SP_PBUF) = pbuf;
        init_queue(field_mut(sp, SP_FREE_Q));
        *field_mut::<c_int>(sp, SP_HASH_LOCK) = 0;
        *field_mut::<c_int>(sp, SP_ASOC_CNT) = 0;
        init_queue(field_mut(sp, 960));
        init_queue(field_mut(sp, 984));
        let mut psta = pbuf;
        for i in 0..NUM_STA {
            _rtw_init_stainfo(psta);
            init_listhead(field_mut(
                sp,
                SP_STA_HASH + i * core::mem::size_of::<List>(),
            ));
            let fq = field_mut::<Queue>(sp, SP_FREE_Q);
            list_insert_tail(field_mut(psta, 16), core::ptr::addr_of_mut!((*fq).queue));
            psta = psta.add(STA_INFO_SIZE);
        }
        *field_mut::<u8>(sp, SP_ADHOC_EXP) = 4;
        *field_mut::<u16>(sp, SP_MAX_AID) = max_aid;
        *field_mut::<u8>(sp, SP_RR_AID) = 0;
        *field_mut::<u16>(sp, SP_STARTED_AID) = 1;
        let sta_aid = rtw_zmalloc(max_aid as u32 * core::mem::size_of::<*mut u8>() as u32);
        if sta_aid.is_null() {
            init_fail(stapriv, max_aid);
            return _FAIL;
        }
        *field_mut::<*mut u8>(sp, SP_STA_AID) = sta_aid;
        let bmp_len = aid_bmp_len(max_aid);
        *field_mut::<u16>(sp, SP_AID_BMP_LEN) = bmp_len;
        let dz = rtw_zmalloc(bmp_len as u32);
        if dz.is_null() {
            init_fail(stapriv, max_aid);
            return _FAIL;
        }
        *field_mut::<*mut u8>(sp, SP_DZ_BMP) = dz;
        let tim = rtw_zmalloc(bmp_len as u32);
        if tim.is_null() {
            init_fail(stapriv, max_aid);
            return _FAIL;
        }
        *field_mut::<*mut u8>(sp, SP_TIM_BMP) = tim;
        init_listhead(field_mut(sp, SP_ASOC_LIST));
        init_listhead(field_mut(sp, SP_AUTH_LIST));
        *field_mut::<c_int>(sp, SP_ASOC_LCK) = 0;
        *field_mut::<c_int>(sp, SP_AUTH_LCK) = 0;
        *field_mut::<c_int>(sp, SP_ASOC_LCNT) = 0;
        *field_mut::<c_int>(sp, SP_AUTH_LCNT) = 0;
        *field_mut::<u8>(sp, SP_AUTH_TO) = 3;
        *field_mut::<u8>(sp, SP_ASSOC_TO) = 3;
        *field_mut::<u16>(sp, SP_EXPIRE_TO) = 60;
        *field_mut::<u16>(sp, SP_MAX_NUM_STA) = NUM_STA as u16;
        for i in 0..RTW_ACL_PERIOD_NUM as c_int {
            rtw_macaddr_acl_init(adapter, i);
        }
        rtw_pre_link_sta_ctl_init(stapriv);
        rtw_set_rx_chk_limit(adapter, 8);
        _SUCCESS
    }
}
