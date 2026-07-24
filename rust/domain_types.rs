// SPDX-License-Identifier: GPL-2.0
//! Domain types crate root (A1): linked into `88x2bu.ko` when `CONFIG_RUST=y`.
//!
//! W1-03+ crypto ports import these types at their Rust API; `extern "C"` shims
//! call `try_from_slice` / `as_bytes` at the ABI edge and return `-1` on
//! `DomainError` the same way C rejects invalid `key_len`.

#![allow(missing_docs)]

#[path = "domain/types.rs"]
mod types;

pub use types::*;

/// Link-time probe: returns `AesCtrNonce::SIZE` so L1 can confirm this object
/// is present alongside the Wave 0/1 Rust crates.
#[no_mangle]
pub extern "C" fn rtw_rust_domain_types_probe() -> core::ffi::c_int {
  AesCtrNonce::SIZE as core::ffi::c_int
}
