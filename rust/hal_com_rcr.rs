// SPDX-License-Identifier: GPL-2.0
// RCR check/add/clear leaf helpers (W4-127).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const SUCCESS: U8 = 1;

extern "C" {
    fn rtw_rust_hal_com_hw_var_rcr() -> i32;
    fn rtw_rust_hal_com_receive_config(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_com_get_hwreg(adapter: *mut c_void, variable: U32, val: *mut U8);
    fn rtw_rust_hal_com_set_hwreg_u8(adapter: *mut c_void, variable: U32, val: *mut U8) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_check(adapter: *mut c_void, check_bit: U32) -> U8 {
    let rcr = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    if (rcr & check_bit) == check_bit {
        1
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_add(adapter: *mut c_void, add: U32) -> U8 {
    let mut rcr: U32 = 0;
    let hw_var = unsafe { rtw_rust_hal_com_hw_var_rcr() as U32 };
    // SAFETY: C shim reads HW_VAR_RCR into a stack u32; adapter is a valid host fixture.
    unsafe { rtw_rust_hal_com_get_hwreg(adapter, hw_var, &mut rcr as *mut U32 as *mut U8) };
    rcr |= add;
    let after_get = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    if rcr != after_get {
        // SAFETY: &mut rcr is a properly aligned u32 backing store, matching C (u8 *)&rcr.
        return unsafe {
            rtw_rust_hal_com_set_hwreg_u8(adapter, hw_var, &mut rcr as *mut U32 as *mut U8)
        };
    }
    SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_clear(adapter: *mut c_void, clear: U32) -> U8 {
    let mut rcr: U32 = 0;
    let hw_var = unsafe { rtw_rust_hal_com_hw_var_rcr() as U32 };
    // SAFETY: C shim reads HW_VAR_RCR into a stack u32; adapter is a valid host fixture.
    unsafe { rtw_rust_hal_com_get_hwreg(adapter, hw_var, &mut rcr as *mut U32 as *mut U8) };
    rcr &= !clear;
    let after_get = unsafe { rtw_rust_hal_com_receive_config(adapter) };
    if rcr != after_get {
        // SAFETY: &mut rcr is a properly aligned u32 backing store, matching C (u8 *)&rcr.
        return unsafe {
            rtw_rust_hal_com_set_hwreg_u8(adapter, hw_var, &mut rcr as *mut U32 as *mut U8)
        };
    }
    SUCCESS
}

const RCR_CBSSID_BCN: U32 = 1 << 7;
const RCR_CBSSID_DATA: U32 = 1 << 6;
const MLME_ACTION_NONE: U8 = 1;
const MLME_SCAN_ENTER: U8 = 2;
const MLME_SCAN_DONE: U8 = 3;
const MLME_STA_CONNECTING: U8 = 5;
const MLME_STA_CONNECTED: U8 = 6;
const MLME_STA_DISCONNECTED: U8 = 7;
const MLME_ADHOC_STARTED: U8 = 12;
const MAX_CLIENT_PORT_NUM: U8 = 4;

#[repr(C)]
struct MiStateHost {
    sta_num: U8,
    ld_sta_num: U8,
    lg_sta_num: U8,
    ld_tdls_num: U8,
    ap_num: U8,
    starting_ap_num: U8,
    ld_ap_num: U8,
    adhoc_num: U8,
    ld_adhoc_num: U8,
    mesh_num: U8,
    ld_mesh_num: U8,
    scan_num: U8,
    scan_enter_num: U8,
    uwps_num: U8,
}

extern "C" {
    fn rtw_rust_hal_com_mi_status_no_self(adapter: *mut c_void, m: *mut MiStateHost);
    fn rtw_rust_hal_com_mi_status_no_others(adapter: *mut c_void, m: *mut MiStateHost);
    fn rtw_rust_hal_com_mi_status_merge(d: *mut MiStateHost, a: *mut MiStateHost);
    fn rtw_rust_hal_com_in_cta_test(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_rx_tsf_filter(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_periodic_tsf_update_etime(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_com_wifi_spec(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_get_clt_num(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_rcr_use_mbssid_shortcut() -> i32;
    fn rtw_rust_hal_com_warn_on(condition: i32);
    fn rtw_rust_hal_com_tsf_update_pause_for_rcr(adapter: *mut c_void);
    fn rtw_rust_hal_com_tsf_update_restore_for_rcr(adapter: *mut c_void);
}

fn adjust_mi_for_action(mstate_s: &mut MiStateHost, self_action: U8) {
    match self_action {
        MLME_SCAN_ENTER => {
            mstate_s.scan_num = 1;
            mstate_s.scan_enter_num = 1;
        }
        MLME_SCAN_DONE => mstate_s.scan_enter_num = 0,
        MLME_STA_CONNECTING => {
            mstate_s.lg_sta_num = 1;
            mstate_s.ld_sta_num = 0;
        }
        MLME_STA_CONNECTED => {
            mstate_s.lg_sta_num = 0;
            mstate_s.ld_sta_num = 1;
        }
        MLME_STA_DISCONNECTED => {
            mstate_s.lg_sta_num = 0;
            mstate_s.ld_sta_num = 0;
        }
        MLME_ACTION_NONE | MLME_ADHOC_STARTED => {}
        _ => unsafe { rtw_rust_hal_com_warn_on(1) },
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_set_chk_bssid(adapter: *mut c_void, self_action: U8) {
    if adapter.is_null() {
        return;
    }
    let mut rcr: U32 = 0;
    let hw_var = unsafe { rtw_rust_hal_com_hw_var_rcr() as U32 };
    unsafe { rtw_rust_hal_com_get_hwreg(adapter, hw_var, &mut rcr as *mut U32 as *mut U8) };
    let mut rcr_new = rcr;

    if unsafe { rtw_rust_hal_com_rcr_use_mbssid_shortcut() } != 0 {
        rcr_new &= !(RCR_CBSSID_BCN | RCR_CBSSID_DATA);
    } else {
        let mut mstate = MiStateHost {
            sta_num: 0,
            ld_sta_num: 0,
            lg_sta_num: 0,
            ld_tdls_num: 0,
            ap_num: 0,
            starting_ap_num: 0,
            ld_ap_num: 0,
            adhoc_num: 0,
            ld_adhoc_num: 0,
            mesh_num: 0,
            ld_mesh_num: 0,
            scan_num: 0,
            scan_enter_num: 0,
            uwps_num: 0,
        };
        let mut mstate_s = MiStateHost {
            sta_num: 0,
            ld_sta_num: 0,
            lg_sta_num: 0,
            ld_tdls_num: 0,
            ap_num: 0,
            starting_ap_num: 0,
            ld_ap_num: 0,
            adhoc_num: 0,
            ld_adhoc_num: 0,
            mesh_num: 0,
            ld_mesh_num: 0,
            scan_num: 0,
            scan_enter_num: 0,
            uwps_num: 0,
        };
        unsafe {
            rtw_rust_hal_com_mi_status_no_self(adapter, &mut mstate);
            rtw_rust_hal_com_mi_status_no_others(adapter, &mut mstate_s);
        }
        adjust_mi_for_action(&mut mstate_s, self_action);
        unsafe { rtw_rust_hal_com_mi_status_merge(&mut mstate, &mut mstate_s) };

        let in_cta = unsafe { rtw_rust_hal_com_in_cta_test(adapter) };
        let mut clear_data = mstate.ap_num != 0
            || mstate.mesh_num != 0
            || mstate.ld_tdls_num != 0
            || in_cta != 0;
        #[cfg(config_find_best_channel)]
        {
            clear_data = clear_data || mstate.scan_enter_num != 0;
        }
        if clear_data {
            rcr_new &= !RCR_CBSSID_DATA;
        } else {
            rcr_new |= RCR_CBSSID_DATA;
        }

        let periodic = unsafe { rtw_rust_hal_com_periodic_tsf_update_etime(adapter) };
        if mstate.scan_enter_num != 0 || in_cta != 0 {
            rcr_new &= !RCR_CBSSID_BCN;
        } else if mstate.lg_sta_num != 0 || periodic != 0 {
            rcr_new |= RCR_CBSSID_BCN;
        } else if (mstate.ap_num != 0 && unsafe { rtw_rust_hal_com_wifi_spec(adapter) } != 0)
            || mstate.mesh_num != 0
        {
            rcr_new &= !RCR_CBSSID_BCN;
        } else {
            rcr_new |= RCR_CBSSID_BCN;
        }

        if unsafe { rtw_rust_hal_com_get_clt_num(adapter) } > MAX_CLIENT_PORT_NUM {
            rcr_new &= !RCR_CBSSID_BCN;
        }
    }

    if rcr == rcr_new {
        return;
    }

    let rx_tsf = unsafe { rtw_rust_hal_com_rx_tsf_filter(adapter) };
    if rx_tsf == 0 && (rcr & RCR_CBSSID_BCN) != 0 && (rcr_new & RCR_CBSSID_BCN) == 0 {
        unsafe { rtw_rust_hal_com_tsf_update_pause_for_rcr(adapter) };
    }
    unsafe {
        rtw_rust_hal_com_set_hwreg_u8(adapter, hw_var, &mut rcr_new as *mut U32 as *mut U8);
    }
    if rx_tsf == 0
        && (rcr & RCR_CBSSID_BCN) == 0
        && (rcr_new & RCR_CBSSID_BCN) != 0
        && self_action != MLME_STA_CONNECTING
    {
        unsafe { rtw_rust_hal_com_tsf_update_restore_for_rcr(adapter) };
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_rcr_set_chk_bssid_act_non(adapter: *mut c_void) {
    rtw_hal_rcr_set_chk_bssid(adapter, MLME_ACTION_NONE);
}