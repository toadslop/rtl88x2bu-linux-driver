// SPDX-License-Identifier: GPL-2.0
//! W3-186 — rtw_set_ps_mode part 1 (host L2; kernel wiring in W3-187).

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

#[cfg(host_pwrctrl_set_ps_test)]
use std::ffi::c_void;

const _TRUE: u8 = 1;
const PS_MODE_ACTIVE: u8 = 0;
const PM_Card_Disable: u8 = 10;
const LPS_PG: u8 = 2;
const LPS_PG_INFO_CFG: u8 = 0;
const LPS_PG_REDLEMEM: u8 = 1;
const LPS_PG_PHYDM_EN: u8 = 3;
const PS_STATE_S4: u8 = (1 << 3) | (1 << 2);
const HOST_HW_VAR_LPS_PG_HANDLE: u8 = 100;
const HOST_HW_VAR_H2C_INACTIVE_IPS: u8 = 101;

type Padapter = *mut c_void;
type Systime = u32;

#[cfg(host_pwrctrl_set_ps_test)]
#[repr(C)]
struct PwrctrlPriv {
    pwr_mode: u8,
    smart_ps: u8,
    bcn_ant_mode: u8,
    lps_leave_cnts: u32,
    lps_level: u8,
    wowlan_mode: u8,
    bFwCurrentInPSMode: u8,
    lock: i32,
}

#[cfg(host_pwrctrl_set_ps_test)]
#[repr(C)]
struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
}

#[cfg(host_pwrctrl_set_ps_test)]
#[repr(C)]
pub struct HostSetPsTrace {
    pub rsvd_page_calls: u8,
    pub default_port_calls: u8,
    pub rpwm_val: u8,
    pub exec_lps_called: u8,
    pub exec_lps_ps_mode: u8,
    pub b_fw_current_in_ps: u8,
    pub pwr_mode: u8,
    pub lps_leave_cnts: u32,
    pub hwreg_count: u32,
    pub wow_leave_fail_cnt: u32,
    pub hwreg_var: [u8; 4],
    pub hwreg_val: [u8; 4],
}

#[cfg(host_pwrctrl_set_ps_test)]
mod host_trace {
    use super::*;

    static mut G_TRACE: HostSetPsTrace = HostSetPsTrace {
        rsvd_page_calls: 0,
        default_port_calls: 0,
        rpwm_val: 0,
        exec_lps_called: 0,
        exec_lps_ps_mode: 0,
        b_fw_current_in_ps: 0,
        pwr_mode: 0,
        lps_leave_cnts: 0,
        hwreg_count: 0,
        wow_leave_fail_cnt: 0,
        hwreg_var: [0; 4],
        hwreg_val: [0; 4],
    };
    static mut G_SYS_CLKR_IN_32K: u8 = 0;
    static mut G_TIME: Systime = 0;

    pub fn reset_trace() {
        unsafe {
            core::ptr::write_bytes(
                (&mut G_TRACE as *mut HostSetPsTrace).cast::<u8>(),
                0,
                core::mem::size_of::<HostSetPsTrace>(),
            );
        }
    }

    pub fn reset_hal() {
        unsafe {
            G_SYS_CLKR_IN_32K = 0;
            G_TIME = 0;
        }
    }

    pub fn set_sys_clkr_in_32k(v: u8) {
        unsafe {
            G_SYS_CLKR_IN_32K = if v != 0 { 1 } else { 0 };
        }
    }

    pub fn trace_ptr() -> *mut HostSetPsTrace {
        unsafe { &mut G_TRACE }
    }

    fn record_hw(var: u8, val: u8) {
        unsafe {
            let n = G_TRACE.hwreg_count as usize;
            if n >= 4 {
                return;
            }
            G_TRACE.hwreg_var[n] = var;
            G_TRACE.hwreg_val[n] = val;
            G_TRACE.hwreg_count += 1;
        }
    }

    pub fn set_ps_mode_impl(padapter: Padapter, ps_mode: u8, smart_ps: u8, bcn_ant_mode: u8) {
        if padapter.is_null() {
            return;
        }
        let a = padapter as *mut HostAdapter;
        let p = unsafe { &mut (*a).pwrctrlpriv };
        let mut hdl: u8;
        if ps_mode > PM_Card_Disable {
            return;
        }
        if p.pwr_mode == ps_mode {
            if ps_mode == PS_MODE_ACTIVE {
                return;
            }
            if p.smart_ps == smart_ps && p.bcn_ant_mode == bcn_ant_mode {
                return;
            }
        }
        if ps_mode != PS_MODE_ACTIVE {
            unsafe {
                G_TRACE.rsvd_page_calls += 1;
                G_TRACE.default_port_calls += 1;
            }
        }
        if ps_mode != PS_MODE_ACTIVE && p.lps_level == LPS_PG && p.wowlan_mode != _TRUE {
            record_hw(HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_INFO_CFG);
        }
        if ps_mode == PS_MODE_ACTIVE {
            p.lps_leave_cnts = p.lps_leave_cnts.saturating_add(1);
            p.pwr_mode = ps_mode;
            unsafe {
                G_TRACE.rpwm_val = PS_STATE_S4;
            }
            if p.wowlan_mode == _TRUE {
                let t0 = unsafe { G_TIME };
                loop {
                    let stuck = unsafe { G_SYS_CLKR_IN_32K != 0 };
                    if !stuck {
                        break;
                    }
                    if unsafe { G_TIME - t0 } > 20 {
                        unsafe {
                            G_TRACE.wow_leave_fail_cnt += 1;
                        }
                        break;
                    }
                        unsafe {
                            G_TIME += 1;
                        }
                }
            }
            if p.lps_level == LPS_PG {
                record_hw(HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_REDLEMEM);
            }
            if p.wowlan_mode == _TRUE {
                record_hw(HOST_HW_VAR_H2C_INACTIVE_IPS, ps_mode);
            }
            unsafe {
                G_TRACE.exec_lps_called = 1;
                G_TRACE.exec_lps_ps_mode = ps_mode;
            }
            if p.lps_level == LPS_PG {
                record_hw(HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_PHYDM_EN);
            }
            p.bFwCurrentInPSMode = 0;
        }
        unsafe {
            G_TRACE.lps_leave_cnts = p.lps_leave_cnts;
            G_TRACE.pwr_mode = p.pwr_mode;
            G_TRACE.b_fw_current_in_ps = p.bFwCurrentInPSMode;
        }
    }
}

#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_set_ps_reset_trace() {
    host_trace::reset_trace();
}

#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_set_ps_get_trace() -> *mut HostSetPsTrace {
    host_trace::trace_ptr()
}

#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_set_ps_reset_hal() {
    host_trace::reset_hal();
}

#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_set_ps_set_sys_clkr_in_32k(v: u8) {
    host_trace::set_sys_clkr_in_32k(v);
}

#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn _enter_pwrlock(_plock: *mut i32) {}
#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn _exit_pwrlock(_plock: *mut i32) {}
#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn rtw_set_ps_mode(
    padapter: Padapter,
    ps_mode: u8,
    smart_ps: u8,
    bcn_ant_mode: u8,
    _msg: *const u8,
) {
    host_trace::set_ps_mode_impl(padapter, ps_mode, smart_ps, bcn_ant_mode);
}
