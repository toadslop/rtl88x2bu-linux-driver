// SPDX-License-Identifier: GPL-2.0
//! W3-170 regsty RX AMPDU size limit dump from `core/rtw_debug_rest.c`.

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

use core::ffi::{c_char, c_void};

type U8 = u8;

extern "C" {
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_regsty_rx_ampdu_sz_limit_by_nss_bw(adapter: *mut c_void) -> *const U8;
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

fn push_u8_dec(out: &mut [u8], pos: &mut usize, v: U8) {
    if v == 0 {
        push_byte(out, pos, b'0');
        return;
    }
    let mut tmp = [0u8; 3];
    let mut n = 0usize;
    let mut x = v;
    while x > 0 && n < tmp.len() {
        tmp[n] = b'0' + (x % 10);
        x /= 10;
        n += 1;
    }
    while n > 0 {
        n -= 1;
        push_byte(out, pos, tmp[n]);
    }
}

fn push_u8_dec_width(out: &mut [u8], pos: &mut usize, v: U8, width: usize) {
    let mut tmp = [0u8; 8];
    let mut n = 0usize;
    let mut x = v;
    if x == 0 {
        tmp[n] = b'0';
        n = 1;
    } else {
        while x > 0 && n < tmp.len() {
            tmp[n] = b'0' + (x % 10);
            x /= 10;
            n += 1;
        }
    }
    if n < width {
        for _ in 0..width - n {
            push_byte(out, pos, b' ');
        }
    }
    while n > 0 {
        n -= 1;
        push_byte(out, pos, tmp[n]);
    }
}

fn emit_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel(sel, buf.as_ptr());
    }
}

fn format_header_line(out: &mut [u8]) -> usize {
    let mut pos = 0usize;
    push_str(out, &mut pos, b"    20M 40M 80M 160M\n");
    pos
}

fn format_row_line(nss: U8, row: &[U8], out: &mut [u8]) -> usize {
    let mut pos = 0usize;
    push_u8_dec(out, &mut pos, nss);
    push_str(out, &mut pos, b"SS ");
    push_u8_dec_width(out, &mut pos, row[0], 3);
    push_byte(out, &mut pos, b' ');
    push_u8_dec_width(out, &mut pos, row[1], 3);
    push_byte(out, &mut pos, b' ');
    push_u8_dec_width(out, &mut pos, row[2], 3);
    push_byte(out, &mut pos, b' ');
    push_u8_dec_width(out, &mut pos, row[3], 4);
    push_byte(out, &mut pos, b'\n');
    pos
}

#[no_mangle]
pub extern "C" fn dump_regsty_rx_ampdu_size_limit(sel: *mut c_void, adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    let tbl = unsafe { rtw_rust_debug_regsty_rx_ampdu_sz_limit_by_nss_bw(adapter) };
    if tbl.is_null() {
        return;
    }

    let mut line = [0u8; 64];
    let n = format_header_line(&mut line);
    emit_line(sel, &line[..n]);

    for i in 0..4 {
        let row = unsafe { core::slice::from_raw_parts(tbl.add(i * 4), 4) };
        let n = format_row_line((i + 1) as U8, row, &mut line);
        emit_line(sel, &line[..n]);
    }
}
