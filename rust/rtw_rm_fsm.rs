// SPDX-License-Identifier: GPL-2.0
//! RM FSM clock/list helpers — Rust port part 1 (W3-112 PR3).

#![allow(non_camel_case_types, non_snake_case, improper_ctypes, non_upper_case_globals)]
#![cfg(any(host_rm_fsm_test, rtw_80211k))]

#[cfg(host_rm_fsm_test)]
use std::ffi::c_int;
#[cfg(not(host_rm_fsm_test))]
use core::ffi::c_int;

const RM_TIMER_NUM: usize = 32;
const CLOCK_UNIT: u32 = 10;
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
pub struct Adapter {
    pub rmpriv: RmPriv,
}

fn null_mut_rmobj() -> *mut RmObj {
    #[cfg(host_rm_fsm_test)]
    return std::ptr::null_mut();
    #[cfg(not(host_rm_fsm_test))]
    return core::ptr::null_mut();
}

#[no_mangle]
pub extern "C" fn is_list_linked(head: *const ListHead) -> c_int {
    unsafe { if (*head).prev.is_null() { 0 } else { 1 } }
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
