// SPDX-License-Identifier: GPL-2.0
//! W3-181 — `rtw_exec_lps` and `rtw_lps_rfon_ctrl` from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_lps_exec_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_lps_exec_test))]
use core::ffi::c_void;

pub(crate) const _TRUE: u8 = 1;
pub(crate) const PS_MODE_ACTIVE: u8 = 0;
pub(crate) const RF_ON: u8 = 0;

pub(crate) type Padapter = *mut c_void;

#[cfg(host_pwrctrl_lps_exec_test)]
#[path = "rtw_pwrctrl_lps_exec_host.rs"]
mod host;

extern "C" {
    fn rtw_rust_lps_exec_mlme_is_asoc(padapter: Padapter) -> u8;
    fn rtw_rust_lps_exec_get_b_fw_current_in_ps(padapter: Padapter) -> u8;
    fn rtw_rust_lps_exec_get_pwr_mode(padapter: Padapter) -> u8;
    fn rtw_rust_lps_exec_active_ps_mode(padapter: Padapter, ps_mode: u8);
    fn rtw_rust_lps_exec_set_fw_pwrmode(padapter: Padapter, ps_mode: u8);
    fn rtw_rust_lps_exec_rfon_lclk_rf_on(padapter: Padapter);
    fn rtw_rust_lps_exec_rfon_lps_ack_block(padapter: Padapter, rfon_ctrl: u8);
    fn rtw_rust_lps_exec_rfon_rf_off_assoc(padapter: Padapter, rfon_ctrl: u8);
    fn rtw_rust_lps_exec_rfon_lclk_rf_off(padapter: Padapter);
    fn rtw_rust_lps_exec_rfon_not_in_lps_log(padapter: Padapter);
    fn rtw_rust_lps_exec_lps_no_wifi_log(padapter: Padapter);
    fn rtw_rust_lps_exec_rfon_no_wifi_log(padapter: Padapter);
}

fn mlme_is_asoc(padapter: Padapter) -> bool {
    unsafe { rtw_rust_lps_exec_mlme_is_asoc(padapter) == _TRUE }
}

#[no_mangle]
pub extern "C" fn rtw_exec_lps(padapter: Padapter, ps_mode: u8) {
    if padapter.is_null() {
        return;
    }

    if ps_mode == PS_MODE_ACTIVE {
        unsafe {
            rtw_rust_lps_exec_active_ps_mode(padapter, ps_mode);
        }
        return;
    }

    if mlme_is_asoc(padapter) {
        unsafe {
            rtw_rust_lps_exec_set_fw_pwrmode(padapter, ps_mode);
        }
    } else {
        unsafe {
            rtw_rust_lps_exec_lps_no_wifi_log(padapter);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_lps_rfon_ctrl(padapter: Padapter, rfon_ctrl: u8) {
    if padapter.is_null() {
        return;
    }

    unsafe {
        if rtw_rust_lps_exec_get_b_fw_current_in_ps(padapter) == 0
            || rtw_rust_lps_exec_get_pwr_mode(padapter) == PS_MODE_ACTIVE
        {
            rtw_rust_lps_exec_rfon_not_in_lps_log(padapter);
            return;
        }
        if rfon_ctrl == RF_ON {
            rtw_rust_lps_exec_rfon_lclk_rf_on(padapter);
            rtw_rust_lps_exec_rfon_lps_ack_block(padapter, rfon_ctrl);
        } else {
            if mlme_is_asoc(padapter) {
                rtw_rust_lps_exec_rfon_rf_off_assoc(padapter, rfon_ctrl);
            } else {
                rtw_rust_lps_exec_rfon_no_wifi_log(padapter);
            }
            rtw_rust_lps_exec_rfon_lclk_rf_off(padapter);
        }
    }
}
