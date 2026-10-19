// SPDX-License-Identifier: GPL-2.0
//! W3-143 MAC/BB register dump leaves from `core/rtw_debug_rest.c`.

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

type U32 = u32;

extern "C" {
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_print_sel_cont(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_read32(adapter: *mut c_void, addr: U32) -> U32;
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

fn print_bytes_cont(sel: *mut c_void, bytes: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(bytes.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = bytes[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel_cont(sel, buf.as_ptr());
    }
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

fn push_hex_nib(out: &mut [u8], pos: &mut usize, nib: u8) {
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

fn push_u16_hex4(out: &mut [u8], pos: &mut usize, v: u16) {
    for shift in [12i32, 8, 4, 0] {
        push_hex_nib(out, pos, ((v >> shift) & 0xf) as u8);
    }
}

fn push_u32_hex8(out: &mut [u8], pos: &mut usize, v: U32) {
    for shift in [28i32, 24, 20, 16, 12, 8, 4, 0] {
        push_hex_nib(out, pos, ((v >> shift) & 0xf) as u8);
    }
}

fn dump_reg_window(sel: *mut c_void, adapter: *mut c_void, start: U32, end: U32, j: &mut i32) {
    let mut line = [0u8; 64];
    let mut i = start;
    while i < end {
        if *j % 4 == 1 {
            let mut pos = 0usize;
            push_str(&mut line, &mut pos, b"0x");
            push_u16_hex4(&mut line, &mut pos, i as u16);
            print_bytes(sel, &line[..pos]);
        }
        let mut pos = 0usize;
        push_str(&mut line, &mut pos, b" 0x");
        let val = unsafe { rtw_rust_debug_read32(adapter, i) };
        push_u32_hex8(&mut line, &mut pos, val);
        push_byte(&mut line, &mut pos, b' ');
        print_bytes_cont(sel, &line[..pos]);
        if *j % 4 == 0 {
            print_bytes_cont(sel, b"\n");
        }
        *j += 1;
        i += 4;
    }
}

#[no_mangle]
pub extern "C" fn mac_reg_dump(sel: *mut c_void, adapter: *mut c_void) {
    let mut j = 1i32;
    print_bytes(sel, b"======= MAC REG =======\n");
    dump_reg_window(sel, adapter, 0x0, 0x800, &mut j);

    #[cfg(CONFIG_RTL8814A)]
    {
        dump_reg_window(sel, adapter, 0x1000, 0x1650, &mut j);
    }

    #[cfg(any(
        CONFIG_RTL8822B,
        CONFIG_RTL8821C,
        CONFIG_RTL8822C,
        CONFIG_RTL8814B,
        CONFIG_RTL8723F
    ))]
    {
        dump_reg_window(sel, adapter, 0x1000, 0x1800, &mut j);
    }

    #[cfg(CONFIG_RTL8192F)]
    {
        dump_reg_window(sel, adapter, 0x1000, 0x1100, &mut j);
        dump_reg_window(sel, adapter, 0x1300, 0x1360, &mut j);
    }

    #[cfg(CONFIG_RTL8814B)]
    {
        dump_reg_window(sel, adapter, 0x2000, 0x2800, &mut j);
        dump_reg_window(sel, adapter, 0x3000, 0x3800, &mut j);
    }
}

#[no_mangle]
pub extern "C" fn bb_reg_dump(sel: *mut c_void, adapter: *mut c_void) {
    let mut j = 1i32;
    print_bytes(sel, b"======= BB REG =======\n");
    dump_reg_window(sel, adapter, 0x800, 0x1000, &mut j);

    #[cfg(any(
        CONFIG_RTL8822B,
        CONFIG_RTL8821C,
        CONFIG_RTL8822C,
        CONFIG_RTL8814B,
        CONFIG_RTL8723F
    ))]
    {
        dump_reg_window(sel, adapter, 0x1800, 0x2000, &mut j);
    }

    #[cfg(any(CONFIG_RTL8822C, CONFIG_RTL8814B, CONFIG_RTL8723F))]
    {
        dump_reg_window(sel, adapter, 0x2c00, 0x2c60, &mut j);
        dump_reg_window(sel, adapter, 0x2d00, 0x2df0, &mut j);
        dump_reg_window(sel, adapter, 0x4000, 0x4060, &mut j);
        dump_reg_window(sel, adapter, 0x4100, 0x4200, &mut j);
    }

    #[cfg(CONFIG_RTL8814B)]
    {
        dump_reg_window(sel, adapter, 0x5200, 0x5400, &mut j);
    }

    #[cfg(CONFIG_RTL8723F)]
    {
        dump_reg_window(sel, adapter, 0x4300, 0x43c0, &mut j);
    }
}
