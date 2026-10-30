// SPDX-License-Identifier: GPL-2.0
//! W3-186 / W3-187 — `rtw_set_ps_mode` host L2 (kernel wiring in W3-187 PR3).

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

#[cfg(not(host_pwrctrl_set_ps_test))]
use core::ffi::{c_char, c_void};

const _TRUE: u8 = 1;
const PS_MODE_ACTIVE: u8 = 0;
const PM_Card_Disable: u8 = 10;
const LPS_PG: u8 = 2;
const LPS_PG_INFO_CFG: u8 = 0;
const LPS_PG_REDLEMEM: u8 = 1;
const LPS_PG_PHYDM_DIS: u8 = 2;
const LPS_PG_PHYDM_EN: u8 = 3;
const PS_STATE_S0: u8 = 1 << 0;
const PS_STATE_S2: u8 = 1 << 1;
const PS_STATE_S4: u8 = (1 << 3) | (1 << 2);
const WIFI_ASOC_STATE: u32 = 0x0000_0001;
const HOST_HW_VAR_LPS_PG_HANDLE: u8 = 100;
const HOST_HW_VAR_H2C_INACTIVE_IPS: u8 = 101;

type Padapter = *mut c_void;
type Systime = u32;

#[cfg(host_pwrctrl_set_ps_test)]
#[repr(C)]
struct MlmePriv {
    fw_state: u32,
}

#[cfg(host_pwrctrl_set_ps_test)]
#[repr(C)]
struct PwrctrlPriv {
    pwr_mode: u8,
    smart_ps: u8,
    bcn_ant_mode: u8,
    lps_leave_cnts: u32,
    lps_enter_cnts: u32,
    lps_level: u8,
    wowlan_mode: u8,
    bFwCurrentInPSMode: u8,
    alives: u8,
    lock: i32,
}

#[cfg(host_pwrctrl_set_ps_test)]
#[repr(C)]
struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
    mlmepriv: MlmePriv,
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
    pub smart_ps: u8,
    pub bcn_ant_mode: u8,
    pub lps_leave_cnts: u32,
    pub lps_enter_cnts: u32,
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
        smart_ps: 0,
        bcn_ant_mode: 0,
        lps_leave_cnts: 0,
        lps_enter_cnts: 0,
        hwreg_count: 0,
        wow_leave_fail_cnt: 0,
        hwreg_var: [0; 4],
        hwreg_val: [0; 4],
    };
    static mut G_SYS_CLKR_IN_32K: u8 = 0;
    static mut G_TIME: Systime = 0;
    static mut G_PS_RDY: u8 = 1;
    static mut G_FW_ASOC: u8 = 1;

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

    pub fn set_ps_rdy(v: u8) {
        unsafe {
            G_PS_RDY = if v != 0 { 1 } else { 0 };
        }
    }

    pub fn set_fw_asoc(v: u8) {
        unsafe {
            G_FW_ASOC = if v != 0 { 1 } else { 0 };
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

    fn set_rpwm(v: u8) {
        unsafe {
            G_TRACE.rpwm_val = v;
        }
    }

    fn exec_lps(ps_mode: u8) {
        unsafe {
            G_TRACE.exec_lps_called = 1;
            G_TRACE.exec_lps_ps_mode = ps_mode;
        }
    }

    fn enter_lps_allowed(a: &HostAdapter) -> bool {
        unsafe {
            if G_PS_RDY == 0 {
                return false;
            }
            G_FW_ASOC != 0 && (a.mlmepriv.fw_state & WIFI_ASOC_STATE) != 0
        }
    }

    extern "C" {
        fn _enter_pwrlock(plock: *mut i32);
        fn _exit_pwrlock(plock: *mut i32);
    }

    fn sync_trace(p: &PwrctrlPriv) {
        unsafe {
            G_TRACE.lps_leave_cnts = p.lps_leave_cnts;
            G_TRACE.lps_enter_cnts = p.lps_enter_cnts;
            G_TRACE.pwr_mode = p.pwr_mode;
            G_TRACE.smart_ps = p.smart_ps;
            G_TRACE.bcn_ant_mode = p.bcn_ant_mode;
            G_TRACE.b_fw_current_in_ps = p.bFwCurrentInPSMode;
        }
    }

    pub fn set_ps_mode_impl(padapter: Padapter, ps_mode: u8, smart_ps: u8, bcn_ant_mode: u8) {
        if padapter.is_null() {
            return;
        }
        let a = padapter as *mut HostAdapter;
        let p = unsafe { &mut (*a).pwrctrlpriv };
        if ps_mode > PM_Card_Disable {
            return;
        }
        if p.pwr_mode == ps_mode {
            if ps_mode == PS_MODE_ACTIVE {
                return;
            }
            #[cfg(not(config_bt_coexist))]
            if p.smart_ps == smart_ps && p.bcn_ant_mode == bcn_ant_mode {
                return;
            }
            #[cfg(config_bt_coexist)]
            let _ = (smart_ps, bcn_ant_mode);
        }
        #[cfg(config_fw_multi_port_support)]
        if ps_mode != PS_MODE_ACTIVE {
            unsafe {
                G_TRACE.rsvd_page_calls += 1;
                G_TRACE.default_port_calls += 1;
            }
        }
        #[cfg(config_lps_pg)]
        if ps_mode != PS_MODE_ACTIVE && p.lps_level == LPS_PG && p.wowlan_mode != _TRUE {
            record_hw(HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_INFO_CFG);
        }
        #[cfg(config_lps_lclk)]
        {
            unsafe {
                _enter_pwrlock(&mut p.lock);
            }
            if ps_mode == PS_MODE_ACTIVE {
                p.lps_leave_cnts = p.lps_leave_cnts.wrapping_add(1);
                p.pwr_mode = ps_mode;
                set_rpwm(PS_STATE_S4);
                #[cfg(any(config_wowlan, config_ap_wowlan, config_p2p_wowlan))]
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
                #[cfg(config_lps_pg)]
                if p.lps_level == LPS_PG {
                    record_hw(HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_REDLEMEM);
                }
                #[cfg(config_wowlan)]
                if p.wowlan_mode == _TRUE {
                    record_hw(HOST_HW_VAR_H2C_INACTIVE_IPS, ps_mode);
                }
                exec_lps(ps_mode);
                #[cfg(config_lps_pg)]
                if p.lps_level == LPS_PG {
                    record_hw(HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_PHYDM_EN);
                }
                p.bFwCurrentInPSMode = 0;
            } else if enter_lps_allowed(unsafe { &*a }) {
                p.lps_enter_cnts = p.lps_enter_cnts.wrapping_add(1);
                p.bFwCurrentInPSMode = _TRUE;
                p.pwr_mode = ps_mode;
                p.smart_ps = smart_ps;
                p.bcn_ant_mode = bcn_ant_mode;
                #[cfg(config_lps_pg)]
                if p.lps_level == LPS_PG {
                    record_hw(HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_PHYDM_DIS);
                }
                exec_lps(ps_mode);
                #[cfg(config_wowlan)]
                if p.wowlan_mode == _TRUE {
                    record_hw(HOST_HW_VAR_H2C_INACTIVE_IPS, ps_mode);
                }
                let pslv = if p.alives == 0 {
                    PS_STATE_S0
                } else {
                    PS_STATE_S2
                };
                set_rpwm(pslv);
            }
            sync_trace(p);
            unsafe {
                _exit_pwrlock(&mut p.lock);
            }
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
pub extern "C" fn host_pwrctrl_set_ps_set_ps_rdy(v: u8) {
    host_trace::set_ps_rdy(v);
}

#[cfg(host_pwrctrl_set_ps_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_set_ps_set_fw_asoc(v: u8) {
    host_trace::set_fw_asoc(v);
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

#[cfg(not(host_pwrctrl_set_ps_test))]
extern "C" {
    fn rtw_rust_ps_proc_enter_pwrlock(padapter: Padapter);
    fn rtw_rust_ps_proc_exit_pwrlock(padapter: Padapter);
    fn rtw_rust_set_ps_get_u8(padapter: Padapter, field: u32) -> u8;
    fn rtw_rust_set_ps_set_u8(padapter: Padapter, field: u32, val: u8);
    fn rtw_rust_set_ps_get_u32(padapter: Padapter, field: u32) -> u32;
    fn rtw_rust_set_ps_inc_u32(padapter: Padapter, field: u32);
    fn rtw_rust_set_ps_may_enter(padapter: Padapter) -> u8;
    fn rtw_rust_set_ps_multiport_preamble(padapter: Padapter);
    fn rtw_rust_set_ps_lps_pg_info_cfg(padapter: Padapter);
    fn rtw_rust_set_ps_hal_hwreg(padapter: Padapter, var: u8, val: u8);
    fn rtw_rust_set_ps_wow_leave_wait(padapter: Padapter);
    fn rtw_exec_lps(padapter: Padapter, ps_mode: u8);
    fn rtw_set_rpwm(padapter: Padapter, pslv: u8);
}

#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U8_PWR_MODE: u32 = 0;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U8_SMART_PS: u32 = 1;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U8_BCN_ANT: u32 = 2;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U8_LPS_LEVEL: u32 = 3;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U8_WOWLAN: u32 = 4;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U8_BFW_PS: u32 = 5;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U8_ALIVES: u32 = 6;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U32_LPS_LEAVE: u32 = 0;
#[cfg(not(host_pwrctrl_set_ps_test))]
const SET_PS_U32_LPS_ENTER: u32 = 1;

#[cfg(not(host_pwrctrl_set_ps_test))]
fn kernel_set_ps_mode_impl(padapter: Padapter, ps_mode: u8, smart_ps: u8, bcn_ant_mode: u8) {
    if padapter.is_null() {
        return;
    }
    if ps_mode > PM_Card_Disable {
        return;
    }
    let cur_pwr = unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_PWR_MODE) };
    if cur_pwr == ps_mode {
        if ps_mode == PS_MODE_ACTIVE {
            return;
        }
        #[cfg(not(config_bt_coexist))]
        {
            let s = unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_SMART_PS) };
            let b = unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_BCN_ANT) };
            if s == smart_ps && b == bcn_ant_mode {
                return;
            }
        }
        #[cfg(config_bt_coexist)]
        let _ = (smart_ps, bcn_ant_mode);
    }
    if ps_mode != PS_MODE_ACTIVE {
        unsafe {
            rtw_rust_set_ps_multiport_preamble(padapter);
        }
        if ps_mode != PS_MODE_ACTIVE {
            let lvl = unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_LPS_LEVEL) };
            let wow = unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_WOWLAN) };
            if lvl == LPS_PG && wow != _TRUE {
                unsafe {
                    rtw_rust_set_ps_lps_pg_info_cfg(padapter);
                }
            }
        }
    }
    #[cfg(config_lps_lclk)]
    {
        unsafe {
            rtw_rust_ps_proc_enter_pwrlock(padapter);
        }
        if ps_mode == PS_MODE_ACTIVE {
            unsafe {
                rtw_rust_set_ps_inc_u32(padapter, SET_PS_U32_LPS_LEAVE);
                rtw_rust_set_ps_set_u8(padapter, SET_PS_U8_PWR_MODE, ps_mode);
                rtw_rust_set_ps_wow_leave_wait(padapter);
            }
            #[cfg(config_lps_pg)]
            if unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_LPS_LEVEL) } == LPS_PG {
                unsafe {
                    rtw_rust_set_ps_hal_hwreg(padapter, HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_REDLEMEM);
                }
            }
            #[cfg(config_wowlan)]
            if unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_WOWLAN) } == _TRUE {
                unsafe {
                    rtw_rust_set_ps_hal_hwreg(padapter, HOST_HW_VAR_H2C_INACTIVE_IPS, ps_mode);
                }
            }
            unsafe {
                rtw_exec_lps(padapter, ps_mode);
            }
            #[cfg(config_lps_pg)]
            if unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_LPS_LEVEL) } == LPS_PG {
                unsafe {
                    rtw_rust_set_ps_hal_hwreg(padapter, HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_PHYDM_EN);
                }
            }
            unsafe {
                rtw_rust_set_ps_set_u8(padapter, SET_PS_U8_BFW_PS, 0);
                rtw_set_rpwm(padapter, PS_STATE_S4);
            }
        } else if unsafe { rtw_rust_set_ps_may_enter(padapter) } != 0 {
            unsafe {
                rtw_rust_set_ps_inc_u32(padapter, SET_PS_U32_LPS_ENTER);
                rtw_rust_set_ps_set_u8(padapter, SET_PS_U8_BFW_PS, _TRUE);
                rtw_rust_set_ps_set_u8(padapter, SET_PS_U8_PWR_MODE, ps_mode);
                rtw_rust_set_ps_set_u8(padapter, SET_PS_U8_SMART_PS, smart_ps);
                rtw_rust_set_ps_set_u8(padapter, SET_PS_U8_BCN_ANT, bcn_ant_mode);
            }
            #[cfg(config_lps_pg)]
            if unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_LPS_LEVEL) } == LPS_PG {
                unsafe {
                    rtw_rust_set_ps_hal_hwreg(padapter, HOST_HW_VAR_LPS_PG_HANDLE, LPS_PG_PHYDM_DIS);
                }
            }
            unsafe {
                rtw_exec_lps(padapter, ps_mode);
            }
            #[cfg(config_wowlan)]
            if unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_WOWLAN) } == _TRUE {
                unsafe {
                    rtw_rust_set_ps_hal_hwreg(padapter, HOST_HW_VAR_H2C_INACTIVE_IPS, ps_mode);
                }
            }
            let pslv = if unsafe { rtw_rust_set_ps_get_u8(padapter, SET_PS_U8_ALIVES) } == 0 {
                PS_STATE_S0
            } else {
                PS_STATE_S2
            };
            unsafe {
                rtw_set_rpwm(padapter, pslv);
            }
        }
        unsafe {
            rtw_rust_ps_proc_exit_pwrlock(padapter);
        }
    }
}

#[cfg(not(host_pwrctrl_set_ps_test))]
#[no_mangle]
pub extern "C" fn rtw_set_ps_mode(
    padapter: Padapter,
    ps_mode: u8,
    smart_ps: u8,
    bcn_ant_mode: u8,
    _msg: *const c_char,
) {
    kernel_set_ps_mode_impl(padapter, ps_mode, smart_ps, bcn_ant_mode);
}
