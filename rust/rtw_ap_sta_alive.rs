// SPDX-License-Identifier: GPL-2.0
//! W3-82 `chk_sta_is_alive` — Rust port of `core/rtw_ap_sta_alive.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

type U8 = u8;
type U64 = u64;
type StaInfo = c_void;
type Adapter = c_void;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;

#[cfg(host_ap_sta_alive_test)]
#[repr(C)]
pub struct StainfoStats {
    pub rx_ctrl_pkts: U64,
    pub last_rx_ctrl_pkts: U64,
    pub rx_data_pkts: U64,
    pub last_rx_data_pkts: U64,
}

#[cfg(host_ap_sta_alive_test)]
#[repr(C)]
pub struct StaInfoHost {
    pub sta_stats: StainfoStats,
}

#[cfg(not(host_ap_sta_alive_test))]
extern "C" {
    fn rtw_rust_ap_sta_alive_rx_data(sta: *mut StaInfo) -> U64;
    fn rtw_rust_ap_sta_alive_last_rx_data(sta: *mut StaInfo) -> U64;
    fn rtw_rust_ap_sta_alive_rx_ctrl(sta: *mut StaInfo) -> U64;
    fn rtw_rust_ap_sta_alive_last_rx_ctrl(sta: *mut StaInfo) -> U64;
    fn rtw_rust_ap_sta_alive_update_last_rx(sta: *mut StaInfo);
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_sta_alive_adapter(sta: *mut StaInfo) -> *mut Adapter;
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_mlme_is_mesh(adapter: *mut Adapter) -> U8;
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_sta_alive_rx_hwmp(sta: *mut StaInfo) -> U64;
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_sta_alive_last_rx_hwmp(sta: *mut StaInfo) -> U64;
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_sta_alive_rx_beacon(sta: *mut StaInfo) -> U64;
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_sta_alive_last_rx_beacon(sta: *mut StaInfo) -> U64;
    #[cfg(config_rtw_mesh)]
    fn rtw_rust_ap_sta_alive_set_alive(sta: *mut StaInfo, alive: U8);
}

#[cfg(host_ap_sta_alive_test)]
fn update_last_rx_host(sta: &mut StaInfoHost) {
    sta.sta_stats.last_rx_ctrl_pkts = sta.sta_stats.rx_ctrl_pkts;
    sta.sta_stats.last_rx_data_pkts = sta.sta_stats.rx_data_pkts;
}

#[cfg(not(host_ap_sta_alive_test))]
fn chk_sta_is_alive_kernel(psta: *mut StaInfo) -> U8 {
    unsafe {
        let last_sum =
            rtw_rust_ap_sta_alive_last_rx_data(psta) + rtw_rust_ap_sta_alive_last_rx_ctrl(psta);
        let rx_sum = rtw_rust_ap_sta_alive_rx_data(psta) + rtw_rust_ap_sta_alive_rx_ctrl(psta);
        let mut ret = if last_sum == rx_sum { _FALSE } else { _TRUE };

        #[cfg(config_rtw_mesh)]
        {
            let adapter = rtw_rust_ap_sta_alive_adapter(psta);
            if !adapter.is_null() && rtw_rust_ap_mlme_is_mesh(adapter) != 0 {
                let hwmp_alive =
                    rtw_rust_ap_sta_alive_rx_hwmp(psta) != rtw_rust_ap_sta_alive_last_rx_hwmp(psta);
                let bcn_alive = rtw_rust_ap_sta_alive_rx_beacon(psta)
                    != rtw_rust_ap_sta_alive_last_rx_beacon(psta);
                rtw_rust_ap_sta_alive_set_alive(psta, ret | hwmp_alive | bcn_alive);
                ret |= hwmp_alive;
            }
        }

        rtw_rust_ap_sta_alive_update_last_rx(psta);
        ret
    }
}

#[cfg(host_ap_sta_alive_test)]
fn chk_sta_is_alive_host(psta: *mut StaInfoHost) -> U8 {
    if psta.is_null() {
        return _FALSE;
    }
    let sta = unsafe { &mut *psta };
    let last_sum = sta.sta_stats.last_rx_data_pkts + sta.sta_stats.last_rx_ctrl_pkts;
    let rx_sum = sta.sta_stats.rx_data_pkts + sta.sta_stats.rx_ctrl_pkts;
    let ret = if last_sum == rx_sum { _FALSE } else { _TRUE };
    update_last_rx_host(sta);
    ret
}

#[no_mangle]
pub extern "C" fn chk_sta_is_alive(psta: *mut StaInfo) -> U8 {
    if psta.is_null() {
        return _FALSE;
    }
    #[cfg(host_ap_sta_alive_test)]
    {
        chk_sta_is_alive_host(psta as *mut StaInfoHost)
    }
    #[cfg(not(host_ap_sta_alive_test))]
    {
        chk_sta_is_alive_kernel(psta)
    }
}
