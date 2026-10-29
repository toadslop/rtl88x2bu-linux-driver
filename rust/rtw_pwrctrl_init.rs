// SPDX-License-Identifier: GPL-2.0
//! W3-189 — `rtw_init_pwrctrl_priv` part 1 from `core/rtw_pwrctrl.c`.

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

#[cfg(host_pwrctrl_init_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_init_test))]
use core::ffi::c_void;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const PS_MODE_ACTIVE: u8 = 0;
const PS_STATE_S4: u8 = (1 << 3) | (1 << 2);
const rf_on: u8 = 0;
const RTW_PWR_STATE_CHK_INTERVAL: u32 = 2000;

type Padapter = *mut c_void;

#[repr(C)]
pub struct HostInitTrace {
    pub set_rpwm_hwreg_calls: u32,
    pub init_workitem_calls: u32,
    pub init_timer_calls: u32,
    pub create_wq_calls: u32,
    pub skipped_non_primary: u8,
}

#[cfg(host_pwrctrl_init_test)]
#[repr(C)]
struct RegistryPriv {
    ips_mode: u8,
    lps_level: u8,
    mp_mode: u8,
    power_mgnt: u8,
    smart_ps: u8,
}

#[cfg(host_pwrctrl_init_test)]
#[repr(C)]
struct PwrctrlPriv {
    lock: i32,
    check_32k_lock: i32,
    rf_pwrstate: u8,
    ips_enter_cnts: u32,
    ips_leave_cnts: u32,
    lps_enter_cnts: u32,
    lps_leave_cnts: u32,
    bips_processing: u32,
    ips_mode: u8,
    ips_mode_req: u8,
    ips_deny_time: u32,
    lps_level: u8,
    pwr_state_check_interval: u32,
    pwr_state_check_cnts: u32,
    bInSuspend: u8,
    bkeepfwalive: u8,
    LpsIdleCount: u8,
    power_mgnt: u8,
    bLeisurePs: u8,
    bFwCurrentInPSMode: u8,
    lps_deny_time: u32,
    rpwm: u8,
    cpwm: u8,
    pwr_mode: u8,
    smart_ps: u8,
    bcn_ant_mode: u8,
    dtim: u8,
    tog: u8,
    rpwm_retry: u8,
    wowlan_mode: u8,
    wowlan_ap_mode: u8,
    wowlan_p2p_mode: u8,
    wowlan_in_resume: u8,
    wowlan_last_wake_reason: u8,
    cpwm_event: *mut c_void,
    dma_event: *mut c_void,
    resume_work: *mut c_void,
    pwr_state_check_timer: *mut c_void,
    lpspg_info_name: *const u8,
    rtw_workqueue: *mut c_void,
}

#[cfg(host_pwrctrl_init_test)]
#[repr(C)]
struct HostAdapter {
    adapter_type: u8,
    registrypriv: RegistryPriv,
    pwrctrlpriv: PwrctrlPriv,
}

#[cfg(host_pwrctrl_init_test)]
static mut G_TIME: u32 = 0;
#[cfg(host_pwrctrl_init_test)]
static mut G_TRACE: HostInitTrace = HostInitTrace {
    set_rpwm_hwreg_calls: 0,
    init_workitem_calls: 0,
    init_timer_calls: 0,
    create_wq_calls: 0,
    skipped_non_primary: 0,
};

#[cfg(host_pwrctrl_init_test)]
static LPSPG_NAME: &[u8] = b"LPSPG_INFO\0";

fn init_part1_host(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }

    #[cfg(host_pwrctrl_init_test)]
    unsafe {
        let ad = padapter as *mut HostAdapter;
        #[cfg(concurrent_mode)]
        if (*ad).adapter_type != 0 {
            G_TRACE.skipped_non_primary = 1;
            return;
        }
        let reg = &(*ad).registrypriv;
        let p = &mut (*ad).pwrctrlpriv;
        p.lock = 0;
        p.check_32k_lock = 0;
        p.rf_pwrstate = rf_on;
        p.ips_enter_cnts = 0;
        p.ips_leave_cnts = 0;
        p.lps_enter_cnts = 0;
        p.lps_leave_cnts = 0;
        p.bips_processing = _FALSE as u32;
        p.ips_mode = reg.ips_mode;
        p.ips_mode_req = reg.ips_mode;
        p.ips_deny_time = G_TIME;
        p.lps_level = reg.lps_level;
        p.pwr_state_check_interval = RTW_PWR_STATE_CHK_INTERVAL;
        p.pwr_state_check_cnts = 0;
        p.bInSuspend = _FALSE;
        p.bkeepfwalive = _FALSE;
        p.LpsIdleCount = 0;
        p.power_mgnt = if reg.mp_mode == 1 {
            PS_MODE_ACTIVE
        } else {
            reg.power_mgnt
        };
        p.bLeisurePs = if p.power_mgnt != PS_MODE_ACTIVE {
            _TRUE
        } else {
            _FALSE
        };
        p.bFwCurrentInPSMode = _FALSE;
        p.lps_deny_time = G_TIME;
        p.rpwm = 0;
        p.cpwm = PS_STATE_S4;
        p.pwr_mode = PS_MODE_ACTIVE;
        p.smart_ps = reg.smart_ps;
        p.bcn_ant_mode = 0;
        p.dtim = 0;
        p.tog = 0x80;
        p.rpwm_retry = 0;
        #[cfg(lps_lclk)]
        {
            G_TRACE.set_rpwm_hwreg_calls += 1;
            G_TRACE.init_workitem_calls += 1;
            G_TRACE.init_workitem_calls += 1;
        }
        #[cfg(lps_pg)]
        {
            p.lpspg_info_name = LPSPG_NAME.as_ptr();
        }
        G_TRACE.init_timer_calls += 1;
        p.wowlan_mode = _FALSE;
        p.wowlan_ap_mode = _FALSE;
        p.wowlan_p2p_mode = _FALSE;
        p.wowlan_in_resume = _FALSE;
        p.wowlan_last_wake_reason = 0;
        #[cfg(resume_wq)]
        {
            G_TRACE.init_workitem_calls += 1;
            G_TRACE.create_wq_calls += 1;
        }
    }
}

#[cfg(host_pwrctrl_init_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_init_reset_trace() {
    unsafe {
        G_TRACE = HostInitTrace {
            set_rpwm_hwreg_calls: 0,
            init_workitem_calls: 0,
            init_timer_calls: 0,
            create_wq_calls: 0,
            skipped_non_primary: 0,
        };
    }
}

#[cfg(host_pwrctrl_init_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_init_get_trace() -> *mut HostInitTrace {
    unsafe { &mut G_TRACE }
}

#[cfg(host_pwrctrl_init_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_init_set_current_time(t: u32) {
    unsafe {
        G_TIME = t;
    }
}

#[no_mangle]
pub extern "C" fn rtw_init_pwrctrl_priv_part1(padapter: Padapter) {
    init_part1_host(padapter);
}
