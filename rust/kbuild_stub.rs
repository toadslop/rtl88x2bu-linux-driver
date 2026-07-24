// SPDX-License-Identifier: GPL-2.0
//! Wave 0 Kbuild probe: proves a `.rs` object can link into `88x2bu.ko`.
//!
//! Linked only when the target kernel has `CONFIG_RUST=y` (see Makefile).
//! No C call site yet — that lands in W0-03 (`rust/scaffold.rs`).

/// Non-empty text symbol so `nm` can confirm the Rust object is in the module.
#[no_mangle]
pub extern "C" fn rtw_rust_kbuild_probe() -> i32 {
    0
}
