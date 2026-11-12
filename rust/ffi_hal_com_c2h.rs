// SPDX-License-Identifier: GPL-2.0
//! Shared `extern "C"` imports for hal_com C2H Rust objects (FX-17).

extern "C" {
    pub fn rtw_rust_hal_com_warn_c2h_short_len(func: *const u8, len: u32, expect: i32);
}
