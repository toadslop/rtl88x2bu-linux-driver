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

const _TRUE: u8 = 1;
const PS_MODE_ACTIVE: u8 = 0;
const RF_ON: u8 = 0;

const HOST_HW_VAR_H2C_FW_PWRMODE: u8 = 0;
const HOST_HW_VAR_LPS_STATE_CHK: u8 = 1;
const HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL: u8 = 2;
const HOST_HW_VAR_LPS_RFON_CHK: u8 = 3;

const WIFI_ASOC_STATE: u32 = 0x0000_0001;

type Padapter = *mut c_void;

#[cfg(host_pwrctrl_lps_exec_test)]
#[repr(C)]
struct PwrctrlPriv {
    bFwCurrentInPSMode: u8,
    pwr_mode: u8,
}

#[cfg(host_pwrctrl_lps_exec_test)]
#[repr(C)]
struct MlmePriv {
    fw_state: u32,
}

#[cfg(host_pwrctrl_lps_exec_test)]
#[repr(C)]
struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
    mlmepriv: MlmePriv,
}

#[cfg(host_pwrctrl_lps_exec_test)]
#[repr(C)]
#[derive(Copy, Clone)]
pub struct HostLpsExecHwreg {
    pub var: u8,
    pub val: u8,
}

#[cfg(host_pwrctrl_lps_exec_test)]
#[repr(C)]
pub struct HostLpsExecTrace {
    pub hwreg_count: u32,
    pub hwreg: [HostLpsExecHwreg; 8],
}

#[cfg(host_pwrctrl_lps_exec_test)]
mod host_trace {
    use super::*;

    static mut G_TRACE: HostLpsExecTrace = HostLpsExecTrace {
        hwreg_count: 0,
        hwreg: [HostLpsExecHwreg { var: 0, val: 0 }; 8],
    };

    pub fn reset() {
        unsafe {
            G_TRACE = HostLpsExecTrace {
                hwreg_count: 0,
                hwreg: [HostLpsExecHwreg { var: 0, val: 0 }; 8],
            };
        }
    }

    pub fn record_hwreg(var: u8, val: u8) {
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

    pub fn trace_ptr() -> *mut HostLpsExecTrace {
        unsafe { &mut G_TRACE }
    }
}

#[cfg(host_pwrctrl_lps_exec_test)]
fn mlme_is_asoc(padapter: Padapter) -> bool {
    let a = padapter as *mut HostAdapter;
    let fw = unsafe { (*a).mlmepriv.fw_state };
    fw & WIFI_ASOC_STATE != 0
}

#[cfg(host_pwrctrl_lps_exec_test)]
fn hal_set_hwreg(_padapter: Padapter, var: u8, val: u8) {
    host_trace::record_hwreg(var, val);
}

#[cfg(host_pwrctrl_lps_exec_test)]
fn b_fw_current_in_ps(padapter: Padapter) -> u8 {
    let a = padapter as *mut HostAdapter;
    unsafe { (*a).pwrctrlpriv.bFwCurrentInPSMode }
}

#[cfg(host_pwrctrl_lps_exec_test)]
fn pwr_mode_get(padapter: Padapter) -> u8 {
    let a = padapter as *mut HostAdapter;
    unsafe { (*a).pwrctrlpriv.pwr_mode }
}

#[cfg(not(host_pwrctrl_lps_exec_test))]
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

#[no_mangle]
pub extern "C" fn rtw_exec_lps(padapter: Padapter, ps_mode: u8) {
    if padapter.is_null() {
        return;
    }

    if ps_mode == PS_MODE_ACTIVE {
        #[cfg(host_pwrctrl_lps_exec_test)]
        {
            hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE, ps_mode);
            hal_set_hwreg(padapter, HOST_HW_VAR_LPS_STATE_CHK, ps_mode);
        }
        #[cfg(not(host_pwrctrl_lps_exec_test))]
        unsafe {
            rtw_rust_lps_exec_active_ps_mode(padapter, ps_mode);
        }
        return;
    }

    #[cfg(host_pwrctrl_lps_exec_test)]
    {
        if mlme_is_asoc(padapter) {
            hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE, ps_mode);
        }
    }
    #[cfg(not(host_pwrctrl_lps_exec_test))]
    {
        if mlme_is_asoc_kernel(padapter) {
            unsafe {
                rtw_rust_lps_exec_set_fw_pwrmode(padapter, ps_mode);
            }
        } else {
            unsafe {
                rtw_rust_lps_exec_lps_no_wifi_log(padapter);
            }
        }
    }
}

#[cfg(not(host_pwrctrl_lps_exec_test))]
fn mlme_is_asoc_kernel(padapter: Padapter) -> bool {
    unsafe { rtw_rust_lps_exec_mlme_is_asoc(padapter) == _TRUE }
}

#[no_mangle]
pub extern "C" fn rtw_lps_rfon_ctrl(padapter: Padapter, rfon_ctrl: u8) {
    if padapter.is_null() {
        return;
    }

    #[cfg(host_pwrctrl_lps_exec_test)]
    {
        if b_fw_current_in_ps(padapter) == 0 || pwr_mode_get(padapter) == PS_MODE_ACTIVE {
            return;
        }
        if rfon_ctrl == RF_ON {
            hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL, rfon_ctrl);
            hal_set_hwreg(padapter, HOST_HW_VAR_LPS_RFON_CHK, rfon_ctrl);
        } else if mlme_is_asoc(padapter) {
            hal_set_hwreg(padapter, HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL, rfon_ctrl);
        }
        return;
    }

    #[cfg(not(host_pwrctrl_lps_exec_test))]
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
            if mlme_is_asoc_kernel(padapter) {
                rtw_rust_lps_exec_rfon_rf_off_assoc(padapter, rfon_ctrl);
            } else {
                rtw_rust_lps_exec_rfon_no_wifi_log(padapter);
            }
            rtw_rust_lps_exec_rfon_lclk_rf_off(padapter);
        }
    }
}

#[cfg(host_pwrctrl_lps_exec_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_exec_reset_trace() {
    host_trace::reset();
}

#[cfg(host_pwrctrl_lps_exec_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_exec_get_trace() -> *mut HostLpsExecTrace {
    host_trace::trace_ptr()
}
