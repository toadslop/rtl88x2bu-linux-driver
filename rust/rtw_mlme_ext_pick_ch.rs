// SPDX-License-Identifier: GPL-2.0
//! W3-71 sitesurvey_pick_ch_behavior — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    unreachable_pub,
    missing_docs
)]

#[cfg(host_mlme_ext_pick_ch_test)]
use std::os::raw::{c_int, c_ulong, c_void};

#[cfg(rust_mlme_ext_pick_ch)]
use core::ffi::{c_int, c_ulong, c_void};

type U8 = u8;
type U32 = u32;
type Systime = c_ulong;
type Adapter = *mut c_void;

const _TRUE: c_int = 1;
const SCAN_PASSIVE: c_int = 0;
const SCAN_ACTIVE: c_int = 1;
const SCAN_PROCESS: U8 = 4;
const SCAN_BACKING_OP: U8 = 5;
const SCAN_TO_P2P_LISTEN: U8 = 10;
const SCAN_COMPLETE: U8 = 12;
const RTW_IEEE80211_CHAN_PASSIVE_SCAN: U32 = 1 << 1;
const RTW_CHF_NO_IR: U8 = 1 << 0;

extern "C" {
    fn rtw_scan_backop_decision(a: Adapter) -> U8;
    fn rtw_chset_search_ch(ch_set: *mut c_void, ch: U32) -> c_int;
    fn rtw_rfctl_dfs_domain_unknown(rfctl: *mut c_void) -> U8;
    fn rtw_rust_pick_ch_scan_abort(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_set_channel_idx(a: Adapter, idx: c_int);
    fn rtw_rust_pick_ch_channel_idx(a: Adapter) -> c_int;
    fn rtw_rust_pick_ch_force_ssid_scan(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_set_force_ssid_scan(a: Adapter, v: U8);
    fn rtw_rust_pick_ch_ssid_num(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_ch_num(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_ch_hw_value(a: Adapter, idx: c_int) -> u16;
    fn rtw_rust_pick_ch_ch_flags(a: Adapter, idx: c_int) -> U32;
    fn rtw_rust_pick_ch_scan_cnt(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_set_scan_cnt(a: Adapter, v: U8);
    fn rtw_rust_pick_ch_scan_cnt_max(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_set_backop_flags(a: Adapter, v: U8);
    fn rtw_rust_pick_ch_channel_set(a: Adapter) -> *mut c_void;
    fn rtw_rust_pick_ch_chset_flags(a: Adapter, idx: c_int) -> U8;
    fn rtw_rust_pick_ch_hidden_bss_cnt(a: Adapter, idx: c_int) -> U8;
    fn rtw_rust_pick_ch_non_ocp_end_time(a: Adapter, idx: c_int) -> Systime;
    fn rtw_rust_pick_ch_dfs_slave_with_rd(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_rx_scan_op_ch_only(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_p2p_scan_op_ch_only(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_rx_op_ch(a: Adapter, idx: c_int) -> U8;
    fn rtw_rust_pick_ch_p2p_op_ch(a: Adapter, idx: c_int) -> U8;
    fn rtw_rust_pick_ch_social_chan(a: Adapter, idx: c_int) -> U8;
    fn rtw_rust_pick_ch_p2p_social(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_p2p_needed(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_rfctl(a: Adapter) -> *mut c_void;
    fn rtw_rust_pick_ch_p2p_state_not_none(a: Adapter) -> U8;
    fn rtw_rust_pick_ch_p2p_findphase_ex_max(a: Adapter);
}

#[cfg(host_mlme_ext_pick_ch_test)]
extern "C" {
    fn host_scan_current_time() -> Systime;
}

#[cfg(rust_mlme_ext_pick_ch)]
extern "C" {
    fn _rtw_get_current_time() -> Systime;
}

fn ch_is_non_ocp(a: Adapter, idx: c_int) -> bool {
    let end = unsafe { rtw_rust_pick_ch_non_ocp_end_time(a, idx) };
    unsafe {
        #[cfg(host_mlme_ext_pick_ch_test)]
        {
            end > host_scan_current_time()
        }
        #[cfg(rust_mlme_ext_pick_ch)]
        {
            end > _rtw_get_current_time()
        }
    }
}

fn scan_abort_hdl(a: Adapter) {
    if unsafe { rtw_rust_pick_ch_scan_abort(a) } == _TRUE as U8 {
        if unsafe { rtw_rust_pick_ch_p2p_state_not_none(a) } != 0 {
            unsafe {
                rtw_rust_pick_ch_p2p_findphase_ex_max(a);
                rtw_rust_pick_ch_set_channel_idx(a, 3);
            }
        } else {
            let ch_num = unsafe { rtw_rust_pick_ch_ch_num(a) };
            unsafe {
                rtw_rust_pick_ch_set_channel_idx(a, ch_num as c_int);
            }
        }
    }
}

fn pick_ch_impl(a: Adapter, ch_out: &mut U8, type_out: &mut c_int) -> U8 {
    scan_abort_hdl(a);

    let mut scan_ch: U8 = 0;
    let mut scan_type = SCAN_PASSIVE;
    let mut backop_flags: U8 = 0;

    let p2p_op = unsafe {
        rtw_rust_pick_ch_rx_scan_op_ch_only(a) != 0 || rtw_rust_pick_ch_p2p_scan_op_ch_only(a) != 0
    };
    let p2p_social_path = unsafe { rtw_rust_pick_ch_p2p_social(a) != 0 };

    if p2p_op {
        let idx = unsafe { rtw_rust_pick_ch_channel_idx(a) };
        scan_ch = unsafe {
            if rtw_rust_pick_ch_rx_scan_op_ch_only(a) != 0 {
                rtw_rust_pick_ch_rx_op_ch(a, idx)
            } else {
                rtw_rust_pick_ch_p2p_op_ch(a, idx)
            }
        };
        scan_type = SCAN_ACTIVE;
    } else if p2p_social_path {
        let idx = unsafe { rtw_rust_pick_ch_channel_idx(a) };
        scan_ch = unsafe { rtw_rust_pick_ch_social_chan(a, idx) };
        let ch_set = unsafe { rtw_rust_pick_ch_channel_set(a) };
        let ch_set_idx = unsafe { rtw_chset_search_ch(ch_set, scan_ch as U32) };
        scan_type = if ch_set_idx >= 0
            && (unsafe { rtw_rust_pick_ch_chset_flags(a, ch_set_idx) } & RTW_CHF_NO_IR) != 0
        {
            SCAN_PASSIVE
        } else {
            SCAN_ACTIVE
        };
    } else {
        backop_flags = unsafe { rtw_scan_backop_decision(a) };
        let scan_cnt = unsafe { rtw_rust_pick_ch_scan_cnt(a) };
        let scan_cnt_max = unsafe { rtw_rust_pick_ch_scan_cnt_max(a) };

        if !(backop_flags != 0 && scan_cnt >= scan_cnt_max) {
            let channel_idx = unsafe { rtw_rust_pick_ch_channel_idx(a) };
            let force_ssid_scan = unsafe { rtw_rust_pick_ch_force_ssid_scan(a) };
            let ssid_num = unsafe { rtw_rust_pick_ch_ssid_num(a) };
            if channel_idx != 0
                && force_ssid_scan == 0
                && ssid_num != 0
                && (unsafe { rtw_rust_pick_ch_ch_flags(a, channel_idx - 1) }
                    & RTW_IEEE80211_CHAN_PASSIVE_SCAN)
                    != 0
            {
                let prev = unsafe { rtw_rust_pick_ch_ch_hw_value(a, channel_idx - 1) as U32 };
                let ch_set = unsafe { rtw_rust_pick_ch_channel_set(a) };
                let ch_set_idx = unsafe { rtw_chset_search_ch(ch_set, prev) };
                if ch_set_idx != -1
                    && unsafe { rtw_rust_pick_ch_hidden_bss_cnt(a, ch_set_idx) } != 0
                    && (unsafe { rtw_rust_pick_ch_dfs_slave_with_rd(a) } == 0
                        || unsafe { rtw_rfctl_dfs_domain_unknown(rtw_rust_pick_ch_rfctl(a)) } != 0
                        || !ch_is_non_ocp(a, ch_set_idx))
                {
                    unsafe {
                        rtw_rust_pick_ch_set_channel_idx(a, channel_idx - 1);
                        rtw_rust_pick_ch_set_force_ssid_scan(a, 1);
                    }
                }
            } else {
                unsafe {
                    rtw_rust_pick_ch_set_force_ssid_scan(a, 0);
                }
            }
        }

        let channel_idx = unsafe { rtw_rust_pick_ch_channel_idx(a) };
        let ch_num = unsafe { rtw_rust_pick_ch_ch_num(a) };
        if channel_idx < ch_num as c_int {
            scan_ch = unsafe { rtw_rust_pick_ch_ch_hw_value(a, channel_idx) as U8 };
            scan_type = if (unsafe { rtw_rust_pick_ch_ch_flags(a, channel_idx) }
                & RTW_IEEE80211_CHAN_PASSIVE_SCAN)
                != 0
            {
                SCAN_PASSIVE
            } else {
                SCAN_ACTIVE
            };
        }
    }

    let next_state = if scan_ch != 0 {
        let mut state = SCAN_PROCESS;
        if backop_flags != 0 {
            let scan_cnt = unsafe { rtw_rust_pick_ch_scan_cnt(a) };
            let scan_cnt_max = unsafe { rtw_rust_pick_ch_scan_cnt_max(a) };
            if scan_cnt < scan_cnt_max {
                unsafe {
                    rtw_rust_pick_ch_set_scan_cnt(a, scan_cnt + 1);
                }
            } else {
                unsafe {
                    rtw_rust_pick_ch_set_backop_flags(a, backop_flags);
                }
                state = SCAN_BACKING_OP;
            }
        }
        state
    } else if unsafe { rtw_rust_pick_ch_p2p_needed(a) != 0 } {
        SCAN_TO_P2P_LISTEN
    } else {
        SCAN_COMPLETE
    };

    if next_state != SCAN_PROCESS {
        unsafe {
            rtw_rust_pick_ch_set_scan_cnt(a, 0);
        }
    }

    *ch_out = scan_ch;
    *type_out = scan_type;
    next_state
}

#[no_mangle]
pub extern "C" fn sitesurvey_pick_ch_behavior(
    padapter: Adapter,
    ch: *mut U8,
    scan_type: *mut c_int,
) -> U8 {
    if padapter.is_null() {
        return SCAN_COMPLETE;
    }
    let mut out_ch: U8 = 0;
    let mut out_type = SCAN_PASSIVE;
    let state = pick_ch_impl(padapter, &mut out_ch, &mut out_type);
    if !ch.is_null() {
        unsafe { *ch = out_ch };
    }
    if !scan_type.is_null() {
        unsafe { *scan_type = out_type };
    }
    state
}
