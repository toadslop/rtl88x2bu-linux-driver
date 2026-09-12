// SPDX-License-Identifier: GPL-2.0
//! W3-82 `issue_aka_chk_frame` — Rust port of `core/rtw_ap_aka_chk.c`.

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

const _SUCCESS: i32 = 0;
const _FAIL: i32 = -1;

#[cfg(host_ap_aka_chk_test)]
const WIFI_SLEEP_STATE: u32 = 0x00000004;
#[cfg(host_ap_aka_chk_test)]
const WIFI_AP_STATE: u32 = 0x00000010;

#[cfg(host_ap_aka_chk_test)]
#[repr(C)]
struct MlmePrivHost {
    fwstate: u32,
}

#[cfg(host_ap_aka_chk_test)]
#[repr(C)]
struct CmnStaHost {
    mac_addr: [U8; 6],
}

#[cfg(host_ap_aka_chk_test)]
#[repr(C)]
struct StaInfoHost {
    cmn: CmnStaHost,
    state: u32,
}

#[cfg(host_ap_aka_chk_test)]
#[repr(C)]
struct AdapterHost {
    mlmepriv: MlmePrivHost,
}

#[cfg(host_ap_aka_chk_test)]
extern "C" {
    fn issue_nulldata(
        adapter: *mut AdapterHost,
        target_addr: *mut U8,
        pwr: i32,
        ps: i32,
        retry: i32,
    ) -> i32;
}

#[cfg(not(host_ap_aka_chk_test))]
extern "C" {
    fn issue_nulldata(
        adapter: *mut Adapter,
        target_addr: *mut U8,
        pwr: i32,
        ps: i32,
        retry: i32,
    ) -> i32;
    fn rtw_rust_aka_mlme_is_ap(adapter: *mut Adapter) -> U8;
    fn rtw_rust_aka_sta_sleep(psta: *mut StaInfo) -> U8;
    fn rtw_rust_aka_sta_mac(psta: *mut StaInfo) -> *mut U8;
    fn rtw_rust_aka_chk_mesh(adapter: *mut Adapter, target_addr: *mut U8, ap_ret: i32) -> i32;
}

#[cfg(host_ap_aka_chk_test)]
fn mlme_is_ap(adapter: &AdapterHost) -> bool {
    (adapter.mlmepriv.fwstate & WIFI_AP_STATE) != 0
}

#[cfg(host_ap_aka_chk_test)]
fn issue_aka_host(adapter: *mut AdapterHost, psta: *mut StaInfoHost) -> i32 {
    if adapter.is_null() || psta.is_null() {
        return _FAIL;
    }
    let adapter = unsafe { &*adapter };
    let psta = unsafe { &*psta };
    let mut ret = _FAIL;
    if mlme_is_ap(adapter) {
        let ps = if (psta.state & WIFI_SLEEP_STATE) != 0 {
            1
        } else {
            3
        };
        ret = unsafe {
            issue_nulldata(
                adapter as *const _ as *mut AdapterHost,
                psta.cmn.mac_addr.as_ptr() as *mut U8,
                0,
                ps,
                50,
            )
        };
    }
    ret
}

#[cfg(not(host_ap_aka_chk_test))]
fn issue_aka_kernel(adapter: *mut Adapter, psta: *mut StaInfo) -> i32 {
    if adapter.is_null() || psta.is_null() {
        return _FAIL;
    }
    unsafe {
        let mut ret = _FAIL;
        let mac = rtw_rust_aka_sta_mac(psta);
        if rtw_rust_aka_mlme_is_ap(adapter) != 0 {
            let ps = if rtw_rust_aka_sta_sleep(psta) != 0 {
                1
            } else {
                3
            };
            ret = issue_nulldata(adapter, mac, 0, ps, 50);
        }
        rtw_rust_aka_chk_mesh(adapter, mac, ret)
    }
}

#[no_mangle]
pub extern "C" fn issue_aka_chk_frame(adapter: *mut Adapter, psta: *mut StaInfo) -> i32 {
    #[cfg(host_ap_aka_chk_test)]
    {
        issue_aka_host(adapter as *mut AdapterHost, psta as *mut StaInfoHost)
    }
    #[cfg(not(host_ap_aka_chk_test))]
    {
        issue_aka_kernel(adapter, psta)
    }
}
