// SPDX-License-Identifier: GPL-2.0
//! GCMP (GCM with GMAC Protocol) — Rust port of `core/crypto/gcmp.c` (W2-02d).
//!
//! Crate root for kbuild; shared logic lives in `gcmp_support.rs`. Decrypt/encrypt
//! ABI shims land in follow-up PRs (W2-02e/W2-02f).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[path = "gcmp_support.rs"]
mod support;

use support::AES_BLOCK_SIZE;

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_gcmp_probe() -> core::ffi::c_int {
    AES_BLOCK_SIZE as core::ffi::c_int
}
