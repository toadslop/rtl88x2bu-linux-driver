// SPDX-License-Identifier: GPL-2.0
//! RM FSM obj/queue/clock helpers — Rust port (W3-112).

#![allow(
    non_camel_case_types,
    non_snake_case,
    improper_ctypes,
    non_upper_case_globals
)]
#![cfg(any(host_rm_fsm_test, rtw_80211k))]

#[cfg(not(host_rm_fsm_test))]
use core::ffi::c_int;
#[cfg(host_rm_fsm_test)]
use std::ffi::c_int;

const _SUCCESS: c_int = 1;
const _FAIL: c_int = 0;
const RM_TIMER_NUM: usize = 32;
const CLOCK_UNIT: u32 = 10;
const RM_ST_IDLE: u8 = 0;
const RM_EV_max: i32 = 13;

#[repr(C)]
pub struct ListHead {
    pub next: *mut ListHead,
    pub prev: *mut ListHead,
}

#[repr(C)]
pub struct Queue {
    pub queue: ListHead,
    pub lock: c_int,
}

#[repr(C)]
pub struct RmClock {
    pub prm: *mut RmObj,
    pub counter: c_int,
    pub evid: c_int,
}

#[repr(C)]
pub struct BcnReqOpt {
    pub req_start: *mut u8,
    pub req_len: u8,
}

#[repr(C)]
pub struct RmMeasReq {
    pub pssid: *mut u8,
    pub opt: BcnReqOptWrap,
}

#[repr(C)]
pub struct BcnReqOptWrap {
    pub bcn: BcnReqOpt,
}

#[repr(C)]
pub struct RmObj {
    pub rmid: u32,
    pub state: u8,
    pub _pad: [u8; 3],
    pub q: RmMeasReq,
    pub pclock: *mut RmClock,
    pub list: ListHead,
}

#[repr(C)]
pub struct RmPriv {
    pub ev_queue: Queue,
    pub rm_queue: Queue,
    pub clock: [RmClock; RM_TIMER_NUM],
}

#[repr(C)]
pub struct RmEvent {
    pub rmid: u32,
    pub evid: c_int,
    pub list: ListHead,
}

#[repr(C)]
pub struct Adapter {
    pub rmpriv: RmPriv,
}

#[cfg(host_rm_fsm_test)]
use std::alloc::{alloc, dealloc, Layout};

extern "C" {
    fn strlen(s: *const u8) -> usize;
}

fn rm_state_initial(prm: *mut RmObj) {
    unsafe {
        (*prm).state = RM_ST_IDLE;
    }
}

unsafe fn list_insert_head(node: *mut ListHead, head: *mut ListHead) {
    (*node).next = (*head).next;
    (*node).prev = head;
    (*(*head).next).prev = node;
    (*head).next = node;
}

unsafe fn list_insert_tail(node: *mut ListHead, head: *mut ListHead) {
    (*node).next = head;
    (*node).prev = (*head).prev;
    (*(*head).prev).next = node;
    (*head).prev = node;
}

unsafe fn list_delete(e: *mut ListHead) {
    (*(*e).next).prev = (*e).prev;
    (*(*e).prev).next = (*e).next;
    (*e).next = e;
    (*e).prev = e;
}

fn size_of_rmobj() -> usize {
    #[cfg(host_rm_fsm_test)]
    return std::mem::size_of::<RmObj>();
    #[cfg(not(host_rm_fsm_test))]
    return core::mem::size_of::<RmObj>();
}

fn null_mut_rmobj() -> *mut RmObj {
    #[cfg(host_rm_fsm_test)]
    return std::ptr::null_mut();
    #[cfg(not(host_rm_fsm_test))]
    return core::ptr::null_mut();
}

#[no_mangle]
pub extern "C" fn is_list_linked(head: *const ListHead) -> c_int {
    unsafe {
        if (*head).prev.is_null() {
            0
        } else {
            1
        }
    }
}

#[no_mangle]
pub extern "C" fn rm_set_clock(prm: *mut RmObj, ms: u32, evid: c_int) {
    unsafe {
        let clk = &mut *(*prm).pclock;
        clk.counter = (ms / CLOCK_UNIT) as c_int;
        clk.evid = evid;
    }
}

#[no_mangle]
pub extern "C" fn rm_alloc_clock(padapter: *mut Adapter, prm: *mut RmObj) -> *mut RmClock {
    unsafe {
        let clocks = &mut (*padapter).rmpriv.clock;
        for i in 0..RM_TIMER_NUM {
            let clk = &mut clocks[i];
            if clk.prm.is_null() {
                clk.prm = prm;
                clk.counter = 0;
                clk.evid = RM_EV_max;
                return clk;
            }
        }
        &mut clocks[RM_TIMER_NUM - 1]
    }
}

#[no_mangle]
pub extern "C" fn rm_cancel_clock(prm: *mut RmObj) {
    unsafe {
        let clk = &mut *(*prm).pclock;
        clk.counter = 0;
        clk.evid = RM_EV_max;
    }
}

#[no_mangle]
pub extern "C" fn rm_free_clock(pclock: *mut RmClock) {
    unsafe {
        let clk = &mut *pclock;
        clk.prm = null_mut_rmobj();
        clk.counter = 0;
        clk.evid = RM_EV_max;
    }
}

#[no_mangle]
pub extern "C" fn rm_enqueue_ev(queue: *mut Queue, obj: *mut RmEvent, to_head: bool) -> c_int {
    if obj.is_null() {
        return _FAIL;
    }
    unsafe {
        if to_head {
            list_insert_head(&mut (*obj).list, &mut (*queue).queue);
        } else {
            list_insert_tail(&mut (*obj).list, &mut (*queue).queue);
        }
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rm_free_rmobj(prm: *mut RmObj) {
    if prm.is_null() {
        return;
    }
    unsafe {
        if is_list_linked(&(*prm).list) != 0 {
            list_delete(&mut (*prm).list);
        }
        if !(*prm).q.pssid.is_null() {
            let n = strlen((*prm).q.pssid) + 1;
            dealloc((*prm).q.pssid, Layout::from_size_align_unchecked(n, 1));
        }
        if !(*prm).q.opt.bcn.req_start.is_null() {
            dealloc(
                (*prm).q.opt.bcn.req_start,
                Layout::from_size_align_unchecked((*prm).q.opt.bcn.req_len as usize, 1),
            );
        }
        if !(*prm).pclock.is_null() {
            rm_free_clock((*prm).pclock);
        }
        dealloc(
            prm as *mut u8,
            Layout::from_size_align_unchecked(size_of_rmobj(), 1),
        );
    }
}

#[no_mangle]
pub extern "C" fn rm_alloc_rmobj(padapter: *mut Adapter) -> *mut RmObj {
    unsafe {
        let layout = Layout::from_size_align_unchecked(size_of_rmobj(), 1);
        let prm = alloc(layout) as *mut RmObj;
        if prm.is_null() {
            return null_mut_rmobj();
        }
        core::ptr::write_bytes(prm, 0, 1);
        (*prm).pclock = rm_alloc_clock(padapter, prm);
        if (*prm).pclock.is_null() {
            rm_free_rmobj(prm);
            return null_mut_rmobj();
        }
        prm
    }
}

#[no_mangle]
pub extern "C" fn rm_enqueue_rmobj(
    padapter: *mut Adapter,
    prm: *mut RmObj,
    to_head: bool,
) -> c_int {
    if prm.is_null() {
        return _FAIL;
    }
    unsafe {
        let queue = &mut (*padapter).rmpriv.rm_queue;
        if to_head {
            list_insert_head(&mut (*prm).list, &mut queue.queue);
        } else {
            list_insert_tail(&mut (*prm).list, &mut queue.queue);
        }
        rm_state_initial(prm);
    }
    _SUCCESS
}
