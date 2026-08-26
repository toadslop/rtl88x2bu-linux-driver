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

#[cfg(all(not(host_cmd_priv_test), not(host_cmd_queue_test)))]
use core::ffi::{c_int, c_void};
#[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
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
    #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
    fn rtw_zmalloc(sz: u32) -> *mut u8;
    #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
    fn rtw_mfree(p: *mut u8, sz: u32);
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_zmalloc(sz: u32) -> *mut u8;
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_mfree(p: *mut u8, sz: u32);
}

#[inline]
fn zmalloc(sz: u32) -> *mut u8 {
    unsafe {
        #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
        {
            return rtw_zmalloc(sz);
        }
        #[cfg(all(not(host_cmd_priv_test), not(host_cmd_queue_test)))]
        {
            return _rtw_zmalloc(sz);
        }
    }
}

#[inline]
fn mfree(p: *mut u8, sz: u32) {
    if !p.is_null() {
        unsafe {
            #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
            {
                rtw_mfree(p, sz);
            }
            #[cfg(all(not(host_cmd_priv_test), not(host_cmd_queue_test)))]
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

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
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

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
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

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
#[no_mangle]
pub extern "C" fn _rtw_init_evt_priv(p: *mut EvtPriv) -> Sint {
    if p.is_null() {
        return _FAIL;
    }
    #[allow(unused_mut)]
    let mut res = _SUCCESS;
    unsafe {
        let e = &mut *p;
        e.event_seq = 0;
        e.evt_done_cnt = 0;
        #[cfg(any(host_cmd_priv_test, event_thread_mode))]
        {
            _rtw_init_sema(&mut e.evt_notify, 0);
            e.evt_allocated_buf = zmalloc(1024 + 4);
            if e.evt_allocated_buf.is_null() {
                res = _FAIL;
            } else {
                e.evt_buf = aligned_buf(e.evt_allocated_buf, 4);
                init_queue(&mut e.evt_queue);
            }
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
    res
}

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
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

// --- W3-61 cmd/evt queue enqueue/filter (core/rtw_cmd_queue.c) ---

#[cfg(any(host_cmd_queue_test, rust_cmd_queue))]
mod cmd_queue {
    use super::{_FAIL, _SUCCESS, List, Queue, MAX_CMDSZ, c_int, c_void, Sint};
    #[cfg(not(host_cmd_queue_test))]
    use super::{CmdPriv, EvtPriv};

    extern "C" {
        #[cfg(host_cmd_queue_test)]
        fn rtw_mfree(p: *mut u8, sz: u32);
        #[cfg(host_cmd_queue_test)]
        fn _rtw_up_sema(s: *mut c_int);
    }

    #[inline]
    fn qfree(p: *mut u8, sz: u32) {
        if !p.is_null() {
            unsafe {
                #[cfg(host_cmd_queue_test)]
                rtw_mfree(p, sz);
                #[cfg(not(host_cmd_queue_test))]
                kernel::_rtw_mfree(p, sz);
            }
        }
    }

    const CMD_SET_DRV_EXTRA: u16 = 12;
    const CMD_SET_CHANPLAN: u16 = 13;

    #[repr(C)]
    pub struct CmdObj {
        pub padapter: *mut c_void,
        pub cmdcode: u16,
        pub res: u8,
        pub no_io: u8,
        pub parmbuf: *mut u8,
        pub cmdsz: u32,
        pub rsp: *mut u8,
        pub rspsz: u32,
        pub sctx: *mut c_void,
        pub list: List,
    }

    #[repr(C)]
    pub struct EvtObj {
        pub evtcode: u16,
        pub res: u8,
        pub parmbuf: *mut u8,
        pub evtsz: u32,
        pub list: List,
    }

    #[cfg(host_cmd_queue_test)]
    #[repr(C)]
    pub struct QueueCmdPriv {
        pub cmd_queue_sema: c_int,
        pub start_cmdthread_sema: c_int,
        pub cmd_queue: Queue,
        pub cmdthd_running: c_int,
        pub padapter: *mut c_void,
    }

    #[cfg(host_cmd_queue_test)]
    #[repr(C)]
    pub struct QueueEvtPriv {
        pub evt_notify: c_int,
        pub evt_queue: Queue,
        pub evt_done_cnt: u32,
    }

    #[cfg(not(host_cmd_queue_test))]
    type QueueCmdPriv = CmdPriv;

    #[cfg(not(host_cmd_queue_test))]
    type QueueEvtPriv = EvtPriv;

    #[repr(C)]
    struct DrvextraCmdParm {
        ec_id: c_int,
        type_: c_int,
        size: c_int,
        pbuf: *mut u8,
    }

    #[cfg(not(host_cmd_queue_test))]
    mod kernel {
        use super::*;

        extern "C" {
            pub fn rtw_rust_hw_init_completed(adapter: *mut c_void) -> u8;
            pub fn rtw_rust_queue_enter_critical(lock: *mut c_int, irql: *mut c_ulong);
            pub fn rtw_rust_queue_exit_critical(lock: *mut c_int, irql: *mut c_ulong);
            pub fn rtw_rust_queue_enter_critical_bh(lock: *mut c_int, irql: *mut c_ulong);
            pub fn rtw_rust_queue_exit_critical_bh(lock: *mut c_int, irql: *mut c_ulong);
            pub fn _rtw_up_sema(sema: *mut c_int);
            pub fn _rtw_mfree(p: *mut u8, sz: u32);
            pub fn rtw_rust_cmd_priv_cmd_queue(p: *mut c_void) -> *mut Queue;
            pub fn rtw_rust_cmd_priv_cmd_queue_sema(p: *mut c_void) -> *mut c_void;
            pub fn rtw_rust_cmd_priv_for_enqueue(p: *mut c_void) -> *mut c_void;
            pub fn rtw_rust_cmd_priv_padapter(p: *mut c_void) -> *mut c_void;
            pub fn rtw_rust_cmd_priv_cmdthd_running(p: *mut c_void) -> c_int;
            #[cfg(event_thread_mode)]
            pub fn rtw_rust_evt_priv_evt_queue(p: *mut c_void) -> *mut Queue;
            #[cfg(event_thread_mode)]
            pub fn rtw_rust_evt_priv_evt_notify(p: *mut c_void) -> *mut c_void;
            #[cfg(event_thread_mode)]
            pub fn rtw_rust_evt_priv_evt_done_cnt(p: *mut c_void) -> *mut u32;
        }
    }

    #[cfg(not(host_cmd_queue_test))]
    use core::ffi::c_ulong;

    #[inline]
    fn list_empty(h: &List) -> bool {
        h.next == h as *const List as *mut List
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
    fn list_insert_head(n: &mut List, head: &mut List) {
        unsafe {
            let next = head.next;
            n.next = next;
            n.prev = head as *mut List;
            (*next).prev = n as *mut List;
            head.next = n as *mut List;
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
    fn enter_critical(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_enter_critical(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    #[inline]
    fn exit_critical(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_exit_critical(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    #[inline]
    fn enter_critical_bh(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_enter_critical_bh(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    #[inline]
    fn exit_critical_bh(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_exit_critical_bh(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    #[inline]
    fn hw_init_completed(adapter: *mut c_void) -> bool {
        if adapter.is_null() {
            return false;
        }
        #[cfg(host_cmd_queue_test)]
        unsafe {
            return *(adapter as *const u8) == 1;
        }
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_hw_init_completed(adapter) == 1
        }
    }

    #[no_mangle]
    pub extern "C" fn _rtw_enqueue_cmd(
        queue: *mut Queue,
        obj: *mut CmdObj,
        to_head: bool,
    ) -> Sint {
        if queue.is_null() || obj.is_null() {
            return _SUCCESS;
        }
        unsafe {
            let q = &mut *queue;
            let mut irqL = 0usize as c_ulong;
            enter_critical(&mut q.lock, &mut irqL);
            if to_head {
                list_insert_head(&mut (*obj).list, &mut q.queue);
            } else {
                list_insert_tail(&mut (*obj).list, &mut q.queue);
            }
            exit_critical(&mut q.lock, &mut irqL);
        }
        _SUCCESS
    }

    #[no_mangle]
    pub extern "C" fn _rtw_dequeue_cmd(queue: *mut Queue) -> *mut CmdObj {
        if queue.is_null() {
            return core::ptr::null_mut();
        }
        unsafe {
            let q = &mut *queue;
            let mut irqL = 0usize as c_ulong;
            enter_critical(&mut q.lock, &mut irqL);
            let obj = if list_empty(&q.queue) {
                core::ptr::null_mut()
            } else {
                let ln = q.queue.next;
                let obj = (ln as *mut u8).offset(-(core::mem::offset_of!(CmdObj, list) as isize))
                    as *mut CmdObj;
                list_delete(&mut *ln);
                obj
            };
            exit_critical(&mut q.lock, &mut irqL);
            obj
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_cmd_filter(pcmdpriv: *mut QueueCmdPriv, cmd_obj: *mut CmdObj) -> Sint {
        if pcmdpriv.is_null() || cmd_obj.is_null() {
            return _FAIL;
        }
        unsafe {
            let cmd = &*cmd_obj;
            let mut allow = 0u8;
            if cmd.cmdcode == CMD_SET_CHANPLAN {
                allow = 1;
            }
            if cmd.no_io != 0 {
                allow = 1;
            }
            #[cfg(host_cmd_queue_test)]
            let (adapter, thd_ok) = {
                let priv_ = &*pcmdpriv;
                (priv_.padapter, priv_.cmdthd_running != 0)
            };
            #[cfg(not(host_cmd_queue_test))]
            let (adapter, thd_ok) = {
                let p = pcmdpriv as *mut c_void;
                (
                    kernel::rtw_rust_cmd_priv_padapter(p),
                    kernel::rtw_rust_cmd_priv_cmdthd_running(p) != 0,
                )
            };
            let hw_ok = hw_init_completed(adapter);
            if (!hw_ok && allow == 0) || !thd_ok {
                return _FAIL;
            }
        }
        _SUCCESS
    }

    #[no_mangle]
    pub extern "C" fn rtw_enqueue_cmd(pcmdpriv: *mut QueueCmdPriv, cmd_obj: *mut CmdObj) -> u32 {
        if pcmdpriv.is_null() || cmd_obj.is_null() {
            return _FAIL as u32;
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                let priv_ = &mut *pcmdpriv;
                (*cmd_obj).padapter = priv_.padapter;
                if rtw_cmd_filter(pcmdpriv, cmd_obj) == _FAIL || (*cmd_obj).cmdsz > MAX_CMDSZ {
                    if (*cmd_obj).cmdcode == CMD_SET_DRV_EXTRA {
                        let extra = (*cmd_obj).parmbuf as *mut DrvextraCmdParm;
                        if !extra.is_null() && (*extra).size > 0 && !(*extra).pbuf.is_null() {
                            qfree((*extra).pbuf, (*extra).size as u32);
                        }
                    }
                    rtw_free_cmd_obj(cmd_obj);
                    return _FAIL as u32;
                }
                let res = _rtw_enqueue_cmd(&mut priv_.cmd_queue, cmd_obj, false);
                if res == _SUCCESS {
                    _rtw_up_sema(&mut priv_.cmd_queue_sema);
                }
                return res as u32;
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                let p = pcmdpriv as *mut c_void;
                let padapter = kernel::rtw_rust_cmd_priv_padapter(p);
                (*cmd_obj).padapter = padapter;
                let priv_for_enqueue =
                    kernel::rtw_rust_cmd_priv_for_enqueue(p) as *mut QueueCmdPriv;
                if rtw_cmd_filter(priv_for_enqueue, cmd_obj) == _FAIL
                    || (*cmd_obj).cmdsz > MAX_CMDSZ
                {
                    if (*cmd_obj).cmdcode == CMD_SET_DRV_EXTRA {
                        let extra = (*cmd_obj).parmbuf as *mut DrvextraCmdParm;
                        if !extra.is_null() && (*extra).size > 0 && !(*extra).pbuf.is_null() {
                            qfree((*extra).pbuf, (*extra).size as u32);
                        }
                    }
                    rtw_free_cmd_obj(cmd_obj);
                    return _FAIL as u32;
                }
                let priv_p = priv_for_enqueue as *mut c_void;
                let q = kernel::rtw_rust_cmd_priv_cmd_queue(priv_p);
                let res = _rtw_enqueue_cmd(q, cmd_obj, false);
                if res == _SUCCESS {
                    kernel::_rtw_up_sema(
                        kernel::rtw_rust_cmd_priv_cmd_queue_sema(priv_p) as *mut c_int,
                    );
                }
                return res as u32;
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_dequeue_cmd(pcmdpriv: *mut QueueCmdPriv) -> *mut CmdObj {
        if pcmdpriv.is_null() {
            return core::ptr::null_mut();
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                _rtw_dequeue_cmd(&mut (*pcmdpriv).cmd_queue)
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                _rtw_dequeue_cmd(kernel::rtw_rust_cmd_priv_cmd_queue(pcmdpriv as *mut c_void))
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_free_cmd_obj(pcmd: *mut CmdObj) {
        if pcmd.is_null() {
            return;
        }
        unsafe {
            let p = &mut *pcmd;
            if !p.parmbuf.is_null() {
                qfree(p.parmbuf, p.cmdsz);
            }
            if !p.rsp.is_null() && p.rspsz != 0 {
                qfree(p.rsp, p.rspsz);
            }
            qfree(pcmd as *mut u8, core::mem::size_of::<CmdObj>() as u32);
        }
    }

    #[cfg(any(host_cmd_queue_test, event_thread_mode))]
    #[no_mangle]
    pub extern "C" fn rtw_enqueue_evt(pevtpriv: *mut QueueEvtPriv, obj: *mut EvtObj) -> u32 {
        if pevtpriv.is_null() || obj.is_null() {
            return _FAIL as u32;
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                let q = &mut (*pevtpriv).evt_queue;
                let mut irqL = 0usize as c_ulong;
                enter_critical(&mut q.lock, &mut irqL);
                list_insert_tail(&mut (*obj).list, &mut q.queue);
                exit_critical(&mut q.lock, &mut irqL);
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                let q = kernel::rtw_rust_evt_priv_evt_queue(pevtpriv as *mut c_void);
                let mut irqL = 0usize as c_ulong;
                enter_critical_bh(&mut (*q).lock, &mut irqL);
                list_insert_tail(&mut (*obj).list, &mut (*q).queue);
                exit_critical_bh(&mut (*q).lock, &mut irqL);
            }
        }
        _SUCCESS as u32
    }

    #[cfg(any(host_cmd_queue_test, event_thread_mode))]
    #[no_mangle]
    pub extern "C" fn rtw_free_evt_obj(pevtobj: *mut EvtObj) {
        if pevtobj.is_null() {
            return;
        }
        unsafe {
            let p = &mut *pevtobj;
            if !p.parmbuf.is_null() {
                qfree(p.parmbuf, p.evtsz);
            }
            qfree(pevtobj as *mut u8, core::mem::size_of::<EvtObj>() as u32);
        }
    }

    #[cfg(any(host_cmd_queue_test, event_thread_mode))]
    #[no_mangle]
    pub extern "C" fn rtw_evt_notify_isr(pevtpriv: *mut QueueEvtPriv) {
        if pevtpriv.is_null() {
            return;
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                (*pevtpriv).evt_done_cnt += 1;
                _rtw_up_sema(&mut (*pevtpriv).evt_notify);
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                let p = pevtpriv as *mut c_void;
                *kernel::rtw_rust_evt_priv_evt_done_cnt(p) += 1;
                kernel::_rtw_up_sema(
                    kernel::rtw_rust_evt_priv_evt_notify(p) as *mut c_int,
                );
            }
        }
    }
}
