// SPDX-License-Identifier: GPL-2.0
// W3-168 PR1: proc_set_fwdl_test_case (remaining setters follow in PR2).

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

use core::ffi::{c_long, c_void};

const TMP_SIZE: usize = 32;
const EFAULT: c_long = -14;

static mut PROC_FWDl_CHKSUM: u8 = 0;
static mut PROC_FWDl_WINT: u8 = 0;

extern "C" {
    fn rtw_rust_debug_proc_copy_user_input(
        buffer: *const c_void,
        count: usize,
        tmp: *mut u8,
        tmp_size: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_apply_fwdl_test(chksum: u8, wintint: u8);
}

fn ws(b: u8) -> bool {
    b == b' ' || b == b'\t' || b == b'\n' || b == b'\r'
}

fn token_at(line: &[u8], idx: usize) -> Option<&[u8]> {
    let mut i = 0usize;
    let mut slot = 0usize;
    while i < line.len() {
        while i < line.len() && ws(line[i]) {
            i += 1;
        }
        if i >= line.len() {
            return None;
        }
        let start = i;
        while i < line.len() && !ws(line[i]) {
            i += 1;
        }
        if slot == idx {
            return Some(&line[start..i]);
        }
        slot += 1;
    }
    None
}

fn parse_u8(tok: &[u8]) -> Option<u8> {
    let mut val = 0u32;
    for &b in tok {
        if !b.is_ascii_digit() {
            return None;
        }
        val = val * 10 + (b - b'0') as u32;
        if val > u8::MAX as u32 {
            return None;
        }
    }
    Some(val as u8)
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_fwdl_test_case(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    _data: *mut c_void,
) -> c_long {
    if count < 1 {
        return EFAULT;
    }
    if buffer.is_null() {
        return count as c_long;
    }
    let mut tmp = [0u8; TMP_SIZE];
    let copied =
        unsafe { rtw_rust_debug_proc_copy_user_input(buffer, count, tmp.as_mut_ptr(), TMP_SIZE) };
    if copied < 0 {
        return copied;
    }
    let line = &tmp[..copied as usize];
    let mut num = 0i32;
    if let Some(tok) = token_at(line, 0) {
        if let Some(v) = parse_u8(tok) {
            unsafe {
                PROC_FWDl_CHKSUM = v;
            }
            num = 1;
        }
    }
    if let Some(tok) = token_at(line, 1) {
        if let Some(v) = parse_u8(tok) {
            unsafe {
                PROC_FWDl_WINT = v;
            }
            num = 2;
        }
    }
    if num >= 1 {
        let (chksum, wintint) = unsafe { (PROC_FWDl_CHKSUM, PROC_FWDl_WINT) };
        unsafe { rtw_rust_debug_proc_apply_fwdl_test(chksum, wintint) };
    }
    count as c_long
}
