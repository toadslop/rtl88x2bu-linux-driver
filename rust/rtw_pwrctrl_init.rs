// SPDX-License-Identifier: GPL-2.0
//! W3-189 / W3-190 — `rtw_init_pwrctrl_priv` from `core/rtw_pwrctrl.c`.

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

pub const _TRUE: u8 = 1;
pub const _FALSE: u8 = 0;
pub const PS_MODE_ACTIVE: u8 = 0;
pub const PS_STATE_S4: u8 = (1 << 3) | (1 << 2);
pub const rf_on: u8 = 0;
pub const RTW_PWR_STATE_CHK_INTERVAL: u32 = 2000;

pub type Padapter = *mut c_void;

#[repr(C)]
pub struct HostInitTrace {
    pub set_rpwm_hwreg_calls: u32,
    pub init_workitem_calls: u32,
    pub init_timer_calls: u32,
    pub create_wq_calls: u32,
    pub skipped_non_primary: u8,
    pub wow_pattern_reset: u32,
    pub flush_wq: u32,
    pub destroy_wq: u32,
    pub lps_poff_deinit: u32,
    pub cancel_workitem: u32,
    pub rsvd_page_free: u32,
    pub free_pwrlock: u32,
    pub lps_ack_mutex_free: u32,
    pub lps_ack_mutex_init: u32,
    pub lps_poff_init: u32,
}

#[cfg(host_pwrctrl_init_test)]
#[repr(C)]
struct RegistryPriv {
    ips_mode: u8,
    lps_level: u8,
    mp_mode: u8,
    power_mgnt: u8,
    smart_ps: u8,
    wow_power_mgnt: u8,
    wow_lps_level: u8,
    wakeup_event: u32,
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
    wowlan_power_mgmt: u8,
    wowlan_lps_level: u8,
    default_patterns_en: u8,
    wowlan_aoac_rpt_loc: u8,
    lps_ack_status: i8,
    cpwm_event: *mut c_void,
    dma_event: *mut c_void,
    resume_work: *mut c_void,
    pwr_state_check_timer: *mut c_void,
    lpspg_info_name: *const u8,
    lps_ack_mutex: i32,
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
    wow_pattern_reset: 0,
    flush_wq: 0,
    destroy_wq: 0,
    lps_poff_deinit: 0,
    cancel_workitem: 0,
    rsvd_page_free: 0,
    free_pwrlock: 0,
    lps_ack_mutex_free: 0,
    lps_ack_mutex_init: 0,
    lps_poff_init: 0,
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
            p.rtw_workqueue = 1 as *mut c_void;
        }
    }
}

fn init_part2_host(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_pwrctrl_init_test)]
    unsafe {
        let ad = padapter as *mut HostAdapter;
        #[cfg(concurrent_mode)]
        if (*ad).adapter_type != 0 {
            return;
        }
        let reg = &(*ad).registrypriv;
        let p = &mut (*ad).pwrctrlpriv;
        #[cfg(wowlan)]
        {
            p.wowlan_power_mgmt = reg.wow_power_mgnt;
            p.wowlan_lps_level = reg.wow_lps_level;
            p.default_patterns_en = if reg.wakeup_event & (1 << 1) != 0 {
                _TRUE
            } else {
                _FALSE
            };
            G_TRACE.wow_pattern_reset += 1;
            p.wowlan_aoac_rpt_loc = 0;
        }
        #[cfg(lps_ack)]
        {
            p.lps_ack_mutex = 0;
            G_TRACE.lps_ack_mutex_init += 1;
            p.lps_ack_status = -1;
        }
        #[cfg(lps_poff)]
        {
            G_TRACE.lps_poff_init += 1;
        }
    }
}

fn free_pwrctrl_host(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_pwrctrl_init_test)]
    unsafe {
        let ad = padapter as *mut HostAdapter;
        #[cfg(concurrent_mode)]
        if (*ad).adapter_type != 0 {
            return;
        }
        let p = &mut (*ad).pwrctrlpriv;
        #[cfg(resume_wq)]
        if !p.rtw_workqueue.is_null() {
            G_TRACE.flush_wq += 1;
            G_TRACE.destroy_wq += 1;
            p.rtw_workqueue = core::ptr::null_mut();
        }
        #[cfg(lps_poff)]
        {
            G_TRACE.lps_poff_deinit += 1;
        }
        #[cfg(lps_lclk)]
        {
            G_TRACE.cancel_workitem += 2;
        }
        #[cfg(lps_pg)]
        {
            G_TRACE.rsvd_page_free += 1;
        }
        G_TRACE.free_pwrlock += 2;
        #[cfg(lps_ack)]
        {
            G_TRACE.lps_ack_mutex_free += 1;
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
            wow_pattern_reset: 0,
            flush_wq: 0,
            destroy_wq: 0,
            lps_poff_deinit: 0,
            cancel_workitem: 0,
            rsvd_page_free: 0,
            free_pwrlock: 0,
            lps_ack_mutex_free: 0,
            lps_ack_mutex_init: 0,
            lps_poff_init: 0,
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

#[cfg(not(host_pwrctrl_init_test))]
#[path = "rtw_pwrctrl_init_kernel.rs"]
mod kernel;

#[no_mangle]
pub extern "C" fn rtw_init_pwrctrl_priv_part1(padapter: Padapter) {
    #[cfg(host_pwrctrl_init_test)]
    init_part1_host(padapter);
    #[cfg(not(host_pwrctrl_init_test))]
    kernel::init_part1_kernel(padapter);
}

#[cfg(host_pwrctrl_init_test)]
#[no_mangle]
pub extern "C" fn rtw_init_pwrctrl_priv_part2(padapter: Padapter) {
    init_part2_host(padapter);
}

#[cfg(host_pwrctrl_init_test)]
#[no_mangle]
pub extern "C" fn rtw_free_pwrctrl_priv(padapter: Padapter) {
    free_pwrctrl_host(padapter);
}
