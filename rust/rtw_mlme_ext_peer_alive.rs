// SPDX-License-Identifier: GPL-2.0
//! W3-69 peer-alive / delba helpers — kernel port (host oracle in PR5).

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    unreachable_pub,
    missing_docs
)]

use core::ffi::{c_int, c_void};

type U8 = u8;
type U16 = u16;
type U64 = u64;
type Adapter = c_void;
type StaInfo = c_void;

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const _SUCCESS: c_int = 0;
const _FAIL: c_int = -1;
const TID_NUM: usize = 16;
const HT_IOT_PEER_BROADCOM: U8 = 3;

extern "C" {
    fn issue_del_ba(a: *mut Adapter, ra: *mut U8, tid: U8, reason: U16, ini: U8);
    fn issue_del_ba_ex(
        a: *mut Adapter,
        ra: *mut U8,
        tid: U8,
        reason: U16,
        ini: U8,
        tc: c_int,
        wm: c_int,
    ) -> c_int;
    fn rtw_inc_and_chk_continual_no_rx_packet(s: *mut StaInfo, tid: c_int) -> c_int;
    fn rtw_reset_continual_no_rx_packet(s: *mut StaInfo, tid: c_int);
    fn rtw_rust_peer_assoc_ap_vendor(a: *mut Adapter) -> U8;
    fn rtw_rust_peer_sta_mac(s: *mut StaInfo) -> *mut U8;
    fn rtw_rust_peer_reorder_enable(s: *mut StaInfo, tid: c_int) -> U8;
    fn rtw_rust_peer_reorder_disable(s: *mut StaInfo, tid: c_int);
    fn rtw_rust_peer_reorder_invalidate_ampdu(s: *mut StaInfo, tid: c_int);
    fn rtw_rust_peer_rx_qos(s: *mut StaInfo, tid: c_int) -> U64;
    fn rtw_rust_peer_last_rx_qos(s: *mut StaInfo, tid: c_int) -> U64;
    fn rtw_rust_peer_rx_data(s: *mut StaInfo) -> U64;
    fn rtw_rust_peer_last_rx_data(s: *mut StaInfo) -> U64;
    fn rtw_rust_peer_rx_beacon(s: *mut StaInfo) -> U64;
    fn rtw_rust_peer_last_rx_beacon(s: *mut StaInfo) -> U64;
    fn rtw_rust_peer_rx_probersp(s: *mut StaInfo) -> U64;
    fn rtw_rust_peer_last_rx_probersp(s: *mut StaInfo) -> U64;
    fn rtw_rust_peer_sta_update_last_rx(s: *mut StaInfo);
    #[cfg(config_tdls)]
    fn rtw_rust_peer_rx_tdls_disc(s: *mut StaInfo) -> U64;
    #[cfg(config_tdls)]
    fn rtw_rust_peer_last_rx_tdls_disc(s: *mut StaInfo) -> U64;
}

unsafe fn kernel_rx_alive(sta: *mut StaInfo) -> bool {
    unsafe {
        !(rtw_rust_peer_rx_data(sta) == rtw_rust_peer_last_rx_data(sta)
            && rtw_rust_peer_rx_beacon(sta) == rtw_rust_peer_last_rx_beacon(sta)
            && rtw_rust_peer_rx_probersp(sta) == rtw_rust_peer_last_rx_probersp(sta))
    }
}

unsafe fn peer_rx_alive(psta: *mut StaInfo, null_ret: U8) -> U8 {
    if psta.is_null() {
        return null_ret;
    }
    let alive = unsafe { kernel_rx_alive(psta) };
    unsafe {
        rtw_rust_peer_sta_update_last_rx(psta);
    }
    if alive {
        _TRUE as U8
    } else {
        _FALSE as U8
    }
}

#[no_mangle]
pub extern "C" fn chk_ap_is_alive(padapter: *mut Adapter, psta: *mut StaInfo) -> U8 {
    let _ = padapter;
    unsafe { peer_rx_alive(psta, _FALSE as U8) }
}

#[no_mangle]
pub extern "C" fn chk_adhoc_peer_is_alive(psta: *mut StaInfo) -> U8 {
    unsafe { peer_rx_alive(psta, _TRUE as U8) }
}

#[cfg(config_tdls)]
#[no_mangle]
pub extern "C" fn chk_tdls_peer_sta_is_alive(padapter: *mut Adapter, psta: *mut StaInfo) -> U8 {
    let _ = padapter;
    if psta.is_null() {
        return _TRUE as U8;
    }
    let alive = unsafe {
        !(rtw_rust_peer_rx_data(psta) == rtw_rust_peer_last_rx_data(psta)
            && rtw_rust_peer_rx_tdls_disc(psta) == rtw_rust_peer_last_rx_tdls_disc(psta))
    };
    if alive {
        _TRUE as U8
    } else {
        _FALSE as U8
    }
}

#[no_mangle]
pub extern "C" fn rtw_delba_check(padapter: *mut Adapter, psta: *mut StaInfo, from_timer: U8) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    if unsafe { rtw_rust_peer_assoc_ap_vendor(padapter) } != HT_IOT_PEER_BROADCOM {
        return;
    }
    for i in 0..TID_NUM {
        let tid = i as c_int;
        let (en, rx, last) = unsafe {
            (
                rtw_rust_peer_reorder_enable(psta, tid) != 0,
                rtw_rust_peer_rx_qos(psta, tid),
                rtw_rust_peer_last_rx_qos(psta, tid),
            )
        };
        if en && rx == last && unsafe { rtw_inc_and_chk_continual_no_rx_packet(psta, tid) } == _TRUE
        {
            let mac = unsafe { rtw_rust_peer_sta_mac(psta) };
            let ret = if from_timer == 0 {
                unsafe { issue_del_ba_ex(padapter, mac, i as U8, 39, 0, 3, 1) }
            } else {
                unsafe { issue_del_ba(padapter, mac, i as U8, 39, 0) };
                _SUCCESS
            };
            unsafe {
                rtw_rust_peer_reorder_disable(psta, tid);
                if ret != _FAIL {
                    rtw_rust_peer_reorder_invalidate_ampdu(psta, tid);
                }
                rtw_reset_continual_no_rx_packet(psta, tid);
            }
        } else {
            unsafe { rtw_reset_continual_no_rx_packet(psta, tid) };
        }
    }
}
