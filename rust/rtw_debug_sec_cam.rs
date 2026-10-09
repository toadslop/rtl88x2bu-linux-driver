// SPDX-License-Identifier: GPL-2.0
//! W3-133 security CAM debug dump helpers.

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

const SEC_TYPE_256: U8 = 0x10;

#[repr(C)]
pub struct SecCamEnt {
    pub ctrl: U16,
    pub mac: [U8; 6],
    pub key: [U8; 16],
}

extern "C" {
    fn security_type_str(value: U8) -> *const c_char;
}

#[cfg(not(host_rtw_debug_sec_cam_test))]
extern "C" {
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
}

fn c_str_bytes(ptr: *const c_char) -> &'static [u8] {
    if ptr.is_null() {
        return b"(null)";
    }
    unsafe {
        let mut len = 0usize;
        while *ptr.add(len) != 0 {
            len += 1;
        }
        core::slice::from_raw_parts(ptr as *const u8, len)
    }
}

fn push_byte(out: &mut [u8], pos: &mut usize, b: U8) {
    if *pos < out.len() {
        out[*pos] = b;
        *pos += 1;
    }
}

fn push_hex_byte(out: &mut [u8], pos: &mut usize, b: U8) {
    for nib in [b >> 4, b & 0xf] {
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

fn push_u16_hex(out: &mut [u8], pos: &mut usize, v: U16) {
    push_byte(out, pos, b'0');
    push_byte(out, pos, b'x');
    for shift in [12i32, 8, 4, 0] {
        let nib = ((v >> shift) & 0xf) as U8;
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

fn push_dec(out: &mut [u8], pos: &mut usize, v: u32, width: usize) {
    let mut digits = [0u8; 10];
    let mut n = 0usize;
    let mut x = v;
    if x == 0 {
        digits[0] = b'0';
        n = 1;
    } else {
        while x > 0 && n < digits.len() {
            digits[n] = b'0' + (x % 10) as u8;
            x /= 10;
            n += 1;
        }
    }
    for _ in n..width {
        push_byte(out, pos, b' ');
    }
    for i in 0..n {
        push_byte(out, pos, digits[n - 1 - i]);
    }
}

fn sec_type_label(ctrl: U16) -> &'static [u8] {
    let ty = if (ctrl & 0x200) != 0 {
        (((ctrl >> 2) & 0x7) as U8) | SEC_TYPE_256
    } else {
        ((ctrl >> 2) & 0x7) as U8
    };
    c_str_bytes(unsafe { security_type_str(ty) })
}

fn format_sec_cam_ent_line(ent: &SecCamEnt, id: Option<i32>, out: &mut [u8]) -> usize {
    let mut pos = 0usize;
    if let Some(id_val) = id {
        push_dec(out, &mut pos, id_val as u32, 2);
        push_byte(out, &mut pos, b' ');
    }
    push_u16_hex(out, &mut pos, ent.ctrl);
    push_byte(out, &mut pos, b' ');
    for i in 0..6 {
        push_hex_byte(out, &mut pos, ent.mac[i]);
        if i + 1 < 6 {
            push_byte(out, &mut pos, b':');
        }
    }
    push_byte(out, &mut pos, b' ');
    for i in 0..16 {
        push_hex_byte(out, &mut pos, ent.key[i]);
    }
    push_byte(out, &mut pos, b' ');
    push_dec(out, &mut pos, (ent.ctrl & 0x03) as u32, 3);
    push_byte(out, &mut pos, b' ');
    let label = sec_type_label(ent.ctrl);
    for &b in label {
        push_byte(out, &mut pos, b);
    }
    for _ in label.len()..8 {
        push_byte(out, &mut pos, b' ');
    }
    push_byte(out, &mut pos, b' ');
    push_dec(out, &mut pos, ((ent.ctrl >> 5) & 0x01) as u32, 2);
    push_byte(out, &mut pos, b' ');
    push_dec(out, &mut pos, ((ent.ctrl >> 6) & 0x01) as u32, 2);
    push_byte(out, &mut pos, b' ');
    push_dec(out, &mut pos, ((ent.ctrl >> 15) & 0x01) as u32, 5);
    pos
}

fn format_title(has_id: bool, out: &mut [u8]) -> usize {
    let mut pos = 0usize;
    if has_id {
        out[..3].copy_from_slice(b"id ");
        pos = 3;
    }
    let hdr = b"ctrl   addr              key                              kid type     MK GK valid";
    let n = core::cmp::min(hdr.len(), out.len() - pos);
    out[pos..pos + n].copy_from_slice(&hdr[..n]);
    pos + n
}

#[cfg(not(host_rtw_debug_sec_cam_test))]
fn emit_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel(sel, buf.as_ptr());
    }
}

#[no_mangle]
pub extern "C" fn dump_sec_cam_ent(sel: *mut c_void, ent: *mut SecCamEnt, id: c_int) {
    if ent.is_null() {
        return;
    }
    let ent = unsafe { &*ent };
    let mut line = [0u8; 200];
    let n = format_sec_cam_ent_line(ent, if id >= 0 { Some(id) } else { None }, &mut line);
    let mut with_nl = [0u8; 210];
    with_nl[..n].copy_from_slice(&line[..n]);
    with_nl[n] = b'\n';
    #[cfg(not(host_rtw_debug_sec_cam_test))]
    emit_line(sel, &with_nl[..n + 1]);
}

#[no_mangle]
pub extern "C" fn dump_sec_cam_ent_title(sel: *mut c_void, has_id: U8) {
    let mut line = [0u8; 120];
    let n = format_title(has_id != 0, &mut line);
    let mut with_nl = [0u8; 128];
    with_nl[..n].copy_from_slice(&line[..n]);
    with_nl[n] = b'\n';
    #[cfg(not(host_rtw_debug_sec_cam_test))]
    emit_line(sel, &with_nl[..n + 1]);
}

#[no_mangle]
pub extern "C" fn dump_sec_cam_ent_format(
    ent: *mut SecCamEnt,
    id: c_int,
    out: *mut u8,
    buflen: usize,
) -> c_int {
    if ent.is_null() || out.is_null() || buflen == 0 {
        return -1;
    }
    let ent = unsafe { &*ent };
    let buf = unsafe { core::slice::from_raw_parts_mut(out, buflen) };
    let n = format_sec_cam_ent_line(ent, if id >= 0 { Some(id) } else { None }, buf);
    if n >= buflen {
        return -1;
    }
    buf[n] = 0;
    n as c_int
}

#[no_mangle]
pub extern "C" fn dump_sec_cam_ent_title_format(has_id: U8, out: *mut u8, buflen: usize) -> c_int {
    if out.is_null() || buflen == 0 {
        return -1;
    }
    let buf = unsafe { core::slice::from_raw_parts_mut(out, buflen) };
    let n = format_title(has_id != 0, buf);
    if n >= buflen {
        return -1;
    }
    buf[n] = 0;
    n as c_int
}
