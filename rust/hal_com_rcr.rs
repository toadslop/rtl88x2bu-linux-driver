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

#[cfg(not(host_hal_com_rcr_test))]
mod chk_bssid {
    use super::*;

    const RCR_CBSSID_BCN: U32 = 1 << 7;
    const RCR_CBSSID_DATA: U32 = 1 << 6;
    const RCR_AAP: U32 = 1;
    const MLME_ACTION_NONE: U8 = 1;
    const MLME_SCAN_ENTER: U8 = 3;
    const MLME_SCAN_DONE: U8 = 4;
    const MLME_STA_CONNECTING: U8 = 6;
    const MLME_STA_CONNECTED: U8 = 7;
    const MLME_STA_DISCONNECTED: U8 = 8;
    const MAX_CLIENT_PORT_NUM: U8 = 4;

    /// Host L2 uses the flat fixture `struct mi_state`. Kernel `struct mi_state`
    /// grows with Kconfig fields — use an opaque stride from C.
    #[cfg(host_hal_com_rcr_chk_bssid_test)]
    #[repr(C)]
    struct MiState {
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

    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    #[repr(C, align(4))]
    struct MiState {
        raw: [u8; 32],
    }

    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    impl MiState {
        fn zeroed() -> Self {
            Self { raw: [0u8; 32] }
        }
    }

    #[cfg(host_hal_com_rcr_chk_bssid_test)]
    impl MiState {
        fn zeroed() -> Self {
            Self {
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
            }
        }
    }

    fn mi_ptr(m: &mut MiState) -> *mut MiState {
        m as *mut MiState
    }

    extern "C" {
        fn rtw_rust_hal_com_mi_status_no_self(adapter: *mut c_void, m: *mut MiState);
        fn rtw_rust_hal_com_mi_status_no_others(adapter: *mut c_void, m: *mut MiState);
        fn rtw_rust_hal_com_mi_status_merge(d: *mut MiState, a: *mut MiState);
        fn rtw_rust_hal_com_chk_bssid_apply_action(m: *mut MiState, self_action: U8);
        fn rtw_rust_hal_com_in_cta_test(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_com_rx_tsf_filter(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_com_periodic_tsf_update_etime(adapter: *mut c_void) -> U32;
        fn rtw_rust_hal_com_wifi_spec(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_com_get_clt_num(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_com_rcr_use_mbssid_shortcut() -> i32;
        fn rtw_rust_hal_com_tsf_update_pause_for_rcr(adapter: *mut c_void);
        fn rtw_rust_hal_com_tsf_update_restore_for_rcr(adapter: *mut c_void);
        #[cfg(config_rtw_multi_ap)]
        fn rtw_rust_hal_com_unassoc_sta_src_chk(adapter: *mut c_void, src: U8) -> U8;
        #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
        fn rtw_rust_mstate_ap_num(m: *mut MiState) -> U8;
        #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
        fn rtw_rust_mstate_mesh_num(m: *mut MiState) -> U8;
        #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
        fn rtw_rust_mstate_tdls_ld_num(m: *mut MiState) -> U8;
        #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
        fn rtw_rust_mstate_scan_enter_num(m: *mut MiState) -> U8;
        #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
        fn rtw_rust_mstate_sta_lg_num(m: *mut MiState) -> U8;
    }

    #[cfg(host_hal_com_rcr_chk_bssid_test)]
    #[inline]
    fn ap_num(m: &MiState) -> U8 {
        m.ap_num
    }
    #[cfg(host_hal_com_rcr_chk_bssid_test)]
    #[inline]
    fn mesh_num(m: &MiState) -> U8 {
        m.mesh_num
    }
    #[cfg(host_hal_com_rcr_chk_bssid_test)]
    #[inline]
    fn tdls_ld_num(m: &MiState) -> U8 {
        m.ld_tdls_num
    }
    #[cfg(host_hal_com_rcr_chk_bssid_test)]
    #[inline]
    fn scan_enter_num(m: &MiState) -> U8 {
        m.scan_enter_num
    }
    #[cfg(host_hal_com_rcr_chk_bssid_test)]
    #[inline]
    fn sta_lg_num(m: &MiState) -> U8 {
        m.lg_sta_num
    }

    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    fn mi_ro(m: &MiState) -> *mut MiState {
        m as *const MiState as *mut MiState
    }

    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    #[inline]
    fn ap_num(m: &MiState) -> U8 {
        unsafe { rtw_rust_mstate_ap_num(mi_ro(m)) }
    }
    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    #[inline]
    fn mesh_num(m: &MiState) -> U8 {
        unsafe { rtw_rust_mstate_mesh_num(mi_ro(m)) }
    }
    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    #[inline]
    fn tdls_ld_num(m: &MiState) -> U8 {
        unsafe { rtw_rust_mstate_tdls_ld_num(mi_ro(m)) }
    }
    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    #[inline]
    fn scan_enter_num(m: &MiState) -> U8 {
        unsafe { rtw_rust_mstate_scan_enter_num(mi_ro(m)) }
    }
    #[cfg(not(host_hal_com_rcr_chk_bssid_test))]
    #[inline]
    fn sta_lg_num(m: &MiState) -> U8 {
        unsafe { rtw_rust_mstate_sta_lg_num(mi_ro(m)) }
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
            let mut mstate = MiState::zeroed();
            let mut mstate_s = MiState::zeroed();
            unsafe {
                rtw_rust_hal_com_mi_status_no_self(adapter, mi_ptr(&mut mstate));
                rtw_rust_hal_com_mi_status_no_others(adapter, mi_ptr(&mut mstate_s));
                rtw_rust_hal_com_chk_bssid_apply_action(mi_ptr(&mut mstate_s), self_action);
                rtw_rust_hal_com_mi_status_merge(mi_ptr(&mut mstate), mi_ptr(&mut mstate_s));
            }

            let in_cta = unsafe { rtw_rust_hal_com_in_cta_test(adapter) };
            let mut clear_data = ap_num(&mstate) != 0
                || mesh_num(&mstate) != 0
                || tdls_ld_num(&mstate) != 0
                || in_cta != 0;
            #[cfg(config_find_best_channel)]
            {
                clear_data = clear_data || scan_enter_num(&mstate) != 0;
            }
            if clear_data {
                rcr_new &= !RCR_CBSSID_DATA;
            } else {
                rcr_new |= RCR_CBSSID_DATA;
            }

            let periodic = unsafe { rtw_rust_hal_com_periodic_tsf_update_etime(adapter) };
            if scan_enter_num(&mstate) != 0 || in_cta != 0 {
                rcr_new &= !RCR_CBSSID_BCN;
            } else if sta_lg_num(&mstate) != 0 || periodic != 0 {
                rcr_new |= RCR_CBSSID_BCN;
            } else if (ap_num(&mstate) != 0 && unsafe { rtw_rust_hal_com_wifi_spec(adapter) } != 0)
                || mesh_num(&mstate) != 0
            {
                rcr_new &= !RCR_CBSSID_BCN;
            } else {
                rcr_new |= RCR_CBSSID_BCN;
            }

            #[cfg(config_client_port_cfg)]
            if unsafe { rtw_rust_hal_com_get_clt_num(adapter) } > MAX_CLIENT_PORT_NUM {
                rcr_new &= !RCR_CBSSID_BCN;
            }
        }

        #[cfg(config_rtw_multi_ap)]
        {
            let mut mstate = MiState::zeroed();
            let mut mstate_s = MiState::zeroed();
            unsafe {
                rtw_rust_hal_com_mi_status_no_self(adapter, mi_ptr(&mut mstate));
                rtw_rust_hal_com_mi_status_no_others(adapter, mi_ptr(&mut mstate_s));
                rtw_rust_hal_com_chk_bssid_apply_action(mi_ptr(&mut mstate_s), self_action);
                rtw_rust_hal_com_mi_status_merge(mi_ptr(&mut mstate), mi_ptr(&mut mstate_s));
            }
            if ap_num(&mstate) != 0
                && unsafe { rtw_rust_hal_com_unassoc_sta_src_chk(adapter, 1) } != 0
            {
                rcr_new |= RCR_AAP;
            } else {
                rcr_new &= !RCR_AAP;
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
}