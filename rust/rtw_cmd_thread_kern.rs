// SPDX-License-Identifier: GPL-2.0
//! W3-91 PR5: kernel `rtw_cmd_thread` loop control in Rust (dispatch via C shims).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

const RTW_RUST_CMD_TH_BREAK: i32 = 0;
const RTW_RUST_CMD_TH_OUTER: i32 = 1;
const RTW_RUST_CMD_TH_INNER: i32 = 2;

extern "C" {
    fn rtw_rust_cmd_thread_start(padapter: *mut c_void);
    fn rtw_rust_cmd_thread_wait_event(padapter: *mut c_void) -> i32;
    fn rtw_rust_cmd_thread_once(padapter: *mut c_void) -> i32;
    fn rtw_rust_cmd_thread_stop(padapter: *mut c_void);
}

/// Kernel cmd thread entry — outer wait/dequeue loops in Rust; per-cmd work in C shims.
#[no_mangle]
pub extern "C" fn rtw_cmd_thread(context: *mut c_void) -> i32 {
    if context.is_null() {
        return 0;
    }
    unsafe {
        rtw_rust_cmd_thread_start(context);
        loop {
            let wait = rtw_rust_cmd_thread_wait_event(context);
            if wait == RTW_RUST_CMD_TH_BREAK {
                break;
            }
            if wait == RTW_RUST_CMD_TH_OUTER {
                continue;
            }
            loop {
                let step = rtw_rust_cmd_thread_once(context);
                if step == RTW_RUST_CMD_TH_BREAK {
                    rtw_rust_cmd_thread_stop(context);
                    return 0;
                }
                if step == RTW_RUST_CMD_TH_OUTER {
                    break;
                }
            }
        }
        rtw_rust_cmd_thread_stop(context);
    }
    0
}
