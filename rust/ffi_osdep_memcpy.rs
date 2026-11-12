// SPDX-License-Identifier: GPL-2.0
//! `_rtw_memcpy` — single declaration per `include/osdep_service.h` (FX-17).

use core::ffi::c_void;

extern "C" {
    pub fn _rtw_memcpy(dst: *mut c_void, src: *const c_void, sz: u32);
}
