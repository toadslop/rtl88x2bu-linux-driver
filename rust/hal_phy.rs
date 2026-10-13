// SPDX-License-Identifier: GPL-2.0
//! HAL PHY helpers — Rust port of `hal/hal_phy.c` (W4-09 PHY_CalculateBitShift).

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

type U32 = u32;

#[no_mangle]
pub extern "C" fn PHY_CalculateBitShift(bit_mask: U32) -> U32 {
    for i in 0..=31 {
        if (bit_mask >> i) & 0x1 == 1 {
            return i;
        }
    }
    32
}
