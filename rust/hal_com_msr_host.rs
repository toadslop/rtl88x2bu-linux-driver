// SPDX-License-Identifier: GPL-2.0
// Host staticlib wrapper for W4-134 MSR L2.

#![allow(dead_code, non_snake_case, non_camel_case_types, non_upper_case_globals)]

extern "C" {
    fn host_msr_adapter_to_dvobj(adapter: *mut core::ffi::c_void) -> *mut core::ffi::c_void;
}

#[repr(C)]
struct HostAdapter {
    hw_port: u8,
    dvobj: *mut core::ffi::c_void,
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut core::ffi::c_void) -> *mut core::ffi::c_void {
    unsafe { host_msr_adapter_to_dvobj(adapter) }
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_com_msr_hw_port(adapter: *mut core::ffi::c_void) -> u8 {
    unsafe { (*(adapter as *mut HostAdapter)).hw_port }
}

include!("hal_com_msr.rs");