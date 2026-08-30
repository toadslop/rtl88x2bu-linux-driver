// SPDX-License-Identifier: GPL-2.0
//! W3-64 roaming kernel port — included from rtw_mlme_rest.rs when CONFIG_RUST_MLME_ROAMING.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    unused_imports,
    unreachable_pub
)]

use core::ffi::{c_int, c_long, c_ulong, c_void};

type U8 = u8;
type U32 = u32;
type S32 = i32;
type Systime = c_ulong;
type IrqL = c_ulong;

const ETH_ALEN: usize = 6;
const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const _SUCCESS: c_int = 1;
const _FAIL: c_int = 0;

#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}

#[repr(C)]
struct WlanBssidEx {
    _pad: [U8; 96],
    rssi: c_long,
    _pad2: [U8; 832],
}

#[repr(C)]
struct RtChannelInfo {
    channel_num: U8,
}

#[repr(C)]
struct WlanNetwork {
    list: List,
    network: WlanBssidEx,
    last_scanned: Systime,
}

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn _rtw_get_passing_time_ms(start: Systime) -> S32;
    fn is_same_ess(a: *mut c_void, b: *mut c_void) -> c_int;
}

mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_mlme_roaming_adapter(mlme: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_mlme_roaming_chset(adapter: *mut c_void) -> *mut RtChannelInfo;
        pub fn rtw_rust_mlme_roaming_cur_scanned(mlme: *mut c_void) -> *mut WlanNetwork;
        pub fn rtw_rust_mlme_roaming_cur_network(mlme: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_mlme_roaming_need_to_roam(mlme: *mut c_void) -> c_int;
        pub fn rtw_rust_mlme_roaming_roam_tgt_addr(mlme: *mut c_void) -> *mut U8;
        pub fn rtw_rust_mlme_roaming_scanr_exp_ms(mlme: *mut c_void) -> U32;
        pub fn rtw_rust_mlme_roaming_rssi_diff_th(mlme: *mut c_void) -> S32;
        pub fn rtw_rust_mlme_roaming_roam_network_ptr(mlme: *mut c_void) -> *mut *mut WlanNetwork;
        pub fn rtw_rust_mlme_roaming_pscanned_ptr(mlme: *mut c_void) -> *mut *mut List;
        pub fn rtw_rust_mlme_roaming_scanned_head(mlme: *mut c_void) -> *mut List;
        pub fn rtw_rust_mlme_roaming_enter_scanned(mlme: *mut c_void, irq: *mut IrqL);
        pub fn rtw_rust_mlme_roaming_exit_scanned(mlme: *mut c_void, irq: *mut IrqL);
        pub fn rtw_rust_mlme_roaming_net_dsconfig(net: *mut WlanNetwork) -> U32;
        pub fn rtw_rust_mlme_roaming_net_rssi(net: *mut WlanNetwork) -> c_long;
        pub fn rtw_rust_mlme_roaming_net_mac(net: *mut WlanNetwork) -> *mut U8;
        pub fn rtw_rust_mlme_roaming_net_bss(net: *mut WlanNetwork) -> *mut c_void;
        pub fn rtw_rust_mlme_roaming_net_last_scanned(net: *mut WlanNetwork) -> Systime;
        pub fn rtw_chset_search_ch(ch_set: *mut RtChannelInfo, ch: U32) -> c_int;
        pub fn rtw_is_desired_network(adapter: *mut c_void, pnetwork: *mut WlanNetwork) -> c_int;
    }
}

#[inline]
fn is_zero_mac_ptr(p: *mut U8) -> bool {
    unsafe {
        let s = core::slice::from_raw_parts(p, ETH_ALEN);
        s.iter().all(|&b| b == 0)
    }
}

#[no_mangle]
pub extern "C" fn rtw_check_roaming_candidate(
    mlme: *mut c_void,
    candidate: *mut *mut WlanNetwork,
    competitor: *mut WlanNetwork,
) -> c_int {
    if mlme.is_null() || candidate.is_null() || competitor.is_null() {
        return _FALSE;
    }
    unsafe {
        let adapter = kernel::rtw_rust_mlme_roaming_adapter(mlme);
        let chset = kernel::rtw_rust_mlme_roaming_chset(adapter);
        let ch = kernel::rtw_rust_mlme_roaming_net_dsconfig(competitor);
        if kernel::rtw_chset_search_ch(chset, ch) < 0 {
            return _FALSE;
        }
        let comp_bss = kernel::rtw_rust_mlme_roaming_net_bss(competitor);
        let cur_bss = kernel::rtw_rust_mlme_roaming_cur_network(mlme);
        if is_same_ess(comp_bss, cur_bss) == _FALSE {
            return _FALSE;
        }
        if kernel::rtw_is_desired_network(adapter, competitor) == _FALSE {
            return _FALSE;
        }
        if kernel::rtw_rust_mlme_roaming_need_to_roam(mlme) == 0 {
            return _FALSE;
        }
        let roam_tgt = kernel::rtw_rust_mlme_roaming_roam_tgt_addr(mlme);
        if !is_zero_mac_ptr(roam_tgt) {
            let comp_mac = kernel::rtw_rust_mlme_roaming_net_mac(competitor);
            if _rtw_memcmp(
                roam_tgt as *const c_void,
                comp_mac as *const c_void,
                ETH_ALEN,
            ) == _TRUE
            {
                *candidate = competitor;
                return _TRUE;
            }
            return _FALSE;
        }
        if (_rtw_get_passing_time_ms(kernel::rtw_rust_mlme_roaming_net_last_scanned(competitor))
            as U32)
            >= kernel::rtw_rust_mlme_roaming_scanr_exp_ms(mlme)
        {
            return _FALSE;
        }
        let cur = kernel::rtw_rust_mlme_roaming_cur_scanned(mlme);
        if cur.is_null() {
            return _FALSE;
        }
        let diff = kernel::rtw_rust_mlme_roaming_net_rssi(competitor)
            - kernel::rtw_rust_mlme_roaming_net_rssi(cur);
        if diff < kernel::rtw_rust_mlme_roaming_rssi_diff_th(mlme) as c_long {
            return _FALSE;
        }
        if !(*candidate).is_null()
            && kernel::rtw_rust_mlme_roaming_net_rssi(*candidate)
                >= kernel::rtw_rust_mlme_roaming_net_rssi(competitor)
        {
            return _FALSE;
        }
        *candidate = competitor;
        _TRUE
    }
}

#[no_mangle]
pub extern "C" fn rtw_select_roaming_candidate(mlme: *mut c_void) -> c_int {
    if mlme.is_null() {
        return _FAIL;
    }
    unsafe {
        if kernel::rtw_rust_mlme_roaming_cur_scanned(mlme).is_null() {
            return _FAIL;
        }
        let mut irq: IrqL = 0;
        kernel::rtw_rust_mlme_roaming_enter_scanned(mlme, &mut irq);
        let head = kernel::rtw_rust_mlme_roaming_scanned_head(mlme);
        *kernel::rtw_rust_mlme_roaming_pscanned_ptr(mlme) = (*head).next;
        let mut candidate: *mut WlanNetwork = core::ptr::null_mut();
        while *kernel::rtw_rust_mlme_roaming_pscanned_ptr(mlme) != head {
            let pscanned = *kernel::rtw_rust_mlme_roaming_pscanned_ptr(mlme);
            let pnetwork = pscanned as *mut WlanNetwork;
            if pnetwork.is_null() {
                kernel::rtw_rust_mlme_roaming_exit_scanned(mlme, &mut irq);
                return _FAIL;
            }
            *kernel::rtw_rust_mlme_roaming_pscanned_ptr(mlme) = (*pscanned).next;
            rtw_check_roaming_candidate(mlme, &mut candidate, pnetwork);
        }
        if candidate.is_null() {
            kernel::rtw_rust_mlme_roaming_exit_scanned(mlme, &mut irq);
            return _FAIL;
        }
        *kernel::rtw_rust_mlme_roaming_roam_network_ptr(mlme) = candidate;
        let roam_tgt = kernel::rtw_rust_mlme_roaming_roam_tgt_addr(mlme);
        let cand_mac = kernel::rtw_rust_mlme_roaming_net_mac(candidate);
        if !is_zero_mac_ptr(roam_tgt)
            && _rtw_memcmp(
                roam_tgt as *const c_void,
                cand_mac as *const c_void,
                ETH_ALEN,
            ) == _TRUE
        {
            core::ptr::write_bytes(roam_tgt, 0, ETH_ALEN);
        }
        kernel::rtw_rust_mlme_roaming_exit_scanned(mlme, &mut irq);
        _SUCCESS
    }
}
