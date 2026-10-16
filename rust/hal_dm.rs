// SPDX-License-Identifier: GPL-2.0
//! HAL DM — Kbuild scaffold from `hal/hal_dm.c` (W4-29; W4-30 ports `Init_ODM_ComInfo`).

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

/// Link-time probe for `rust/hal_dm.o` (replaced by real exports in W4-30+).
#[no_mangle]
pub extern "C" fn rtw_rust_hal_dm_kbuild_probe() -> i32 {
    0
}
