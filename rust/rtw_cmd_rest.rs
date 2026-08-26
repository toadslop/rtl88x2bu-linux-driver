// SPDX-License-Identifier: GPL-2.0
//! W3-60 cmd priv init/teardown — Rust port of `core/rtw_cmd_priv.c` (cmd slice).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

#[cfg(host_cmd_priv_test)]
use std::os::raw::{c_int, c_void};
#[cfg(not(host_cmd_priv_test))]
use core::ffi::{c_int, c_void};

type Sint = c_int;
const _SUCCESS: Sint = 1;
const _FAIL: Sint = 0;

#[repr(C)]
struct List { next: *mut List, prev: *mut List }
#[repr(C)]
struct Queue { queue: List, lock: c_int }

#[repr(C)]
pub struct CmdPriv {
    cmd_queue_sema: c_int,
    start_cmdthread_sema: c_int,
    cmd_queue: Queue,
    cmd_seq: u8,
    cmd_buf: *mut u8,
    cmd_allocated_buf: *mut u8,
    rsp_buf: *mut u8,
    rsp_allocated_buf: *mut u8,
    cmd_issued_cnt: u32,
    cmd_done_cnt: u32,
    rsp_cnt: u32,
    cmdthd_running: c_int,
    padapter: *mut c_void,
    sctx_mutex: c_int,
}

extern "C" {
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_init_listhead(list: *mut List);
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_spinlock_init(lock: *mut c_int);
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_spinlock_free(lock: *mut c_int);
    fn _rtw_init_sema(sema: *mut c_int, init_val: c_int);
    fn _rtw_free_sema(sema: *mut c_int);
    fn _rtw_mutex_init(mutex: *mut c_int);
    fn _rtw_mutex_free(mutex: *mut c_int);
    #[cfg(host_cmd_priv_test)]
    fn rtw_zmalloc(sz: u32) -> *mut u8;
    #[cfg(host_cmd_priv_test)]
    fn rtw_mfree(p: *mut u8, sz: u32);
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_zmalloc(sz: u32) -> *mut u8;
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_mfree(p: *mut u8, sz: u32);
}

#[inline]
fn zmalloc(sz: u32) -> *mut u8 {
    unsafe {
        #[cfg(host_cmd_priv_test)]
        { return rtw_zmalloc(sz); }
        #[cfg(not(host_cmd_priv_test))]
        { return _rtw_zmalloc(sz); }
    }
}

#[inline]
fn mfree(p: *mut u8, sz: u32) {
    if !p.is_null() {
        unsafe {
            #[cfg(host_cmd_priv_test)]
            { rtw_mfree(p, sz); }
            #[cfg(not(host_cmd_priv_test))]
            { _rtw_mfree(p, sz); }
        }
    }
}

#[inline]
fn init_queue(q: *mut Queue) {
    unsafe {
        #[cfg(host_cmd_priv_test)]
        {
            (*q).queue.next = &mut (*q).queue;
            (*q).queue.prev = &mut (*q).queue;
            (*q).lock = 0;
        }
        #[cfg(not(host_cmd_priv_test))]
        {
            _rtw_init_listhead(&mut (*q).queue);
            _rtw_spinlock_init(&mut (*q).lock);
        }
    }
}

#[inline]
fn aligned_buf(base: *mut u8, align: usize) -> *mut u8 {
    if base.is_null() {
        return core::ptr::null_mut();
    }
    let off = align - ((base as usize) & (align - 1));
    unsafe { base.add(off) }
}

#[no_mangle]
pub extern "C" fn _rtw_init_cmd_priv(p: *mut CmdPriv) -> Sint {
    if p.is_null() {
        return _FAIL;
    }
    unsafe {
        let c = &mut *p;
        _rtw_init_sema(&mut c.cmd_queue_sema, 0);
        _rtw_init_sema(&mut c.start_cmdthread_sema, 0);
        init_queue(&mut c.cmd_queue);
        c.cmd_seq = 1;
        c.cmd_allocated_buf = zmalloc(1536 + 512);
        if c.cmd_allocated_buf.is_null() {
            return _FAIL;
        }
        c.cmd_buf = aligned_buf(c.cmd_allocated_buf, 512);
        c.rsp_allocated_buf = zmalloc(512 + 4);
        if c.rsp_allocated_buf.is_null() {
            return _FAIL;
        }
        c.rsp_buf = aligned_buf(c.rsp_allocated_buf, 4);
        c.cmd_issued_cnt = 0;
        c.cmd_done_cnt = 0;
        c.rsp_cnt = 0;
        _rtw_mutex_init(&mut c.sctx_mutex);
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn _rtw_free_cmd_priv(p: *mut CmdPriv) {
    if p.is_null() {
        return;
    }
    unsafe {
        let c = &mut *p;
        #[cfg(not(host_cmd_priv_test))]
        { _rtw_spinlock_free(&mut c.cmd_queue.lock); }
        _rtw_free_sema(&mut c.cmd_queue_sema);
        _rtw_free_sema(&mut c.start_cmdthread_sema);
        mfree(c.cmd_allocated_buf, 1536 + 512);
        mfree(c.rsp_allocated_buf, 512 + 4);
        _rtw_mutex_free(&mut c.sctx_mutex);
    }
}
