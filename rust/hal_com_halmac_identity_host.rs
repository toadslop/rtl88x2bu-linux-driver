// SPDX-License-Identifier: GPL-2.0
// Host staticlib wrapper for W4-135 HALMAC identity L2.

#![allow(dead_code, non_snake_case, non_camel_case_types, non_upper_case_globals)]

#[repr(C)]
struct HostAdapter {
    hw_port: u8,
    clt_port: u8,
    dvobj: *mut core::ffi::c_void,
}

#[no_mangle]
pub extern "C" fn host_halmac_identity_adapter_to_dvobj(adapter: *mut core::ffi::c_void) -> *mut core::ffi::c_void {
    unsafe { (*(adapter as *mut HostAdapter)).dvobj }
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_com_get_hw_port(adapter: *mut core::ffi::c_void) -> u8 {
    unsafe { (*(adapter as *mut HostAdapter)).hw_port }
}

include!("hal_com_halmac_identity.rs");