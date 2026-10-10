// SPDX-License-Identifier: GPL-2.0
//! W3-134 tx/rx debug dump leaves (part 1) from `core/rtw_debug_rest.c`.

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

use core::ffi::{c_char, c_int, c_void};

type U8 = u8;
type U16 = u16;

const RX_AMPDU_SIZE_INVALID: U8 = 0xff;

extern "C" {
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_log_info(line: *const c_char);
    fn rtw_rust_debug_recv_sink_udpport(adapter: *mut c_void) -> U16;
    fn rtw_rust_debug_recv_pre_rtp_rxseq(adapter: *mut c_void) -> U16;
    fn rtw_rust_debug_recv_cur_rtp_rxseq(adapter: *mut c_void) -> U16;
    fn rtw_rust_debug_recv_set_pre_rtp_rxseq(adapter: *mut c_void, seq: U16);
    fn rtw_rust_debug_recv_set_cur_rtp_rxseq(adapter: *mut c_void, seq: U16);
    fn rtw_rust_debug_sta_reorder_get(
        sta: *mut c_void,
        tid: c_int,
        enable: *mut U8,
        ampdu_size: *mut U8,
        indicate_seq: *mut U16,
    );
}

fn print_bytes(sel: *mut c_void, bytes: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(bytes.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = bytes[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel(sel, buf.as_ptr());
    }
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut with_nl = [0u8; 260];
    let n = core::cmp::min(line.len(), with_nl.len().saturating_sub(2));
    with_nl[..n].copy_from_slice(&line[..n]);
    with_nl[n] = b'\n';
    print_bytes(sel, &with_nl[..n + 1]);
}

fn push_byte(out: &mut [u8], pos: &mut usize, b: U8) {
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

fn push_u16_dec(out: &mut [u8], pos: &mut usize, v: U16) {
    let mut tmp = [0u8; 5];
    let mut n = 0usize;
    let mut x = v;
    if x == 0 {
        push_byte(out, pos, b'0');
        return;
    }
    while x > 0 && n < tmp.len() {
        tmp[n] = b'0' + (x % 10) as U8;
        x /= 10;
        n += 1;
    }
    while n > 0 {
        n -= 1;
        push_byte(out, pos, tmp[n]);
    }
}

#[no_mangle]
pub extern "C" fn rtw_sink_rtp_seq_dbg(adapter: *mut c_void, ehdr_pos: *mut U8) {
    if adapter.is_null() || ehdr_pos.is_null() {
        return;
    }
    let sink_port = unsafe { rtw_rust_debug_recv_sink_udpport(adapter) };
    if sink_port == 0 {
        return;
    }
    let pkt_port = unsafe { core::ptr::read_unaligned(ehdr_pos.add(0x24) as *const U16) };
    if pkt_port != sink_port.to_be() {
        return;
    }
    let pre = unsafe { rtw_rust_debug_recv_cur_rtp_rxseq(adapter) };
    let cur = unsafe { U16::from_be(core::ptr::read_unaligned(ehdr_pos.add(0x2c) as *const U16)) };
    unsafe {
        rtw_rust_debug_recv_set_pre_rtp_rxseq(adapter, pre);
        rtw_rust_debug_recv_set_cur_rtp_rxseq(adapter, cur);
    }
    if pre + 1 == cur {
        return;
    }
    if pre == 65535 && cur == 0 {
        return;
    }
    let mut line = [0u8; 96];
    let mut pos = 0usize;
    push_str(
        &mut line,
        &mut pos,
        b"rtw_sink_rtp_seq_dbg : RTP Seq num from ",
    );
    push_u16_dec(&mut line, &mut pos, pre);
    push_str(&mut line, &mut pos, b" to ");
    push_u16_dec(&mut line, &mut pos, cur);
    push_byte(&mut line, &mut pos, b'\n');
    line[pos] = 0;
    unsafe {
        rtw_rust_debug_log_info(line.as_ptr() as *const c_char);
    }
}

#[no_mangle]
pub extern "C" fn sta_rx_reorder_ctl_dump(sel: *mut c_void, sta: *mut c_void) {
    if sta.is_null() {
        return;
    }
    for i in 0..16 {
        let mut enable = 0u8;
        let mut ampdu_size = 0u8;
        let mut indicate_seq = 0u16;
        unsafe {
            rtw_rust_debug_sta_reorder_get(sta, i, &mut enable, &mut ampdu_size, &mut indicate_seq);
        }
        if ampdu_size == RX_AMPDU_SIZE_INVALID && indicate_seq == 0xffff {
            continue;
        }
        let mut line = [0u8; 80];
        let mut pos = 0usize;
        push_str(&mut line, &mut pos, b"tid=");
        push_u16_dec(&mut line, &mut pos, i as U16);
        push_str(&mut line, &mut pos, b", enable=");
        push_byte(&mut line, &mut pos, b'0' + enable);
        push_str(&mut line, &mut pos, b", ampdu_size=");
        push_u16_dec(&mut line, &mut pos, ampdu_size as U16);
        push_str(&mut line, &mut pos, b", indicate_seq=");
        push_u16_dec(&mut line, &mut pos, indicate_seq);
        print_line(sel, &line[..pos]);
    }
}
