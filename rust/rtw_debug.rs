// SPDX-License-Identifier: GPL-2.0
//! W3-130/W3-131 debug dumps — Rust port of `core/rtw_debug.c` leaves.

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

#[cfg(config_rtw_debug)]
use core::ffi::c_uint;
use core::ffi::{c_char, c_int, c_void};

const _DRV_MAX_: c_int = 6;

extern "C" {
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_drv_name() -> *const c_char;
    fn rtw_rust_debug_driver_version() -> *const c_char;
}

#[cfg(config_rtw_debug)]
extern "C" {
    static mut rtw_drv_log_level: c_uint;
    fn rtw_rust_debug_log_level_str(idx: c_int) -> *const c_char;
}

fn c_str_bytes(ptr: *const c_char) -> &'static [u8] {
    if ptr.is_null() {
        return b"";
    }
    unsafe {
        let mut len = 0usize;
        while *ptr.add(len) != 0 {
            len += 1;
        }
        core::slice::from_raw_parts(ptr as *const u8, len)
    }
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

#[no_mangle]
pub extern "C" fn dump_drv_version(sel: *mut c_void) {
    let name = c_str_bytes(unsafe { rtw_rust_debug_drv_name() });
    let ver = c_str_bytes(unsafe { rtw_rust_debug_driver_version() });
    let mut line = [0u8; 128];
    let mut pos = 0usize;
    for &b in name {
        if pos >= line.len().saturating_sub(1) {
            break;
        }
        line[pos] = b;
        pos += 1;
    }
    if pos < line.len().saturating_sub(1) {
        line[pos] = b' ';
        pos += 1;
    }
    for &b in ver {
        if pos >= line.len().saturating_sub(1) {
            break;
        }
        line[pos] = b;
        pos += 1;
    }
    print_line(sel, &line[..pos]);
}

#[no_mangle]
pub extern "C" fn dump_log_level(sel: *mut c_void) {
    #[cfg(config_rtw_debug)]
    {
        let level = unsafe { rtw_drv_log_level };
        let mut hdr = [0u8; 48];
        let prefix = b"drv_log_level:";
        hdr[..prefix.len()].copy_from_slice(prefix);
        let mut n = prefix.len();
        let (digits, len) = format_u32(level);
        for i in 0..len {
            if n >= hdr.len().saturating_sub(1) {
                break;
            }
            hdr[n] = digits[i];
            n += 1;
        }
        print_line(sel, &hdr[..n]);

        for i in 0..=_DRV_MAX_ {
            let label = unsafe { rtw_rust_debug_log_level_str(i) };
            if label.is_null() {
                continue;
            }
            let mark = if level == i as u32 { b'+' } else { b' ' };
            let text = c_str_bytes(label);
            let mut line = [0u8; 96];
            line[0] = mark;
            line[1] = b' ';
            let mut pos = 2usize;
            for &b in text {
                if pos >= line.len().saturating_sub(4) {
                    break;
                }
                line[pos] = b;
                pos += 1;
            }
            if pos < line.len().saturating_sub(4) {
                line[pos] = b' ';
                pos += 1;
                line[pos] = b'=';
                pos += 1;
                line[pos] = b' ';
                pos += 1;
            }
            let (digits, len) = format_u32(i as u32);
            for j in 0..len {
                if pos >= line.len().saturating_sub(1) {
                    break;
                }
                line[pos] = digits[j];
                pos += 1;
            }
            print_line(sel, &line[..pos]);
        }
    }
    #[cfg(not(config_rtw_debug))]
    {
        print_line(sel, b"CONFIG_RTW_DEBUG is disabled");
    }
}

fn format_u32(v: u32) -> ([u8; 10], usize) {
    let mut buf = [0u8; 10];
    if v == 0 {
        buf[0] = b'0';
        return (buf, 1);
    }
    let mut digits = [0u8; 10];
    let mut n = 0usize;
    let mut x = v;
    while x > 0 && n < digits.len() {
        digits[n] = b'0' + (x % 10) as u8;
        x /= 10;
        n += 1;
    }
    for i in 0..n {
        buf[i] = digits[n - 1 - i];
    }
    (buf, n)
}

#[cfg(config_proc_debug)]
extern "C" {
    fn rtw_rust_debug_dump_drv_cfg_part1(sel: *mut c_void);
    fn rtw_rust_debug_dump_drv_cfg_tail(sel: *mut c_void);
}

#[cfg(config_proc_debug)]
#[no_mangle]
pub extern "C" fn dump_drv_cfg(sel: *mut c_void) {
    unsafe {
        rtw_rust_debug_dump_drv_cfg_part1(sel);
        rtw_rust_debug_dump_drv_cfg_tail(sel);
    }
}
