// SPDX-License-Identifier: GPL-2.0
//! W3-82 `rtw_ap_expire_asoc_sta_tick` — Rust port of `core/rtw_ap_expire_asoc.c`.

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
type Adapter = c_void;
type StaInfo = c_void;

extern "C" {
    fn chk_sta_is_alive(psta: *mut StaInfo) -> U8;
}

#[cfg(host_ap_expire_asoc_test)]
#[repr(C)]
pub struct StainfoStats {
    pub rx_ctrl_pkts: u64,
    pub last_rx_ctrl_pkts: u64,
    pub rx_data_pkts: u64,
    pub last_rx_data_pkts: u64,
}

#[cfg(host_ap_expire_asoc_test)]
#[repr(C)]
pub struct StaInfoHost {
    pub sta_stats: StainfoStats,
    pub expire_to: U8,
    pub keep_alive_trycnt: U8,
}

#[cfg(host_ap_expire_asoc_test)]
#[repr(C)]
pub struct StaPrivHost {
    pub expire_to: U8,
}

#[cfg(host_ap_expire_asoc_test)]
#[repr(C)]
pub struct AdapterHost {
    pub stapriv: StaPrivHost,
}

#[cfg(not(host_ap_expire_asoc_test))]
extern "C" {
    fn rtw_rust_expire_asoc_stapriv_expire_to(padapter: *mut Adapter) -> U8;
    fn rtw_rust_expire_asoc_sta_expire_to(psta: *mut StaInfo) -> U8;
    fn rtw_rust_expire_asoc_sta_set_expire_to(psta: *mut StaInfo, v: U8);
    fn rtw_rust_expire_asoc_sta_set_keep_alive_trycnt(psta: *mut StaInfo, v: U8);
    #[cfg(expire_asoc_clear_under_exist_checking)]
    fn rtw_rust_expire_asoc_sta_clear_under_exist_checking(psta: *mut StaInfo);
}

#[cfg(host_ap_expire_asoc_test)]
fn expire_asoc_tick_host(padapter: *mut AdapterHost, psta: *mut StaInfoHost) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    let adapter = unsafe { &*padapter };
    let sta = unsafe { &mut *psta };
    let alive = unsafe { chk_sta_is_alive(psta as *mut StaInfo) };
    if alive != 0 || sta.expire_to == 0 {
        sta.expire_to = adapter.stapriv.expire_to;
        sta.keep_alive_trycnt = 0;
    } else {
        sta.expire_to -= 1;
    }
}

#[cfg(not(host_ap_expire_asoc_test))]
fn expire_asoc_tick_kernel(padapter: *mut Adapter, psta: *mut StaInfo) {
    unsafe {
        let alive = chk_sta_is_alive(psta);
        let expire_to = rtw_rust_expire_asoc_sta_expire_to(psta);
        if alive != 0 || expire_to == 0 {
            rtw_rust_expire_asoc_sta_set_expire_to(
                psta,
                rtw_rust_expire_asoc_stapriv_expire_to(padapter),
            );
            rtw_rust_expire_asoc_sta_set_keep_alive_trycnt(psta, 0);
            #[cfg(expire_asoc_clear_under_exist_checking)]
            rtw_rust_expire_asoc_sta_clear_under_exist_checking(psta);
        } else {
            rtw_rust_expire_asoc_sta_set_expire_to(psta, expire_to - 1);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_ap_expire_asoc_sta_tick(padapter: *mut Adapter, psta: *mut StaInfo) {
    #[cfg(host_ap_expire_asoc_test)]
    expire_asoc_tick_host(padapter as *mut AdapterHost, psta as *mut StaInfoHost);
    #[cfg(not(host_ap_expire_asoc_test))]
    expire_asoc_tick_kernel(padapter, psta);
}
