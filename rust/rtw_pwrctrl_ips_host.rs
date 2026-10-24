// SPDX-License-Identifier: GPL-2.0
//! Host L2 build of W3-167 IPS helpers (pairs with tests/host/pwrctrl).

#![allow(non_snake_case, non_camel_case_types)]

use std::ffi::c_void;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const _SUCCESS: i32 = 1;
const RF_OFF: u8 = 2;
const RF_ON: u8 = 0;
const IPS_LEVEL_2: u8 = 2;
const IPS_NONE: u8 = 0;

#[repr(C)]
pub struct PwrctrlPriv {
    pub lock: i32,
    pub bips_processing: u32,
    pub ips_mode: u8,
    pub ips_mode_req: u8,
    pub ips_enter_cnts: u32,
    pub ips_leave_cnts: u32,
    pub change_rfpwrstate: u8,
    pub bpower_saving: u8,
    pub bkeepfwalive: u8,
    pub rf_pwrstate: u8,
}

#[repr(C)]
pub struct HostAdapter {
    pub adapter_type: u8,
    pub pwrctrlpriv: PwrctrlPriv,
}

#[repr(C)]
pub struct HostIpsTrace {
    pub ips_pwr_down: u32,
    pub ips_pwr_up: u32,
    pub ips_pwr_up_result: i32,
    pub btcoex_notify: u32,
    pub last_btcoex_mode: u8,
    pub is_primary: u8,
}

static mut G_TRACE: HostIpsTrace = HostIpsTrace {
    ips_pwr_down: 0,
    ips_pwr_up: 0,
    ips_pwr_up_result: _SUCCESS,
    btcoex_notify: 0,
    last_btcoex_mode: 0,
    is_primary: 1,
};

type Padapter = *mut HostAdapter;

fn pwr(a: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*a).pwrctrlpriv }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ips_reset_trace() {
    unsafe {
        G_TRACE = HostIpsTrace {
            ips_pwr_down: 0,
            ips_pwr_up: 0,
            ips_pwr_up_result: _SUCCESS,
            btcoex_notify: 0,
            last_btcoex_mode: 0,
            is_primary: 1,
        };
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ips_set_pwr_up_result(r: i32) {
    unsafe {
        G_TRACE.ips_pwr_up_result = r;
    }
}

#[no_mangle]
pub extern "C" fn host_pwrctrl_ips_get_trace() -> *mut HostIpsTrace {
    std::ptr::addr_of_mut!(G_TRACE)
}

#[no_mangle]
pub extern "C" fn _ips_enter(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let p = pwr(padapter);
        (*p).bips_processing = _TRUE as u32;
        (*p).ips_mode = (*p).ips_mode_req;
        (*p).ips_enter_cnts += 1;
        if (*p).change_rfpwrstate == RF_OFF {
            (*p).bpower_saving = _TRUE;
            if (*p).ips_mode == IPS_LEVEL_2 {
                (*p).bkeepfwalive = _TRUE;
            }
            G_TRACE.ips_pwr_down += 1;
            (*p).rf_pwrstate = RF_OFF;
        }
        (*p).bips_processing = _FALSE as u32;
    }
}

#[no_mangle]
pub extern "C" fn ips_enter(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let mode = (*pwr(padapter)).ips_mode_req;
        G_TRACE.btcoex_notify += 1;
        G_TRACE.last_btcoex_mode = mode;
        _ips_enter(padapter);
    }
}

#[no_mangle]
pub extern "C" fn _ips_leave(padapter: Padapter) -> i32 {
    if padapter.is_null() {
        return _SUCCESS;
    }
    unsafe {
        let p = pwr(padapter);
        if (*p).rf_pwrstate == RF_OFF && (*p).bips_processing == 0 {
            (*p).bips_processing = _TRUE as u32;
            (*p).change_rfpwrstate = RF_ON;
            (*p).ips_leave_cnts += 1;
            G_TRACE.ips_pwr_up += 1;
            let result = G_TRACE.ips_pwr_up_result;
            if result == _SUCCESS {
                (*p).rf_pwrstate = RF_ON;
            }
            (*p).bips_processing = _FALSE as u32;
            (*p).bkeepfwalive = _FALSE;
            (*p).bpower_saving = _FALSE;
            result
        } else {
            _SUCCESS
        }
    }
}

#[no_mangle]
pub extern "C" fn ips_leave(padapter: Padapter) -> i32 {
    if padapter.is_null() || unsafe { (*padapter).adapter_type != 0 } {
        return _SUCCESS;
    }
    _ips_leave(padapter)
}
