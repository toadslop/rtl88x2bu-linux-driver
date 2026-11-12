// SPDX-License-Identifier: GPL-2.0
//! Shared systime shims from `hal/hal_com_rest.c` (FX-17).

pub type Systime = core::ffi::c_ulong;

extern "C" {
    pub fn rtw_rust_hal_com_get_current_time() -> Systime;
    pub fn rtw_rust_hal_com_get_passing_time_ms(start: Systime) -> u32;
}
