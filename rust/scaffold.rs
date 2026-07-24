// SPDX-License-Identifier: GPL-2.0
//! Wave 0 scaffold: one `extern "C"` entry called from C module init.
//!
//! Linked only when the target kernel has `CONFIG_RUST=y` (see Makefile).
//! Called once from `rtw_drv_entry` (`os_dep/linux/usb_intf.c`).

/// Invoked from `rtw_drv_entry` so dmesg can show mixed C+Rust init ran.
///
/// Returns `0` on success. Keep this side-effect free beyond the call itself;
/// the C caller logs the breadcrumb (and any non-zero return).
#[no_mangle]
pub extern "C" fn rtw_rust_scaffold_init() -> core::ffi::c_int {
    0
}
