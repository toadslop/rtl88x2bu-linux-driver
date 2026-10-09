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

#[cfg(any(config_rtw_debug, config_proc_debug))]
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
mod drv_cfg_part1 {
    use super::*;

    extern "C" {
        fn rtw_rust_debug_kernel_release() -> *const c_char;
        fn rtw_rust_debug_drv_cfg_num_values(
            dbg: *mut c_int,
            reg_cert: *mut c_uint,
            txpwr_by_rate: *mut c_int,
            txpwr_by_rate_en: *mut c_int,
            txpwr_limit: *mut c_int,
            txpwr_limit_en: *mut c_int,
            adaptivity_en: *mut c_int,
            adaptivity_mode: *mut c_int,
            busy_deny_ms: *mut c_uint,
        );
        fn rtw_rust_debug_dump_drv_cfg_pre_num_banners(sel: *mut c_void);
        fn rtw_rust_debug_dump_drv_cfg_odm_minimal_banners(sel: *mut c_void);
        fn rtw_rust_debug_dump_drv_cfg_post_num_banners(sel: *mut c_void);
        fn rtw_rust_debug_dump_drv_cfg_tail(sel: *mut c_void);
    }

    fn append_u32(line: &mut [u8], pos: &mut usize, val: u32) {
        let (digits, len) = format_u32(val);
        for i in 0..len {
            if *pos >= line.len().saturating_sub(1) {
                break;
            }
            line[*pos] = digits[i];
            *pos += 1;
        }
    }

    fn print_kv(sel: *mut c_void, key: &[u8], val: u32) {
        let mut line = [0u8; 96];
        let n = core::cmp::min(key.len(), line.len().saturating_sub(12));
        line[..n].copy_from_slice(&key[..n]);
        let mut pos = n;
        append_u32(&mut line, &mut pos, val);
        print_line(sel, &line[..pos]);
    }

    #[no_mangle]
    pub extern "C" fn dump_drv_cfg(sel: *mut c_void) {
        let krel = unsafe { rtw_rust_debug_kernel_release() };
        if !krel.is_null() {
            let rel = c_str_bytes(krel);
            let mut line = [0u8; 128];
            let hdr = b"\nKernel Version: ";
            line[..hdr.len()].copy_from_slice(hdr);
            let mut pos = hdr.len();
            for &b in rel {
                if pos >= line.len().saturating_sub(2) {
                    break;
                }
                line[pos] = b;
                pos += 1;
            }
            print_line(sel, &line[..pos]);
        }

        let ver = c_str_bytes(unsafe { rtw_rust_debug_driver_version() });
        let mut dv = [0u8; 80];
        let pfx = b"Driver Version: ";
        dv[..pfx.len()].copy_from_slice(pfx);
        let mut pos = pfx.len();
        for &b in ver {
            if pos >= dv.len().saturating_sub(1) {
                break;
            }
            dv[pos] = b;
            pos += 1;
        }
        print_line(sel, &dv[..pos]);
        print_line(sel, b"------------------------------------------------");

        let mut reg_cert = 0u32;
        let mut tx_by_rate = 0i32;
        let mut tx_by_rate_en = 0i32;
        let mut tx_limit = 0i32;
        let mut tx_limit_en = 0i32;
        let mut adapt_en = 0i32;
        let mut adapt_mode = 0i32;
        unsafe {
            rtw_rust_debug_dump_drv_cfg_pre_num_banners(sel);
            rtw_rust_debug_drv_cfg_num_values(
                core::ptr::null_mut(),
                &mut reg_cert,
                &mut tx_by_rate,
                &mut tx_by_rate_en,
                &mut tx_limit,
                &mut tx_limit_en,
                &mut adapt_en,
                &mut adapt_mode,
                core::ptr::null_mut(),
            );
        }

        let mut cert_line = [0u8; 48];
        let cert_pfx = b"RTW_DEF_MODULE_REGULATORY_CERT=0x";
        cert_line[..cert_pfx.len()].copy_from_slice(cert_pfx);
        let mut cpos = cert_pfx.len();
        let byte = (reg_cert & 0xff) as u8;
        for nib in [byte >> 4, byte & 0xf] {
            cert_line[cpos] = if nib < 10 {
                b'0' + nib
            } else {
                b'a' + (nib - 10)
            };
            cpos += 1;
        }
        print_line(sel, &cert_line[..cpos]);
        print_kv(sel, b"CONFIG_TXPWR_BY_RATE=", tx_by_rate as u32);
        print_kv(sel, b"CONFIG_TXPWR_BY_RATE_EN=", tx_by_rate_en as u32);
        print_kv(sel, b"CONFIG_TXPWR_LIMIT=", tx_limit as u32);
        print_kv(sel, b"CONFIG_TXPWR_LIMIT_EN=", tx_limit_en as u32);
        unsafe {
            rtw_rust_debug_dump_drv_cfg_odm_minimal_banners(sel);
        }
        print_kv(sel, b"CONFIG_RTW_ADAPTIVITY_EN = ", adapt_en as u32);
        if adapt_en != 0 {
            if adapt_mode != 0 {
                print_line(sel, b"ADAPTIVITY_MODE = carrier_sense");
            } else {
                print_line(sel, b"ADAPTIVITY_MODE = normal");
            }
        }
        unsafe {
            rtw_rust_debug_dump_drv_cfg_post_num_banners(sel);
            rtw_rust_debug_dump_drv_cfg_tail(sel);
        }
    }
}

#[cfg(config_rust_rtw_debug_test_hooks)]
#[path = "rtw_debug_test_hooks.rs"]
mod test_hooks;
