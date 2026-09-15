// SPDX-License-Identifier: GPL-2.0
//! W3-79 `_rtw_free_sta_priv` kernel entry (C body shim until full Rust port).

#![allow(
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals
)]

pub type StaPriv = core::ffi::c_void;

extern "C" {
    fn rtw_rust_free_sta_priv_body(stapriv: *mut StaPriv) -> u32;
}

#[no_mangle]
pub extern "C" fn _rtw_free_sta_priv(stapriv: *mut StaPriv) -> u32 {
    unsafe { rtw_rust_free_sta_priv_body(stapriv) }
}
