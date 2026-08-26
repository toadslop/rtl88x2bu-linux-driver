// SPDX-License-Identifier: GPL-2.0
//! W3-60 cmd/evt priv init and teardown — Rust port of `core/rtw_cmd_priv.c`.

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

#[cfg(not(host_cmd_priv_test))]
use core::ffi::{c_int, c_void};
#[cfg(host_cmd_priv_test)]
use std::os::raw::{c_int, c_void};

type Sint = c_int;
const _SUCCESS: Sint = 1;
const _FAIL: Sint = 0;

const MAX_CMDSZ: u32 = 1536;
const CMDBUFF_ALIGN_SZ: usize = 512;
const MAX_RSPSZ: u32 = 512;

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
struct WorkItem {
    s: c_int,
}
#[repr(C)]
struct RtwCbuf {
    size: u32,
    write: u32,
    read: u32,
    bufs: *mut *mut c_void,
}

#[repr(C)]
pub struct EvtPriv {
    #[cfg(any(host_cmd_priv_test, event_thread_mode))]
    evt_notify: c_int,
    #[cfg(any(host_cmd_priv_test, event_thread_mode))]
    evt_queue: Queue,
    #[cfg(any(host_cmd_priv_test, c2h_wk))]
    c2h_wk: WorkItem,
    #[cfg(any(host_cmd_priv_test, c2h_wk))]
    c2h_wk_alive: bool,
    #[cfg(any(host_cmd_priv_test, c2h_wk))]
    c2h_queue: *mut RtwCbuf,
    event_seq: c_int,
    evt_buf: *mut u8,
    evt_allocated_buf: *mut u8,
    evt_done_cnt: u32,
}

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

#[cfg(not(host_cmd_priv_test))]
mod kernel_layout {
    use super::c_void;

    extern "C" {
        pub fn rtw_rust_cmd_priv_cmd_queue_sema(p: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_cmd_priv_start_cmdthread_sema(p: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_cmd_priv_cmd_seq(p: *mut c_void) -> *mut u8;
        pub fn rtw_rust_cmd_priv_cmd_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_cmd_allocated_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_rsp_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_rsp_allocated_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_cmd_issued_cnt(p: *mut c_void) -> *mut u32;
        pub fn rtw_rust_cmd_priv_cmd_done_cnt(p: *mut c_void) -> *mut u32;
        pub fn rtw_rust_cmd_priv_rsp_cnt(p: *mut c_void) -> *mut u32;
        pub fn rtw_rust_cmd_priv_sctx_mutex(p: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_cmd_priv_init_queue(p: *mut c_void);
        pub fn rtw_rust_cmd_priv_spinlock_free(p: *mut c_void);
    }
}

extern "C" {
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_init_listhead(list: *mut List);
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_spinlock_init(lock: *mut c_int);
    fn _rtw_init_sema(sema: *mut c_int, init_val: c_int);
    fn _rtw_free_sema(sema: *mut c_int);
    fn _rtw_mutex_init(mutex: *mut c_int);
    fn _rtw_mutex_free(mutex: *mut c_int);
    fn _init_workitem(wk: *mut WorkItem, func: *mut c_void, cntx: *mut c_void);
    fn _cancel_workitem_sync(wk: *mut WorkItem);
    fn rtw_msleep_os(ms: c_int);
    fn c2h_wk_callback(wk: *mut WorkItem);
    fn rtw_cbuf_alloc(size: u32) -> *mut RtwCbuf;
    fn rtw_cbuf_free(cbuf: *mut RtwCbuf);
    fn rtw_cbuf_empty(cbuf: *mut RtwCbuf) -> bool;
    fn rtw_cbuf_pop(cbuf: *mut RtwCbuf) -> *mut c_void;
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
        {
            return rtw_zmalloc(sz);
        }
        #[cfg(not(host_cmd_priv_test))]
        {
            return _rtw_zmalloc(sz);
        }
    }
}

#[inline]
fn mfree(p: *mut u8, sz: u32) {
    if !p.is_null() {
        unsafe {
            #[cfg(host_cmd_priv_test)]
            {
                rtw_mfree(p, sz);
            }
            #[cfg(not(host_cmd_priv_test))]
            {
                _rtw_mfree(p, sz);
            }
        }
    }
}

#[inline]
fn init_queue_host(q: *mut Queue) {
    unsafe {
        (*q).queue.next = &mut (*q).queue;
        (*q).queue.prev = &mut (*q).queue;
        (*q).lock = 0;
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
pub extern "C" fn _rtw_init_cmd_priv(p: *mut c_void) -> Sint {
    if p.is_null() {
        return _FAIL;
    }
    unsafe {
        #[cfg(host_cmd_priv_test)]
        {
            let c = &mut *(p as *mut CmdPriv);
            _rtw_init_sema(&mut c.cmd_queue_sema, 0);
            _rtw_init_sema(&mut c.start_cmdthread_sema, 0);
            init_queue_host(&mut c.cmd_queue);
            c.cmd_seq = 1;
            c.cmd_allocated_buf = zmalloc(MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32);
            if c.cmd_allocated_buf.is_null() {
                return _FAIL;
            }
            c.cmd_buf = aligned_buf(c.cmd_allocated_buf, CMDBUFF_ALIGN_SZ);
            c.rsp_allocated_buf = zmalloc(MAX_RSPSZ + 4);
            if c.rsp_allocated_buf.is_null() {
                return _FAIL;
            }
            c.rsp_buf = aligned_buf(c.rsp_allocated_buf, 4);
            c.cmd_issued_cnt = 0;
            c.cmd_done_cnt = 0;
            c.rsp_cnt = 0;
            _rtw_mutex_init(&mut c.sctx_mutex);
        }
        #[cfg(not(host_cmd_priv_test))]
        {
            use kernel_layout::*;
            _rtw_init_sema(rtw_rust_cmd_priv_cmd_queue_sema(p).cast(), 0);
            _rtw_init_sema(rtw_rust_cmd_priv_start_cmdthread_sema(p).cast(), 0);
            rtw_rust_cmd_priv_init_queue(p);
            *rtw_rust_cmd_priv_cmd_seq(p) = 1;
            *rtw_rust_cmd_priv_cmd_allocated_buf(p) = zmalloc(MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32);
            if (*rtw_rust_cmd_priv_cmd_allocated_buf(p)).is_null() {
                return _FAIL;
            }
            *rtw_rust_cmd_priv_cmd_buf(p) =
                aligned_buf(*rtw_rust_cmd_priv_cmd_allocated_buf(p), CMDBUFF_ALIGN_SZ);
            *rtw_rust_cmd_priv_rsp_allocated_buf(p) = zmalloc(MAX_RSPSZ + 4);
            if (*rtw_rust_cmd_priv_rsp_allocated_buf(p)).is_null() {
                return _FAIL;
            }
            *rtw_rust_cmd_priv_rsp_buf(p) = aligned_buf(*rtw_rust_cmd_priv_rsp_allocated_buf(p), 4);
            *rtw_rust_cmd_priv_cmd_issued_cnt(p) = 0;
            *rtw_rust_cmd_priv_cmd_done_cnt(p) = 0;
            *rtw_rust_cmd_priv_rsp_cnt(p) = 0;
            _rtw_mutex_init(rtw_rust_cmd_priv_sctx_mutex(p).cast());
        }
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn _rtw_free_cmd_priv(p: *mut c_void) {
    if p.is_null() {
        return;
    }
    unsafe {
        #[cfg(host_cmd_priv_test)]
        {
            let c = &mut *(p as *mut CmdPriv);
            _rtw_free_sema(&mut c.cmd_queue_sema);
            _rtw_free_sema(&mut c.start_cmdthread_sema);
            mfree(c.cmd_allocated_buf, MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32);
            mfree(c.rsp_allocated_buf, MAX_RSPSZ + 4);
            _rtw_mutex_free(&mut c.sctx_mutex);
        }
        #[cfg(not(host_cmd_priv_test))]
        {
            use kernel_layout::*;
            rtw_rust_cmd_priv_spinlock_free(p);
            _rtw_free_sema(rtw_rust_cmd_priv_cmd_queue_sema(p).cast());
            _rtw_free_sema(rtw_rust_cmd_priv_start_cmdthread_sema(p).cast());
            mfree(
                *rtw_rust_cmd_priv_cmd_allocated_buf(p),
                MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32,
            );
            mfree(*rtw_rust_cmd_priv_rsp_allocated_buf(p), MAX_RSPSZ + 4);
            _rtw_mutex_free(rtw_rust_cmd_priv_sctx_mutex(p).cast());
        }
    }
}

#[no_mangle]
pub extern "C" fn _rtw_init_evt_priv(p: *mut EvtPriv) -> Sint {
    if p.is_null() {
        return _FAIL;
    }
    unsafe {
        let e = &mut *p;
        e.event_seq = 0;
        e.evt_done_cnt = 0;
        #[cfg(any(host_cmd_priv_test, event_thread_mode))]
        {
            _rtw_init_sema(&mut e.evt_notify, 0);
            e.evt_allocated_buf = zmalloc(1024 + 4);
            if e.evt_allocated_buf.is_null() {
                return _FAIL;
            }
            e.evt_buf = aligned_buf(e.evt_allocated_buf, 4);
            init_queue(&mut e.evt_queue);
        }
        #[cfg(any(host_cmd_priv_test, c2h_wk))]
        {
            _init_workitem(
                &mut e.c2h_wk,
                c2h_wk_callback as *mut c_void,
                core::ptr::null_mut(),
            );
            e.c2h_wk_alive = false;
            e.c2h_queue = rtw_cbuf_alloc(11);
        }
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn _rtw_free_evt_priv(p: *mut EvtPriv) {
    if p.is_null() {
        return;
    }
    unsafe {
        let e = &mut *p;
        #[cfg(any(host_cmd_priv_test, event_thread_mode))]
        {
            _rtw_free_sema(&mut e.evt_notify);
            mfree(e.evt_allocated_buf, 1024 + 4);
        }
        #[cfg(any(host_cmd_priv_test, c2h_wk))]
        {
            _cancel_workitem_sync(&mut e.c2h_wk);
            while e.c2h_wk_alive {
                rtw_msleep_os(10);
            }
            if !e.c2h_queue.is_null() {
                while !rtw_cbuf_empty(e.c2h_queue) {
                    let c2h = rtw_cbuf_pop(e.c2h_queue);
                    if !c2h.is_null() && c2h != p as *mut c_void {
                        mfree(c2h as *mut u8, 16);
                    }
                }
                rtw_cbuf_free(e.c2h_queue);
            }
        }
        let _ = e;
    }
}
