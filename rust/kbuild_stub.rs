// SPDX-License-Identifier: GPL-2.0
//! Wave 0 Kbuild probe: proves a `.rs` object can link into `88x2bu.ko`.
//!
//! Linked only when the target kernel has `CONFIG_RUST=y` (see Makefile).
//! Link-only probe (no C call site). The init call lives in `rust/scaffold.rs` (W0-03).

/// Non-empty text symbol so `nm` can confirm the Rust object is in the module.
#[no_mangle]
pub extern "C" fn rtw_rust_kbuild_probe() -> i32 {
    0
}
