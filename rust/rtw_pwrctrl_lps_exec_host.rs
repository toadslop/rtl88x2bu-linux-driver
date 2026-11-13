// SPDX-License-Identifier: GPL-2.0
//! Host L2 stubs for `rtw_pwrctrl_lps_exec.rs` (shared exec / rfon cores).

#![allow(non_snake_case, non_camel_case_types)]

use crate::{Padapter, _TRUE};

pub(crate) const HOST_HW_VAR_H2C_FW_PWRMODE: u8 = 0;
pub(crate) const HOST_HW_VAR_LPS_STATE_CHK: u8 = 1;
pub(crate) const HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL: u8 = 2;
pub(crate) const HOST_HW_VAR_LPS_RFON_CHK: u8 = 3;

#[repr(C)]
pub struct PwrctrlPriv {
    pub bFwCurrentInPSMode: u8,
    pub pwr_mode: u8,
}

#[repr(C)]
pub struct MlmePriv {
    pub fw_state: u32,
}

#[repr(C)]
pub struct HostAdapter {
    pub pwrctrlpriv: PwrctrlPriv,
    pub mlmepriv: MlmePriv,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct HostLpsExecHwreg {
    pub var: u8,
    pub val: u8,
}

#[repr(C)]
pub struct HostLpsExecTrace {
    pub hwreg_count: u32,
    pub hwreg: [HostLpsExecHwreg; 8],
}

const WIFI_ASOC_STATE: u32 = 0x0000_0001;

static mut G_TRACE: HostLpsExecTrace = HostLpsExecTrace {
    hwreg_count: 0,
    hwreg: [HostLpsExecHwreg { var: 0, val: 0 }; 8],
};

fn adapter(padapter: Padapter) -> *mut HostAdapter {
    padapter as *mut HostAdapter
}

fn record_hwreg(var: u8, val: u8) {
    unsafe {
        let n = G_TRACE.hwreg_count as usize;
        if n >= G_TRACE.hwreg.len() {
            return;
        }
        G_TRACE.hwreg[n].var = var;
        G_TRACE.hwreg[n].val = val;
        G_TRACE.hwreg_count += 1;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_exec_reset_trace() {
    unsafe {
        G_TRACE = HostLpsExecTrace {
            hwreg_count: 0,
            hwreg: [HostLpsExecHwreg { var: 0, val: 0 }; 8],
        };
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_exec_get_trace() -> *mut HostLpsExecTrace {
    unsafe { &mut G_TRACE }
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_mlme_is_asoc(padapter: Padapter) -> u8 {
    let fw = unsafe { (*adapter(padapter)).mlmepriv.fw_state };
    if fw & WIFI_ASOC_STATE != 0 {
        _TRUE
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_get_b_fw_current_in_ps(padapter: Padapter) -> u8 {
    unsafe { (*adapter(padapter)).pwrctrlpriv.bFwCurrentInPSMode }
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_get_pwr_mode(padapter: Padapter) -> u8 {
    unsafe { (*adapter(padapter)).pwrctrlpriv.pwr_mode }
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_active_ps_mode(_padapter: Padapter, ps_mode: u8) {
    record_hwreg(HOST_HW_VAR_H2C_FW_PWRMODE, ps_mode);
    record_hwreg(HOST_HW_VAR_LPS_STATE_CHK, ps_mode);
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_set_fw_pwrmode(_padapter: Padapter, ps_mode: u8) {
    record_hwreg(HOST_HW_VAR_H2C_FW_PWRMODE, ps_mode);
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_rfon_not_in_lps_log(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_lps_no_wifi_log(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_rfon_no_wifi_log(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_rfon_lclk_rf_on(_padapter: Padapter) {
    let _ = _padapter;
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_rfon_lps_ack_block(_padapter: Padapter, rfon_ctrl: u8) {
    let _ = _padapter;
    record_hwreg(HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL, rfon_ctrl);
    record_hwreg(HOST_HW_VAR_LPS_RFON_CHK, rfon_ctrl);
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_rfon_rf_off_assoc(_padapter: Padapter, rfon_ctrl: u8) {
    let _ = _padapter;
    record_hwreg(HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL, rfon_ctrl);
}

#[no_mangle]
pub extern "C" fn rtw_rust_lps_exec_rfon_lclk_rf_off(_padapter: Padapter) {
    let _ = _padapter;
}
