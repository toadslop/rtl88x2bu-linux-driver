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
    fn rtw_rust_debug_proc_sscanf_hhu_pair(tmp: *const u8, first: *mut u8, second: *mut u8) -> i32;
    fn rtw_rust_debug_proc_apply_fwdl_test(chksum: u8, wintint: u8);
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
    if copied == 0 {
        return count as c_long;
    }
    let mut chksum = 0u8;
    let mut wintint = 0u8;
    let num = unsafe {
        rtw_rust_debug_proc_sscanf_hhu_pair(tmp.as_ptr(), &mut chksum, &mut wintint)
    };
    if num >= 1 {
        unsafe {
            PROC_FWDl_CHKSUM = chksum;
        }
    }
    if num >= 2 {
        unsafe {
            PROC_FWDl_WINT = wintint;
        }
    }
    if num >= 1 {
        let (chksum, wintint) = unsafe { (PROC_FWDl_CHKSUM, PROC_FWDl_WINT) };
        unsafe { rtw_rust_debug_proc_apply_fwdl_test(chksum, wintint) };
    }
    count as c_long
}
