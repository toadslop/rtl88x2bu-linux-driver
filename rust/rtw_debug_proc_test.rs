// SPDX-License-Identifier: GPL-2.0
// W3-168 proc manufacturing test setters (hook storage in rtw_debug_test_hooks.rs).

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
static mut PROC_STA_WAIT_MS: u32 = 0;
static mut PROC_STA_FORCE_FAIL: u8 = 0;
#[cfg(config_ap_mode)]
static mut PROC_AP_AUTH_FAIL: u16 = 0;
#[cfg(config_ap_mode)]
static mut PROC_AP_ASOC_FAIL: u16 = 0;

extern "C" {
    fn rtw_rust_debug_proc_copy_user_input(
        buffer: *const c_void,
        count: usize,
        tmp: *mut u8,
        tmp_size: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_sscanf_hhu_pair(tmp: *const u8, first: *mut u8, second: *mut u8) -> i32;
    fn rtw_rust_debug_proc_sscanf_hhu(tmp: *const u8, val: *mut u8) -> i32;
    fn rtw_rust_debug_proc_sscanf_u32(tmp: *const u8, val: *mut u32) -> i32;
    fn rtw_rust_debug_proc_sscanf_u32_hhu(tmp: *const u8, u: *mut u32, b: *mut u8) -> i32;
    fn rtw_rust_debug_proc_apply_fwdl_test(chksum: u8, wintint: u8);
    fn rtw_rust_debug_proc_apply_del_rx_ampdu_test(no_tx_fail: u8);
    fn rtw_rust_debug_proc_apply_wait_hiq_empty(wait_ms: u32);
    fn rtw_rust_debug_proc_apply_sta_linking_test(wait_ms: u32, force_fail: u8);
}

#[cfg(config_ap_mode)]
extern "C" {
    fn rtw_rust_debug_proc_sscanf_u16_pair(
        tmp: *const u8,
        first: *mut u16,
        second: *mut u16,
    ) -> i32;
    fn rtw_rust_debug_proc_apply_ap_linking_test(auth_fail: u16, asoc_fail: u16);
}

fn read_line(
    buffer: *const c_void,
    count: usize,
    tmp: &mut [u8; TMP_SIZE],
) -> Result<usize, c_long> {
    if count < 1 {
        return Err(EFAULT);
    }
    if buffer.is_null() {
        return Ok(0);
    }
    let copied =
        unsafe { rtw_rust_debug_proc_copy_user_input(buffer, count, tmp.as_mut_ptr(), TMP_SIZE) };
    if copied < 0 {
        return Err(copied);
    }
    Ok(copied as usize)
}

macro_rules! proc_set {
    ($fn:ident, $handler:expr) => {
        #[cfg(any(config_rtw_debug, config_proc_debug))]
        #[no_mangle]
        pub extern "C" fn $fn(
            _file: *mut c_void,
            buffer: *const c_void,
            count: usize,
            _pos: *mut c_long,
            _data: *mut c_void,
        ) -> c_long {
            let mut tmp = [0u8; TMP_SIZE];
            let n = match read_line(buffer, count, &mut tmp) {
                Ok(n) => n,
                Err(e) => return e,
            };
            if n > 0 {
                $handler(tmp.as_ptr());
            }
            count as c_long
        }
    };
}

proc_set!(proc_set_fwdl_test_case, |tmp: *const u8| {
    let mut chksum = 0u8;
    let mut wintint = 0u8;
    let num = unsafe { rtw_rust_debug_proc_sscanf_hhu_pair(tmp, &mut chksum, &mut wintint) };
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
});

proc_set!(proc_set_del_rx_ampdu_test_case, |tmp: *const u8| {
    let mut no_tx_fail = 0u8;
    let num = unsafe { rtw_rust_debug_proc_sscanf_hhu(tmp, &mut no_tx_fail) };
    if num >= 1 {
        unsafe { rtw_rust_debug_proc_apply_del_rx_ampdu_test(no_tx_fail) };
    }
});

proc_set!(proc_set_wait_hiq_empty, |tmp: *const u8| {
    let mut wait_ms = 0u32;
    let num = unsafe { rtw_rust_debug_proc_sscanf_u32(tmp, &mut wait_ms) };
    if num >= 1 {
        unsafe { rtw_rust_debug_proc_apply_wait_hiq_empty(wait_ms) };
    }
});

proc_set!(proc_set_sta_linking_test, |tmp: *const u8| {
    let mut wait_ms = 0u32;
    let mut force_fail = 0u8;
    let num = unsafe { rtw_rust_debug_proc_sscanf_u32_hhu(tmp, &mut wait_ms, &mut force_fail) };
    if num >= 1 {
        unsafe {
            PROC_STA_WAIT_MS = wait_ms;
        }
    }
    if num >= 2 {
        unsafe {
            PROC_STA_FORCE_FAIL = force_fail;
        }
    }
    if num >= 1 {
        let (wait_ms, force_fail) = unsafe { (PROC_STA_WAIT_MS, PROC_STA_FORCE_FAIL) };
        unsafe { rtw_rust_debug_proc_apply_sta_linking_test(wait_ms, force_fail) };
    }
});

#[cfg(config_ap_mode)]
proc_set!(proc_set_ap_linking_test, |tmp: *const u8| {
    let mut auth = 0u16;
    let mut asoc = 0u16;
    let num = unsafe { rtw_rust_debug_proc_sscanf_u16_pair(tmp, &mut auth, &mut asoc) };
    if num >= 1 {
        unsafe {
            PROC_AP_AUTH_FAIL = auth;
        }
    }
    if num >= 2 {
        unsafe {
            PROC_AP_ASOC_FAIL = asoc;
        }
    }
    if num >= 1 {
        let (auth, asoc) = unsafe { (PROC_AP_AUTH_FAIL, PROC_AP_ASOC_FAIL) };
        unsafe { rtw_rust_debug_proc_apply_ap_linking_test(auth, asoc) };
    }
});
