// SPDX-License-Identifier: GPL-2.0
//! Recv leaf helpers — Rust port of `core/rtw_recv_rest.c` (W3-39).

#![allow(dead_code, improper_ctypes, missing_docs, non_camel_case_types, non_snake_case, non_upper_case_globals, unreachable_pub)]

#[cfg(host_recv_test)]
use std::os::raw::{c_int, c_uint, c_void};
#[cfg(not(host_recv_test))]
use core::ffi::{c_int, c_uint, c_void};

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const MAX_CONTINUAL_NORXPACKET_COUNT: c_int = 4;
const HDR_3ADDR_SZ: usize = 24;

#[cfg(host_recv_test)]
const TID_NUM: usize = 16;

#[cfg(host_recv_test)]
#[repr(C)]
pub struct StaInfo {
    pub continual_no_rx_packet: [c_int; TID_NUM],
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct RecvFrame {
    pub _pad: u32,
    pub len: c_uint,
    pub rx_data: *mut u8,
}

#[cfg(not(host_recv_test))]
mod kernel {
    use super::*;
    extern "C" {
        fn rtw_rust_recv_continual_no_rx(sta: *mut c_void, tid: c_int) -> *mut c_int;
        fn rtw_rust_atomic_inc_return(v: *mut c_int) -> c_int;
        fn rtw_rust_atomic_set(v: *mut c_int, val: c_int);
        fn rtw_rust_del_wfd_ie(ies: *mut u8, len: c_uint, msg: *const u8) -> c_uint;
        fn rtw_rust_recv_frame_rx_data(rframe: *mut c_void) -> *mut u8;
        fn rtw_rust_recv_frame_len(rframe: *mut c_void) -> c_uint;
        fn rtw_rust_recv_frame_set_len(rframe: *mut c_void, len: c_uint);
    }
    pub(super) unsafe fn continual_no_rx(sta: *mut c_void, tid: c_int) -> *mut c_int {
        unsafe { rtw_rust_recv_continual_no_rx(sta, tid) }
    }
    pub(super) unsafe fn atomic_inc(v: *mut c_int) -> c_int {
        unsafe { rtw_rust_atomic_inc_return(v) }
    }
    pub(super) unsafe fn atomic_set(v: *mut c_int, val: c_int) {
        unsafe { rtw_rust_atomic_set(v, val) };
    }
    pub(super) unsafe fn del_wfd_ie(ies: *mut u8, len: c_uint) -> c_uint {
        unsafe { rtw_rust_del_wfd_ie(ies, len, core::ptr::null()) }
    }
    pub(super) unsafe fn frame_rx_data(rframe: *mut c_void) -> *mut u8 {
        unsafe { rtw_rust_recv_frame_rx_data(rframe) }
    }
    pub(super) unsafe fn frame_len(rframe: *mut c_void) -> c_uint {
        unsafe { rtw_rust_recv_frame_len(rframe) }
    }
    pub(super) unsafe fn frame_set_len(rframe: *mut c_void, len: c_uint) {
        unsafe { rtw_rust_recv_frame_set_len(rframe, len) };
    }
}

#[cfg(host_recv_test)]
extern "C" {
    fn rtw_del_wfd_ie(ies: *mut u8, ies_len_ori: c_uint, msg: *const u8) -> c_uint;
}

#[no_mangle]
pub extern "C" fn rtw_inc_and_chk_continual_no_rx_packet(sta: *mut c_void, tid_index: c_int) -> c_int {
    if sta.is_null() || tid_index < 0 {
        return _FALSE;
    }
    #[cfg(host_recv_test)]
    {
        let tid = tid_index as usize;
        if tid >= TID_NUM {
            return _FALSE;
        }
        let sta = unsafe { &mut *(sta as *mut StaInfo) };
        let value = sta.continual_no_rx_packet[tid] + 1;
        sta.continual_no_rx_packet[tid] = value;
        return if value >= MAX_CONTINUAL_NORXPACKET_COUNT {
            _TRUE
        } else {
            _FALSE
        };
    }
    #[cfg(not(host_recv_test))]
    {
        let counter = unsafe { kernel::continual_no_rx(sta, tid_index) };
        if counter.is_null() {
            return _FALSE;
        }
        let value = unsafe { kernel::atomic_inc(counter) };
        if value >= MAX_CONTINUAL_NORXPACKET_COUNT {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_reset_continual_no_rx_packet(sta: *mut c_void, tid_index: c_int) {
    if sta.is_null() || tid_index < 0 {
        return;
    }
    #[cfg(host_recv_test)]
    {
        let tid = tid_index as usize;
        if tid < TID_NUM {
            unsafe { (&mut *(sta as *mut StaInfo)).continual_no_rx_packet[tid] = 0 };
        }
    }
    #[cfg(not(host_recv_test))]
    {
        let counter = unsafe { kernel::continual_no_rx(sta, tid_index) };
        if !counter.is_null() {
            unsafe { kernel::atomic_set(counter, 0) };
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rframe_del_wfd_ie(rframe: *mut c_void, ies_offset: u8) -> bool {
    if rframe.is_null() {
        return false;
    }
    let (rx_data, len) = {
        #[cfg(host_recv_test)]
        {
            let rf = unsafe { &mut *(rframe as *mut RecvFrame) };
            (rf.rx_data, rf.len)
        }
        #[cfg(not(host_recv_test))]
        {
            (unsafe { kernel::frame_rx_data(rframe) }, unsafe { kernel::frame_len(rframe) })
        }
    };
    if rx_data.is_null() {
        return false;
    }
    let ies = unsafe { rx_data.add(HDR_3ADDR_SZ + ies_offset as usize) };
    let ies_len_ori = len - (ies as u32 - rx_data as u32);
    let ies_len = {
        #[cfg(host_recv_test)]
        {
            unsafe { rtw_del_wfd_ie(ies, ies_len_ori, core::ptr::null()) }
        }
        #[cfg(not(host_recv_test))]
        {
            unsafe { kernel::del_wfd_ie(ies, ies_len_ori) }
        }
    };
    let new_len = len - (ies_len_ori - ies_len);
    #[cfg(host_recv_test)]
    {
        unsafe { (*(rframe as *mut RecvFrame)).len = new_len };
    }
    #[cfg(not(host_recv_test))]
    {
        unsafe { kernel::frame_set_len(rframe, new_len) };
    }
    ies_len_ori != ies_len
}
