// SPDX-License-Identifier: GPL-2.0
//! W3-185 — `rtw_set_fw_in_ips_mode` from `core/rtw_pwrctrl.c` (CONFIG_FWLPS_IN_IPS).

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

#[cfg(host_pwrctrl_fw_ips_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_fw_ips_test))]
use core::ffi::c_void;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const IPS_NONE: u8 = 0;

const H2C_INACTIVE_PS_: u8 = 0x27;
const H2C_INACTIVE_PS_LEN: u8 = 4;
const REG_HMETFR: u16 = 0x01CC;
const REG_CR: u16 = 0x0100;

const HOST_HW_VAR_SET_RPWM: u8 = 109;
const HOST_HW_VAR_CPWM: u8 = 110;
const HOST_HW_VAR_RPWM_TOG: u8 = 147;

type Padapter = *mut c_void;
type Systime = u32;

#[cfg(host_pwrctrl_fw_ips_test)]
#[repr(C)]
struct PwrctrlPriv {
    ips_mode_req: u8,
    tog: u8,
}

#[cfg(host_pwrctrl_fw_ips_test)]
#[repr(C)]
struct HostAdapter {
    netif_up: u8,
    pwrctrlpriv: PwrctrlPriv,
}

#[cfg(host_pwrctrl_fw_ips_test)]
mod host_hal {
    use super::*;

    static mut G_HMETFR: [u8; 128] = [0; 128];
    static mut G_HMETFR_LEN: i32 = 0;
    static mut G_HMETFR_IDX: i32 = 0;
    static mut G_REG_CR: [u8; 32] = [0; 32];
    static mut G_REG_CR_LEN: i32 = 0;
    static mut G_REG_CR_IDX: i32 = 0;
    static mut G_CPWM_ORIG: u8 = 0;
    static mut G_CPWM_AFTER: u8 = 0;
    static mut G_CPWM_READS_BEFORE: u32 = 0;
    static mut G_CPWM_READ_COUNT: u32 = 0;
    static mut G_TIME_MS: u32 = 0;
    static mut G_BTCOEX_CALLS: i32 = 0;
    static mut G_LAST_BTCOEX: u8 = 0;
    static mut G_H2C_COUNT: i32 = 0;
    static mut G_H2C_PARM0: [u8; 4] = [0; 4];
    static mut G_READ8_COUNT: i32 = 0;
    static mut G_HWREG_COUNT: i32 = 0;

    pub fn reset_trace() {
        unsafe {
            G_HMETFR_IDX = 0;
            G_REG_CR_IDX = 0;
            G_CPWM_READ_COUNT = 0;
            G_BTCOEX_CALLS = 0;
            G_H2C_COUNT = 0;
            G_READ8_COUNT = 0;
            G_HWREG_COUNT = 0;
        }
    }

    pub fn set_hmetfr(vals: &[u8]) {
        unsafe {
            let n = vals.len().min(G_HMETFR.len());
            G_HMETFR[..n].copy_from_slice(&vals[..n]);
            G_HMETFR_LEN = n as i32;
            G_HMETFR_IDX = 0;
        }
    }

    pub fn set_reg_cr(vals: &[u8]) {
        unsafe {
            let n = vals.len().min(G_REG_CR.len());
            G_REG_CR[..n].copy_from_slice(&vals[..n]);
            G_REG_CR_LEN = n as i32;
            G_REG_CR_IDX = 0;
        }
    }

    pub fn set_cpwm_leave(orig: u8, after: u8, reads_before: u32) {
        unsafe {
            G_CPWM_ORIG = orig;
            G_CPWM_AFTER = after;
            G_CPWM_READS_BEFORE = reads_before;
            G_CPWM_READ_COUNT = 0;
        }
    }

    pub fn set_time_ms(ms: u32) {
        unsafe {
            G_TIME_MS = ms;
        }
    }

    fn read8_reg(reg: u16) -> u8 {
        unsafe {
            G_READ8_COUNT += 1;
            if reg == REG_HMETFR && G_HMETFR_IDX < G_HMETFR_LEN {
                let v = G_HMETFR[G_HMETFR_IDX as usize];
                G_HMETFR_IDX += 1;
                return v;
            }
            if reg == REG_CR && G_REG_CR_IDX < G_REG_CR_LEN {
                let v = G_REG_CR[G_REG_CR_IDX as usize];
                G_REG_CR_IDX += 1;
                return v;
            }
            if reg == REG_HMETFR {
                return 0;
            }
            0
        }
    }

    pub fn btcoex_ips_notify(_padapter: Padapter, mode: u8) {
        unsafe {
            G_BTCOEX_CALLS += 1;
            G_LAST_BTCOEX = mode;
        }
    }

    pub fn hal_fill_h2c(_padapter: Padapter, _cmd: u8, _len: u8, parm: &[u8; 4]) {
        unsafe {
            if G_H2C_COUNT < 4 {
                G_H2C_PARM0 = *parm;
                G_H2C_COUNT += 1;
            }
        }
    }

    pub fn read8(_padapter: Padapter, reg: u16) -> u8 {
        read8_reg(reg)
    }

    pub fn hal_get_hwreg(_padapter: Padapter, var: u8) -> u8 {
        unsafe {
            G_HWREG_COUNT += 1;
            if var == HOST_HW_VAR_RPWM_TOG {
                return 0x10;
            }
            if var == HOST_HW_VAR_CPWM {
                G_CPWM_READ_COUNT += 1;
                if G_CPWM_READ_COUNT > G_CPWM_READS_BEFORE {
                    return G_CPWM_AFTER;
                }
                return G_CPWM_ORIG;
            }
            0
        }
    }

    pub fn hal_set_hwreg(padapter: Padapter, var: u8, val: u8) {
        unsafe {
            G_HWREG_COUNT += 1;
        }
        if var == HOST_HW_VAR_SET_RPWM {
            let a = padapter as *mut HostAdapter;
            unsafe {
                (*a).pwrctrlpriv.tog = (val.wrapping_add(0x80)) & 0x80;
            }
        }
    }

    pub fn mdelay(ms: u32) {
        unsafe {
            G_TIME_MS += ms;
        }
    }

    pub fn current_time() -> Systime {
        unsafe { G_TIME_MS }
    }

    pub fn passing_time_ms(start: Systime) -> u32 {
        unsafe {
            if G_TIME_MS < start {
                0
            } else {
                G_TIME_MS - start
            }
        }
    }

    pub fn netif_up(padapter: Padapter) -> u8 {
        let a = padapter as *mut HostAdapter;
        unsafe { (*a).netif_up }
    }

    pub fn ips_mode_req(padapter: Padapter) -> u8 {
        let a = padapter as *mut HostAdapter;
        unsafe { (*a).pwrctrlpriv.ips_mode_req }
    }

    pub fn set_tog(padapter: Padapter, tog: u8) {
        let a = padapter as *mut HostAdapter;
        unsafe {
            (*a).pwrctrlpriv.tog = tog;
        }
    }

    pub fn h2c_count() -> i32 {
        unsafe { G_H2C_COUNT }
    }

    pub fn h2c_parm0() -> u8 {
        unsafe { G_H2C_PARM0[0] }
    }

    pub fn btcoex_calls() -> i32 {
        unsafe { G_BTCOEX_CALLS }
    }

    pub fn last_btcoex() -> u8 {
        unsafe { G_LAST_BTCOEX }
    }

    pub fn read8_count() -> i32 {
        unsafe { G_READ8_COUNT }
    }

    pub fn hwreg_count() -> i32 {
        unsafe { G_HWREG_COUNT }
    }
}

#[cfg(not(host_pwrctrl_fw_ips_test))]
extern "C" {
    fn rtw_rust_fw_ips_netif_up(padapter: Padapter) -> u8;
    fn rtw_rust_fw_ips_ips_mode_req(padapter: Padapter) -> u8;
    fn rtw_rust_fw_ips_set_tog(padapter: Padapter, tog: u8);
    fn rtw_rust_fw_ips_btcoex_ips_notify(padapter: Padapter, mode: u8);
    fn rtw_rust_fw_ips_hal_fill_h2c(padapter: Padapter, cmd: u8, len: u8, parm: *const u8);
    fn rtw_rust_fw_ips_read8(padapter: Padapter, reg: u16) -> u8;
    fn rtw_rust_fw_ips_hal_get_cpwm(padapter: Padapter, out: *mut u8);
    fn rtw_rust_fw_ips_hal_get_rpwm_tog(padapter: Padapter, out: *mut u8);
    fn rtw_rust_fw_ips_hal_set_rpwm(padapter: Padapter, rpwm: u8);
    fn rtw_rust_fw_ips_mdelay(ms: u32);
    fn rtw_rust_fw_ips_get_current_time() -> Systime;
    fn rtw_rust_fw_ips_get_passing_time_ms(start: Systime) -> u32;
}

#[no_mangle]
pub extern "C" fn rtw_set_fw_in_ips_mode(padapter: Padapter, enable: u8) {
    if padapter.is_null() {
        return;
    }

    #[cfg(host_pwrctrl_fw_ips_test)]
    {
        if host_hal::netif_up(padapter) == _FALSE {
            return;
        }
    }
    #[cfg(not(host_pwrctrl_fw_ips_test))]
    {
        if unsafe { rtw_rust_fw_ips_netif_up(padapter) } == _FALSE {
            return;
        }
    }

    let mut parm = [0u8; 4];
    let mut cnt: i32;
    let mut val8: u8 = 0;
    let mut cpwm_orig: u8;
    let mut cpwm_now: u8 = 0;

    if enable != 0 {
        #[cfg(host_pwrctrl_fw_ips_test)]
        let ips_req = host_hal::ips_mode_req(padapter);
        #[cfg(not(host_pwrctrl_fw_ips_test))]
        let ips_req = unsafe { rtw_rust_fw_ips_ips_mode_req(padapter) };

        #[cfg(host_pwrctrl_fw_ips_test)]
        host_hal::btcoex_ips_notify(padapter, ips_req);
        #[cfg(not(host_pwrctrl_fw_ips_test))]
        unsafe {
            rtw_rust_fw_ips_btcoex_ips_notify(padapter, ips_req);
        }

        parm[0] = 0x1;
        #[cfg(host_pwrctrl_fw_ips_test)]
        host_hal::hal_fill_h2c(padapter, H2C_INACTIVE_PS_, H2C_INACTIVE_PS_LEN, &parm);
        #[cfg(not(host_pwrctrl_fw_ips_test))]
        unsafe {
            rtw_rust_fw_ips_hal_fill_h2c(
                padapter,
                H2C_INACTIVE_PS_,
                H2C_INACTIVE_PS_LEN,
                parm.as_ptr(),
            );
        }

        cnt = 0;
        loop {
            #[cfg(host_pwrctrl_fw_ips_test)]
            {
                val8 = host_hal::read8(padapter, REG_HMETFR);
                host_hal::mdelay(10);
            }
            #[cfg(not(host_pwrctrl_fw_ips_test))]
            {
                val8 = unsafe { rtw_rust_fw_ips_read8(padapter, REG_HMETFR) };
                unsafe { rtw_rust_fw_ips_mdelay(10) };
            }
            cnt += 1;
            if cnt >= 100 || val8 == 0 {
                break;
            }
        }

        if val8 == 0 {
            #[cfg(host_pwrctrl_fw_ips_test)]
            {
                val8 = host_hal::hal_get_hwreg(padapter, HOST_HW_VAR_RPWM_TOG);
                val8 = val8.wrapping_add(0x80);
                val8 |= 1;
                host_hal::hal_set_hwreg(padapter, HOST_HW_VAR_SET_RPWM, val8);
                host_hal::set_tog(padapter, (val8.wrapping_add(0x80)) & 0x80);
            }
            #[cfg(not(host_pwrctrl_fw_ips_test))]
            unsafe {
                rtw_rust_fw_ips_hal_get_rpwm_tog(padapter, &mut val8);
                val8 = val8.wrapping_add(0x80);
                val8 |= 1;
                rtw_rust_fw_ips_hal_set_rpwm(padapter, val8);
                rtw_rust_fw_ips_set_tog(padapter, (val8.wrapping_add(0x80)) & 0x80);
            }
            cnt = 0;
            if parm[1] == 0 || parm[2] == 0 {
                loop {
                    #[cfg(host_pwrctrl_fw_ips_test)]
                    {
                        val8 = host_hal::read8(padapter, REG_CR);
                        host_hal::mdelay(10);
                    }
                    #[cfg(not(host_pwrctrl_fw_ips_test))]
                    {
                        val8 = unsafe { rtw_rust_fw_ips_read8(padapter, REG_CR) };
                        unsafe { rtw_rust_fw_ips_mdelay(10) };
                    }
                    cnt += 1;
                    if cnt >= 20 || val8 == 0xEA {
                        break;
                    }
                }
                let _ = val8;
            }
        }
    } else {
        {
            cpwm_orig = 0;
            #[cfg(host_pwrctrl_fw_ips_test)]
            {
                cpwm_orig = host_hal::hal_get_hwreg(padapter, HOST_HW_VAR_CPWM);
                val8 = host_hal::hal_get_hwreg(padapter, HOST_HW_VAR_RPWM_TOG);
                val8 = val8.wrapping_add(0x80);
                val8 |= 1 << 6;
                host_hal::hal_set_hwreg(padapter, HOST_HW_VAR_SET_RPWM, val8);
                host_hal::set_tog(padapter, (val8.wrapping_add(0x80)) & 0x80);
            }
            #[cfg(not(host_pwrctrl_fw_ips_test))]
            unsafe {
                rtw_rust_fw_ips_hal_get_cpwm(padapter, &mut cpwm_orig);
                rtw_rust_fw_ips_hal_get_rpwm_tog(padapter, &mut val8);
                val8 = val8.wrapping_add(0x80);
                val8 |= 1 << 6;
                rtw_rust_fw_ips_hal_set_rpwm(padapter, val8);
                rtw_rust_fw_ips_set_tog(padapter, (val8.wrapping_add(0x80)) & 0x80);
            }

            #[cfg(host_pwrctrl_fw_ips_test)]
            let start = host_hal::current_time();
            #[cfg(not(host_pwrctrl_fw_ips_test))]
            let start = unsafe { rtw_rust_fw_ips_get_current_time() };

            loop {
                #[cfg(host_pwrctrl_fw_ips_test)]
                {
                    host_hal::mdelay(1);
                    cpwm_now = host_hal::hal_get_hwreg(padapter, HOST_HW_VAR_CPWM);
                }
                #[cfg(not(host_pwrctrl_fw_ips_test))]
                unsafe {
                    rtw_rust_fw_ips_mdelay(1);
                    rtw_rust_fw_ips_hal_get_cpwm(padapter, &mut cpwm_now);
                }
                if (cpwm_orig ^ cpwm_now) & 0x80 != 0 {
                    break;
                }
                #[cfg(host_pwrctrl_fw_ips_test)]
                let elapsed = host_hal::passing_time_ms(start);
                #[cfg(not(host_pwrctrl_fw_ips_test))]
                let elapsed = unsafe { rtw_rust_fw_ips_get_passing_time_ms(start) };
                if elapsed > 100 {
                    break;
                }
            }
        }

        parm[0] = 0;
        parm[1] = 0;
        parm[2] = 0;
        #[cfg(host_pwrctrl_fw_ips_test)]
        host_hal::hal_fill_h2c(padapter, H2C_INACTIVE_PS_, H2C_INACTIVE_PS_LEN, &parm);
        #[cfg(not(host_pwrctrl_fw_ips_test))]
        unsafe {
            rtw_rust_fw_ips_hal_fill_h2c(
                padapter,
                H2C_INACTIVE_PS_,
                H2C_INACTIVE_PS_LEN,
                parm.as_ptr(),
            );
        }

        #[cfg(host_pwrctrl_fw_ips_test)]
        host_hal::btcoex_ips_notify(padapter, IPS_NONE);
        #[cfg(not(host_pwrctrl_fw_ips_test))]
        unsafe {
            rtw_rust_fw_ips_btcoex_ips_notify(padapter, IPS_NONE);
        }
    }
}

#[cfg(host_pwrctrl_fw_ips_test)]
mod host_pwrctrl_fw_ips {
    #[repr(C)]
    #[derive(Copy, Clone)]
    pub struct host_pwrctrl_fw_ips_h2c {
        pub cmd: u8,
        pub len: u8,
        pub parm: [u8; 4],
    }

    #[repr(C)]
    #[derive(Copy, Clone)]
    pub struct host_pwrctrl_fw_ips_hwreg {
        pub var: u8,
        pub val: u8,
    }

    #[repr(C)]
    pub struct host_pwrctrl_fw_ips_trace {
        pub btcoex_ips_notify_calls: i32,
        pub last_btcoex_mode: u8,
        pub h2c_count: i32,
        pub h2c: [host_pwrctrl_fw_ips_h2c; 4],
        pub read8_count: i32,
        pub read8_reg: [u8; 32],
        pub read8_val: [u8; 32],
        pub hwreg_count: i32,
        pub hwreg: [host_pwrctrl_fw_ips_hwreg; 16],
        pub final_tog: u8,
    }
}

#[cfg(host_pwrctrl_fw_ips_test)]
static mut G_TRACE: host_pwrctrl_fw_ips::host_pwrctrl_fw_ips_trace = unsafe { core::mem::zeroed() };

#[cfg(host_pwrctrl_fw_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_fw_ips_reset_trace() {
    host_hal::reset_trace();
}

#[cfg(host_pwrctrl_fw_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_fw_ips_get_trace(
) -> *mut host_pwrctrl_fw_ips::host_pwrctrl_fw_ips_trace {
    unsafe {
        G_TRACE.btcoex_ips_notify_calls = host_hal::btcoex_calls();
        G_TRACE.last_btcoex_mode = host_hal::last_btcoex();
        G_TRACE.h2c_count = host_hal::h2c_count();
        if G_TRACE.h2c_count > 0 {
            G_TRACE.h2c[0].cmd = H2C_INACTIVE_PS_;
            G_TRACE.h2c[0].len = H2C_INACTIVE_PS_LEN;
            G_TRACE.h2c[0].parm[0] = host_hal::h2c_parm0();
        }
        G_TRACE.read8_count = host_hal::read8_count();
        G_TRACE.hwreg_count = host_hal::hwreg_count();
        &mut G_TRACE
    }
}

#[cfg(host_pwrctrl_fw_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_fw_ips_set_hmetfr_sequence(vals: *const u8, n: i32) {
    if vals.is_null() || n <= 0 {
        return;
    }
    let slice = unsafe { core::slice::from_raw_parts(vals, n as usize) };
    host_hal::set_hmetfr(slice);
}

#[cfg(host_pwrctrl_fw_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_fw_ips_set_reg_cr_sequence(vals: *const u8, n: i32) {
    if vals.is_null() || n <= 0 {
        return;
    }
    let slice = unsafe { core::slice::from_raw_parts(vals, n as usize) };
    host_hal::set_reg_cr(slice);
}

#[cfg(host_pwrctrl_fw_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_fw_ips_set_cpwm_leave(cpwm_orig: u8, cpwm_after: u8, reads: u32) {
    host_hal::set_cpwm_leave(cpwm_orig, cpwm_after, reads);
}

#[cfg(host_pwrctrl_fw_ips_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_fw_ips_set_time_ms(ms: u32) {
    host_hal::set_time_ms(ms);
}
