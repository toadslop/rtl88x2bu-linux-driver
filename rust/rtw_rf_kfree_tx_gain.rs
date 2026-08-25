// SPDX-License-Identifier: GPL-2.0
//! W3-59 kfree TX gain get helper — Rust port of `core/rtw_rf_kfree_tx_gain.c` (PR3).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe,
    unused_variables,
    static_mut_refs
)]

#[cfg(not(host_rf_kfree_tx_gain_test))]
use core::ffi::c_void;

const KFREE_FLAG_ON: u8 = 1 << 0;

const BB_GAIN_2G: i32 = 0;
const BB_GAIN_NUM: i32 = 6;

const RF_PATH_MAX: usize = 4;

#[repr(C)]
struct KfreeDataT {
    flag: u8,
    bb_gain: [[i8; RF_PATH_MAX]; BB_GAIN_NUM as usize],
}

#[cfg(host_rf_kfree_tx_gain_test)]
#[repr(C)]
struct HalDataT {
    kfree_data: KfreeDataT,
}

#[cfg(host_rf_kfree_tx_gain_test)]
#[repr(C)]
pub struct Adapter {
    hal_data: HalDataT,
}

#[cfg(not(host_rf_kfree_tx_gain_test))]
pub type Adapter = c_void;

extern "C" {
    fn rtw_ch_to_bb_gain_sel(ch: i32) -> i32;
}

#[cfg(host_rf_kfree_tx_gain_test)]
unsafe fn kfree_data_ptr(adapter: *mut Adapter) -> *mut KfreeDataT {
    unsafe { &mut (*adapter).hal_data.kfree_data }
}

#[cfg(not(host_rf_kfree_tx_gain_test))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_kfree_get_kfree_data(adapter: *mut Adapter) -> *mut KfreeDataT;
        pub fn rtw_rust_kfree_warn_on(cond: bool);
    }
}

#[no_mangle]
pub extern "C" fn rtw_rf_get_kfree_tx_gain_offset(
    padapter: *mut Adapter,
    path: u8,
    ch: u8,
) -> i8 {
    #[cfg(any(host_rf_kfree_tx_gain_test, rf_power_trim))]
    {
        if padapter.is_null() {
            return 0;
        }
        let mut kfree_offset = 0i8;
        unsafe {
            #[cfg(host_rf_kfree_tx_gain_test)]
            let kfree_data = kfree_data_ptr(padapter);
            #[cfg(not(host_rf_kfree_tx_gain_test))]
            let kfree_data = kernel::rtw_rust_kfree_get_kfree_data(padapter);

            if kfree_data.is_null() {
                return 0;
            }

            let bb_gain_sel = rtw_ch_to_bb_gain_sel(ch as i32);
            if bb_gain_sel < BB_GAIN_2G || bb_gain_sel >= BB_GAIN_NUM {
                #[cfg(not(host_rf_kfree_tx_gain_test))]
                kernel::rtw_rust_kfree_warn_on(true);
                return 0;
            }

            if (*kfree_data).flag & KFREE_FLAG_ON != 0 {
                kfree_offset = (*kfree_data).bb_gain[bb_gain_sel as usize][path as usize];
            }
        }
        return kfree_offset;
    }

    #[cfg(not(any(host_rf_kfree_tx_gain_test, rf_power_trim)))]
    {
        let _ = (padapter, path, ch);
        return 0;
    }
}
