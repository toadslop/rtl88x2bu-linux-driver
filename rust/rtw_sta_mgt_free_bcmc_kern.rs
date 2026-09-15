// SPDX-License-Identifier: GPL-2.0
//! W3-79 `rtw_init_bcmc_stainfo` kernel entry (C body shim until full Rust port).

#![allow(
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals
)]

pub type Adapter = core::ffi::c_void;

extern "C" {
    fn rtw_rust_init_bcmc_stainfo_body(padapter: *mut Adapter) -> u32;
}

#[no_mangle]
pub extern "C" fn rtw_init_bcmc_stainfo(padapter: *mut Adapter) -> u32 {
    unsafe { rtw_rust_init_bcmc_stainfo_body(padapter) }
}
