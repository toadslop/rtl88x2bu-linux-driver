// SPDX-License-Identifier: GPL-2.0
//! Allowlisted C bindings for the Wave 1 crypto pilot (W1-01).
//!
//! Regenerated with `./scripts/bindgen_rtw.sh`. Linked only when the target
//! kernel has `CONFIG_RUST=y` (see Makefile). Future `rust/ffi` (W1-02) will
//! re-export these; crypto TUs may also `include!` the generated file.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unused_imports
)]

include!("bindings/generated.rs");

/// Link-time probe: returns `AES_BLOCK_SIZE` from the generated bindings.
///
/// Confirms the bindings object is present in `88x2bu.ko` (`nm` / L1).
#[no_mangle]
pub extern "C" fn rtw_rust_bindings_probe() -> core::ffi::c_int {
    AES_BLOCK_SIZE as core::ffi::c_int
}
