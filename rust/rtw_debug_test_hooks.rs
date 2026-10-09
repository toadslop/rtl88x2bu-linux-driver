// SPDX-License-Identifier: GPL-2.0
//! W3-132 manufacturing/test hook globals — Rust port from `core/rtw_debug_rest.c`.

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

#[cfg(not(host_rtw_debug_test_hooks_test))]
use core::ffi::c_ulong;
#[cfg(host_rtw_debug_test_hooks_test)]
use std::os::raw::c_ulong;

#[cfg(not(host_rtw_debug_test_hooks_test))]
use core::sync::atomic::{AtomicU16, AtomicU32, AtomicU8, Ordering};
#[cfg(host_rtw_debug_test_hooks_test)]
use std::sync::atomic::{AtomicU16, AtomicU32, AtomicU8, Ordering};

type U8 = u8;
type U16 = u16;
type U32 = u32;
type Systime = c_ulong;

static FWDl_CHKSUM: AtomicU8 = AtomicU8::new(0);
static FWDl_WINT: AtomicU8 = AtomicU8::new(0);
static DEL_RX_NO_TX: AtomicU8 = AtomicU8::new(0);
static WAIT_HIQ_MS: AtomicU32 = AtomicU32::new(0);
static STA_LINK_WAIT_MS: AtomicU32 = AtomicU32::new(0);
static STA_LINK_FORCE_FAIL: AtomicU8 = AtomicU8::new(0);
static AP_AUTH_FAIL: AtomicU16 = AtomicU16::new(0);
static AP_ASOC_FAIL: AtomicU16 = AtomicU16::new(0);

static mut STA_LINK_START: Systime = 0;

#[cfg(host_rtw_debug_test_hooks_test)]
static HOST_PASSING_MS: AtomicU32 = AtomicU32::new(0);

extern "C" {
    fn rtw_rust_debug_test_print(msg: *const u8);
}

#[cfg(not(host_rtw_debug_test_hooks_test))]
extern "C" {
    fn _rtw_get_current_time() -> Systime;
    fn _rtw_get_passing_time_ms(start: Systime) -> i32;
}

fn test_print(msg: &[u8]) {
    unsafe {
        rtw_rust_debug_test_print(msg.as_ptr());
    }
}

fn current_time() -> Systime {
    #[cfg(host_rtw_debug_test_hooks_test)]
    {
        return 0;
    }
    #[cfg(not(host_rtw_debug_test_hooks_test))]
    {
        unsafe { _rtw_get_current_time() }
    }
}

fn passing_time_ms(start: Systime) -> U32 {
    #[cfg(host_rtw_debug_test_hooks_test)]
    {
        let _ = start;
        return HOST_PASSING_MS.load(Ordering::Relaxed);
    }
    #[cfg(not(host_rtw_debug_test_hooks_test))]
    {
        unsafe { _rtw_get_passing_time_ms(start) as U32 }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_debug_set_fwdl_test_case(chksum: U8, wintint: U8) {
    FWDl_CHKSUM.store(chksum, Ordering::Relaxed);
    FWDl_WINT.store(wintint, Ordering::Relaxed);
}

#[no_mangle]
pub extern "C" fn rtw_rust_debug_set_del_rx_ampdu_test_no_tx_fail(v: U8) {
    DEL_RX_NO_TX.store(v, Ordering::Relaxed);
}

#[no_mangle]
pub extern "C" fn rtw_rust_debug_set_wait_hiq_empty_ms(ms: U32) {
    WAIT_HIQ_MS.store(ms, Ordering::Relaxed);
}

#[no_mangle]
pub extern "C" fn rtw_rust_debug_set_sta_linking_test(wait_ms: U32, force_fail: U8) {
    STA_LINK_WAIT_MS.store(wait_ms, Ordering::Relaxed);
    STA_LINK_FORCE_FAIL.store(force_fail, Ordering::Relaxed);
}

#[no_mangle]
pub extern "C" fn rtw_rust_debug_set_ap_linking_test(auth_fail: U16, asoc_fail: U16) {
    AP_AUTH_FAIL.store(auth_fail, Ordering::Relaxed);
    AP_ASOC_FAIL.store(asoc_fail, Ordering::Relaxed);
}

#[cfg(host_rtw_debug_test_hooks_test)]
#[no_mangle]
pub extern "C" fn host_debug_test_hooks_set_passing_ms(ms: U32) {
    HOST_PASSING_MS.store(ms, Ordering::Relaxed);
}

#[no_mangle]
pub extern "C" fn rtw_fwdl_test_trigger_chksum_fail() -> bool {
    let v = FWDl_CHKSUM.load(Ordering::Relaxed);
    if v != 0 {
        test_print(b"fwdl test case: trigger chksum_fail\n");
        FWDl_CHKSUM.store(v - 1, Ordering::Relaxed);
        return true;
    }
    false
}

#[no_mangle]
pub extern "C" fn rtw_fwdl_test_trigger_wintint_rdy_fail() -> bool {
    let v = FWDl_WINT.load(Ordering::Relaxed);
    if v != 0 {
        test_print(b"fwdl test case: trigger wintint_rdy_fail\n");
        FWDl_WINT.store(v - 1, Ordering::Relaxed);
        return true;
    }
    false
}

#[no_mangle]
pub extern "C" fn rtw_del_rx_ampdu_test_trigger_no_tx_fail() -> bool {
    let v = DEL_RX_NO_TX.load(Ordering::Relaxed);
    if v != 0 {
        test_print(b"del_rx_ampdu test case: trigger no_tx_fail\n");
        DEL_RX_NO_TX.store(v - 1, Ordering::Relaxed);
        return true;
    }
    false
}

#[no_mangle]
pub extern "C" fn rtw_get_wait_hiq_empty_ms() -> U32 {
    WAIT_HIQ_MS.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn rtw_sta_linking_test_set_start() {
    unsafe {
        STA_LINK_START = current_time();
    }
}

#[no_mangle]
pub extern "C" fn rtw_sta_linking_test_wait_done() -> bool {
    passing_time_ms(unsafe { STA_LINK_START }) >= STA_LINK_WAIT_MS.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn rtw_sta_linking_test_force_fail() -> bool {
    STA_LINK_FORCE_FAIL.load(Ordering::Relaxed) != 0
}

#[no_mangle]
pub extern "C" fn rtw_ap_linking_test_force_auth_fail() -> U16 {
    AP_AUTH_FAIL.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn rtw_ap_linking_test_force_asoc_fail() -> U16 {
    AP_ASOC_FAIL.load(Ordering::Relaxed)
}
