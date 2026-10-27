// SPDX-License-Identifier: GPL-2.0
//! W3-171 RX BH tracking dump from `core/rtw_recv.c` (DBG_RX_BH_TRACKING).

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

#[cfg(host_recv_bh_dbg_test)]
use std::os::raw::{c_char, c_void};

#[cfg(not(host_recv_bh_dbg_test))]
use core::ffi::{c_char, c_void};

type U32 = u32;

#[cfg(host_recv_bh_dbg_test)]
#[repr(C)]
pub struct RecvPriv {
    pub rx_bh_stage: U32,
    pub rx_bh_buf_dq_cnt: U32,
    pub rx_bh_lbuf: *mut c_void,
    pub rx_bh_cbuf: *mut c_void,
    pub rx_bh_cbuf_data: *mut c_void,
    pub rx_bh_cbuf_dlen: U32,
    pub rx_bh_cbuf_pos: U32,
    pub rx_bh_cframe: *mut c_void,
}

fn push_byte(out: &mut [u8], pos: &mut usize, b: u8) {
    if *pos < out.len() {
        out[*pos] = b;
        *pos += 1;
    }
}

fn push_str(out: &mut [u8], pos: &mut usize, s: &[u8]) {
    for &b in s {
        push_byte(out, pos, b);
    }
}

fn push_u32(out: &mut [u8], pos: &mut usize, v: U32) {
    let mut tmp = [0u8; 10];
    let mut n = 0usize;
    let mut x = v;
    if x == 0 {
        push_byte(out, pos, b'0');
        return;
    }
    while x > 0 {
        tmp[n] = b'0' + (x % 10) as u8;
        x /= 10;
        n += 1;
    }
    while n > 0 {
        n -= 1;
        push_byte(out, pos, tmp[n]);
    }
}

/// Glibc `fprintf` / host L2 oracle style (`(nil)` / `0x` unpadded hex).
#[cfg(host_recv_bh_dbg_test)]
fn push_ptr(out: &mut [u8], pos: &mut usize, p: usize) {
    if p == 0 {
        push_str(out, pos, b"(nil)");
        return;
    }
    push_str(out, pos, b"0x");
    let mut tmp = [0u8; 16];
    let mut n = 0usize;
    let mut x = p;
    while x > 0 {
        let nib = (x & 0xf) as u8;
        tmp[n] = if nib < 10 {
            b'0' + nib
        } else {
            b'a' + (nib - 10)
        };
        x >>= 4;
        n += 1;
    }
    while n > 0 {
        n -= 1;
        push_byte(out, pos, tmp[n]);
    }
}

/// Kernel `RTW_PRINT_SEL(..., %p, ...)` on 6.12 (`(null)` / zero-padded hex, no `0x`).
#[cfg(not(host_recv_bh_dbg_test))]
fn push_ptr(out: &mut [u8], pos: &mut usize, p: usize) {
    if p == 0 {
        push_str(out, pos, b"(null)");
        return;
    }
    for shift in (0..16).rev() {
        let nib = ((p >> (shift * 4)) & 0xf) as u8;
        push_byte(
            out,
            pos,
            if nib < 10 {
                b'0' + nib
            } else {
                b'a' + (nib - 10)
            },
        );
    }
}

#[cfg(not(host_recv_bh_dbg_test))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_recv_bh_stage(recv: *mut c_void) -> U32;
        pub fn rtw_rust_recv_bh_buf_dq_cnt(recv: *mut c_void) -> U32;
        pub fn rtw_rust_recv_bh_lbuf(recv: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_recv_bh_cbuf(recv: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_recv_bh_cbuf_dlen(recv: *mut c_void) -> U32;
        pub fn rtw_rust_recv_bh_cbuf_pos(recv: *mut c_void) -> U32;
        pub fn rtw_rust_recv_bh_cframe(recv: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_recv_bh_print_sel(sel: *mut c_void, line: *const c_char);
    }
}

#[cfg(host_recv_bh_dbg_test)]
extern "C" {
    fn rtw_rust_recv_bh_print_sel(sel: *mut c_void, line: *const c_char);
}

fn emit(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 160];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        #[cfg(host_recv_bh_dbg_test)]
        rtw_rust_recv_bh_print_sel(sel, buf.as_ptr());
        #[cfg(not(host_recv_bh_dbg_test))]
        kernel::rtw_rust_recv_bh_print_sel(sel, buf.as_ptr());
    }
}

#[no_mangle]
pub extern "C" fn dump_rx_bh_tk(sel: *mut c_void, recv: *mut c_void) {
    if recv.is_null() {
        return;
    }
    let (stage, dq, lbuf, cbuf, dlen, pos, cframe) = {
        #[cfg(host_recv_bh_dbg_test)]
        {
            let r = unsafe { &*(recv as *const RecvPriv) };
            (
                r.rx_bh_stage,
                r.rx_bh_buf_dq_cnt,
                r.rx_bh_lbuf as usize,
                r.rx_bh_cbuf as usize,
                r.rx_bh_cbuf_dlen,
                r.rx_bh_cbuf_pos,
                r.rx_bh_cframe as usize,
            )
        }
        #[cfg(not(host_recv_bh_dbg_test))]
        {
            unsafe {
                (
                    kernel::rtw_rust_recv_bh_stage(recv),
                    kernel::rtw_rust_recv_bh_buf_dq_cnt(recv),
                    kernel::rtw_rust_recv_bh_lbuf(recv) as usize,
                    kernel::rtw_rust_recv_bh_cbuf(recv) as usize,
                    kernel::rtw_rust_recv_bh_cbuf_dlen(recv),
                    kernel::rtw_rust_recv_bh_cbuf_pos(recv),
                    kernel::rtw_rust_recv_bh_cframe(recv) as usize,
                )
            }
        }
    };
    let mut line = [0u8; 160];
    let mut p = 0usize;
    push_str(&mut line, &mut p, b"[RXBHTK]s:");
    push_u32(&mut line, &mut p, stage);
    push_str(&mut line, &mut p, b", buf_dqc:");
    push_u32(&mut line, &mut p, dq);
    push_str(&mut line, &mut p, b", lbuf:");
    push_ptr(&mut line, &mut p, lbuf);
    push_str(&mut line, &mut p, b", cbuf:");
    push_ptr(&mut line, &mut p, cbuf);
    push_str(&mut line, &mut p, b", dlen:");
    push_u32(&mut line, &mut p, dlen);
    push_str(&mut line, &mut p, b", pos:");
    push_u32(&mut line, &mut p, pos);
    push_str(&mut line, &mut p, b", cframe:");
    push_ptr(&mut line, &mut p, cframe);
    push_byte(&mut line, &mut p, b'\n');
    emit(sel, &line[..p]);
}
