// SPDX-License-Identifier: GPL-2.0
//! CCMP (CTR with CBC-MAC Protocol) — Rust port stub for L2 harness wiring (W2-09a).
//!
//! Full `extern "C"` shims land in W2-09/W2-10. This crate root exists so
//! `test-ccmp-rust` can link a staticlib today; the host runner still uses the
//! C oracle (`ccmp.c`) until the port replaces it.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

/// Kbuild / L2 probe — replaced when the real port lands.
#[no_mangle]
pub extern "C" fn rtw_rust_ccmp_probe() -> i32 {
    0
}
