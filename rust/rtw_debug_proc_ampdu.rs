// SPDX-License-Identifier: GPL-2.0
// proc rx_ampdu get (W3-204 PR1).

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

use core::ffi::{c_char, c_int, c_long, c_void};

type U8 = u8;

const RX_AMPDU_ACCEPT_INVALID: U8 = 0xff;
const RX_AMPDU_SIZE_INVALID: U8 = 0xff;
const RX_AMPDU_DRV_FIXED: U8 = 1;

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_fix_rx_ampdu_accept(adapter: *mut c_void) -> U8;
    fn rtw_rust_debug_fix_rx_ampdu_size(adapter: *mut c_void) -> U8;
    fn rtw_rust_debug_proc_read_user_line(
        buffer: *const c_void,
        count: usize,
        tmp: *mut c_char,
        tmp_size: usize,
    ) -> c_long;
    fn rtw_rx_ampdu_is_accept(adapter: *mut c_void) -> U8;
    fn rtw_rx_ampdu_size(adapter: *mut c_void) -> U8;
    fn rtw_rx_ampdu_set_accept(adapter: *mut c_void, accept: U8, reason: U8) -> U8;
    fn rtw_rx_ampdu_set_size(adapter: *mut c_void, size: U8, reason: U8) -> U8;
    fn rtw_rx_ampdu_apply(adapter: *mut c_void);
    fn dump_regsty_rx_ampdu_size_limit(sel: *mut c_void, adapter: *mut c_void);
}

fn print_bytes(sel: *mut c_void, bytes: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(bytes.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = bytes[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel(sel, buf.as_ptr());
    }
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut with_nl = [0u8; 132];
    let n = core::cmp::min(line.len(), with_nl.len().saturating_sub(2));
    with_nl[..n].copy_from_slice(&line[..n]);
    with_nl[n] = b'\n';
    print_bytes(sel, &with_nl[..n + 1]);
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

fn push_u8_dec_width_left(out: &mut [u8], pos: &mut usize, v: U8, width: usize) {
    let start = *pos;
    push_u8_dec(out, pos, v);
    while *pos - start < width {
        push_byte(out, pos, b' ');
    }
}

fn emit_get_rx_ampdu(sel: *mut c_void, adapter: *mut c_void) {
    let fix_accept = unsafe { rtw_rust_debug_fix_rx_ampdu_accept(adapter) };
    let fix_size = unsafe { rtw_rust_debug_fix_rx_ampdu_size(adapter) };
    let mut line = [0u8; 48];
    let mut pos;

    print_bytes(sel, b"accept: ");
    pos = 0;
    if fix_accept == RX_AMPDU_ACCEPT_INVALID {
        push_u8_dec(&mut line, &mut pos, unsafe {
            rtw_rx_ampdu_is_accept(adapter)
        });
        push_str(&mut line, &mut pos, b"(auto)");
    } else {
        push_u8_dec(&mut line, &mut pos, fix_accept);
        push_str(&mut line, &mut pos, b"(fixed)");
    }
    print_line(sel, &line[..pos]);

    print_bytes(sel, b"size: ");
    pos = 0;
    if fix_size == RX_AMPDU_SIZE_INVALID {
        push_u8_dec(&mut line, &mut pos, unsafe { rtw_rx_ampdu_size(adapter) });
        push_str(&mut line, &mut pos, b"(auto) with conditional limit:");
        print_line(sel, &line[..pos]);
        unsafe { dump_regsty_rx_ampdu_size_limit(sel, adapter) };
    } else {
        push_u8_dec(&mut line, &mut pos, fix_size);
        push_str(&mut line, &mut pos, b"(fixed)");
        print_line(sel, &line[..pos]);
    }
    print_line(sel, b"");
    print_line(sel, b"fix_rx_ampdu_accept fix_rx_ampdu_size");
    pos = 0;
    push_u8_dec_width_left(&mut line, &mut pos, fix_accept, 19);
    push_byte(&mut line, &mut pos, b' ');
    push_u8_dec_width_left(&mut line, &mut pos, fix_size, 17);
    print_line(sel, &line[..pos]);
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_rx_ampdu(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = if m.is_null() {
        core::ptr::null_mut()
    } else {
        unsafe { rtw_rust_debug_proc_adapter(m) }
    };
    if !adapter.is_null() {
        emit_get_rx_ampdu(m, adapter);
    }
    0
}

fn parse_u8_field(bytes: &[u8]) -> Option<U8> {
    if bytes.is_empty() {
        return None;
    }
    let mut val: u32 = 0;
    for &b in bytes {
        if b == b' ' || b == b'\t' || b == b'\n' || b == b'\r' {
            continue;
        }
        if b < b'0' || b > b'9' {
            return None;
        }
        val = val * 10 + (b - b'0') as u32;
        if val > 255 {
            return None;
        }
    }
    Some(val as U8)
}

fn parse_set_line(tmp: &[u8], count: usize) -> (Option<U8>, Option<U8>) {
    let mut n = count;
    while n > 0 && (tmp[n - 1] == b'\n' || tmp[n - 1] == b'\r' || tmp[n - 1] == b' ') {
        n -= 1;
    }
    let slice = &tmp[..n];
    let mut parts = 0usize;
    let mut accept: Option<U8> = None;
    let mut size: Option<U8> = None;
    let mut start = 0usize;
    let mut i = 0usize;
    while i <= slice.len() {
        if i == slice.len() || slice[i] == b' ' || slice[i] == b'\t' {
            if start < i {
                let field = &slice[start..i];
                if parts == 0 {
                    accept = parse_u8_field(field);
                    parts = 1;
                } else if parts == 1 {
                    size = parse_u8_field(field);
                    parts = 2;
                }
            }
            start = i + 1;
        }
        i += 1;
    }
    (accept, size)
}

fn apply_set_rx_ampdu(adapter: *mut c_void, tmp: &[u8], count: usize) -> c_long {
    if adapter.is_null() || count < 1 {
        return -1;
    }
    let (accept, size) = parse_set_line(tmp, count);
    if accept.is_some() {
        unsafe {
            rtw_rx_ampdu_set_accept(adapter, accept.unwrap(), RX_AMPDU_DRV_FIXED);
        }
    }
    if size.is_some() {
        unsafe {
            rtw_rx_ampdu_set_size(adapter, size.unwrap(), RX_AMPDU_DRV_FIXED);
        }
    }
    if accept.is_some() || size.is_some() {
        unsafe { rtw_rx_ampdu_apply(adapter) };
    }
    count as c_long
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_rx_ampdu(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    if count < 1 {
        return -1;
    }
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    let mut tmp = [0i8; 32];
    let n = unsafe {
        rtw_rust_debug_proc_read_user_line(buffer, count, tmp.as_mut_ptr(), tmp.len())
    };
    if n < 0 {
        return n;
    }
    apply_set_rx_ampdu(
        adapter,
        unsafe { core::slice::from_raw_parts(tmp.as_ptr() as *const u8, n as usize) },
        n as usize,
    )
}

#[cfg(host_rtw_debug_proc_ampdu_test)]
#[no_mangle]
pub extern "C" fn host_proc_set_rx_ampdu_from_buf(
    adapter: *mut c_void,
    buffer: *const u8,
    count: usize,
) -> c_long {
    if buffer.is_null() {
        return -1;
    }
    let slice = unsafe { core::slice::from_raw_parts(buffer, count) };
    apply_set_rx_ampdu(adapter, slice, count)
}
