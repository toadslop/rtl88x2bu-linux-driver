// SPDX-License-Identifier: GPL-2.0
//! Host L2 staticlib: security CAM entry read (FX-12).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

extern "C" {
    fn host_sec_cam_read(addr: u8) -> u32;
}

fn rtw_sec_read_cam(_adapter: *mut core::ffi::c_void, addr: u8) -> u32 {
    unsafe { host_sec_cam_read(addr) }
}

fn rtw_sec_write_cam(_adapter: *mut core::ffi::c_void, _addr: u8, _wdata: u32) {}

include!("hal_com_sec_cam_ent.rs");
