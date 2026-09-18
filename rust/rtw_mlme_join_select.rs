// SPDX-License-Identifier: GPL-2.0
//! W3-87 join candidate select — host L2 oracle.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    missing_docs
)]

use std::os::raw::{c_int, c_long, c_void};

type U8 = u8;
type U32 = u32;
type S32 = i32;
const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const _SUCCESS: c_int = 1;
const _FAIL: c_int = 0;
const ETH_ALEN: usize = 6;
const MAX_IE_SZ: usize = 768;

const WIFI_UNDER_LINKING: S32 = 0x80;
const SS_DENY_BLOCK_SCAN: U8 = 2;
const SS_DENY_BY_DRV: U8 = 3;
const SS_DENY_SELF_AP_UNDER_WPS: U8 = 4;
const SS_DENY_SELF_AP_UNDER_LINKING: U8 = 5;
const SS_DENY_SELF_AP_UNDER_SURVEY: U8 = 6;
const SS_DENY_SELF_STA_UNDER_LINKING: U8 = 8;
const SS_DENY_SELF_STA_UNDER_SURVEY: U8 = 9;
const SS_ALLOW: U8 = 12;
const WIFI_AP_STATE: S32 = 0x10;
const WIFI_UNDER_WPS: S32 = 0x100;
const WIFI_UNDER_SURVEY: S32 = 0x800;

#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}

#[repr(C)]
struct Queue {
    queue: List,
    lock: c_int,
}

#[repr(C)]
struct Ndis80211Ssid {
    ssid_length: U32,
    ssid: [U8; 32],
}

#[repr(C)]
struct Ndis80211Configuration {
    length: U32,
    beacon_period: U32,
    atim_window: U32,
    ds_config: U32,
}

#[repr(C)]
struct WlanPhyInfo {
    signal_strength: U8,
    signal_quality: U8,
    optimum_antenna: U8,
}

#[repr(C)]
struct WlanBssidEx {
    length: U32,
    mac_address: [U8; ETH_ALEN],
    reserved: [U8; 2],
    ssid: Ndis80211Ssid,
    privacy: U32,
    rssi: c_long,
    configuration: Ndis80211Configuration,
    phy_info: WlanPhyInfo,
    ie_length: U32,
    ies: [U8; MAX_IE_SZ],
}

#[repr(C)]
struct WlanNetwork {
    list: List,
    network: WlanBssidEx,
    last_scanned: u64,
}

#[repr(C)]
struct MlmePriv {
    fw_state: S32,
    scanned_queue: Queue,
    cur_network: WlanNetwork,
    assoc_ssid: Ndis80211Ssid,
    assoc_bssid: [U8; ETH_ALEN],
    assoc_by_bssid: U32,
    nic_hdl: *mut c_void,
    pscanned: *mut List,
    roam_network: *mut WlanNetwork,
}

#[repr(C)]
struct RtChannelInfo {
    channel_num: U8,
}

#[repr(C)]
struct RfCtl {
    channel_set: [RtChannelInfo; 14],
}

#[repr(C)]
struct RegistryPriv {
    scan_interval_thr: U32,
}

#[repr(C)]
struct DvobjPriv {
    scan_deny: U8,
}

#[repr(C)]
struct Adapter {
    mlmepriv: MlmePriv,
    rfctl: RfCtl,
    registrypriv: RegistryPriv,
    dvobj: DvobjPriv,
}

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn rtw_chset_search_ch(ch_set: *mut RtChannelInfo, ch: U32) -> c_int;
    fn rtw_is_desired_network(a: *mut Adapter, n: *mut WlanNetwork) -> c_int;
    fn rtw_is_scan_deny(a: *mut Adapter) -> c_int;
    fn rtw_joinbss_cmd(a: *mut Adapter, n: *mut WlanNetwork) -> c_int;
}

fn check_fwstate(mlme: &MlmePriv, state: S32) -> bool {
    if state == 0 && mlme.fw_state == 0 {
        return true;
    }
    (mlme.fw_state & state) != 0
}

fn set_fwstate(mlme: &mut MlmePriv, state: S32) {
    mlme.fw_state |= state;
}

#[no_mangle]
pub extern "C" fn _rtw_sitesurvey_condition_check(
    caller: *const u8,
    adapter: *mut Adapter,
    check_sc_interval: bool,
) -> U8 {
    let _ = (caller, check_sc_interval);
    if adapter.is_null() {
        return SS_ALLOW;
    }
    unsafe {
        let mlme = &(*adapter).mlmepriv;
        if (*adapter).dvobj.scan_deny != 0 {
            return SS_DENY_BLOCK_SCAN;
        }
        if rtw_is_scan_deny(adapter) != 0 {
            return SS_DENY_BY_DRV;
        }
        if check_fwstate(mlme, WIFI_AP_STATE) {
            if check_fwstate(mlme, WIFI_UNDER_WPS) {
                return SS_DENY_SELF_AP_UNDER_WPS;
            }
            if check_fwstate(mlme, WIFI_UNDER_LINKING) {
                return SS_DENY_SELF_AP_UNDER_LINKING;
            }
            if check_fwstate(mlme, WIFI_UNDER_SURVEY) {
                return SS_DENY_SELF_AP_UNDER_SURVEY;
            }
        } else {
            if check_fwstate(mlme, WIFI_UNDER_LINKING) {
                return SS_DENY_SELF_STA_UNDER_LINKING;
            }
            if check_fwstate(mlme, WIFI_UNDER_SURVEY) {
                return SS_DENY_SELF_STA_UNDER_SURVEY;
            }
        }
        SS_ALLOW
    }
}

#[no_mangle]
pub extern "C" fn rtw_check_join_candidate(
    mlme: *mut MlmePriv,
    candidate: *mut *mut WlanNetwork,
    competitor: *mut WlanNetwork,
) -> c_int {
    if mlme.is_null() || candidate.is_null() || competitor.is_null() {
        return _FALSE;
    }
    unsafe {
        let adapter = mlme as *mut Adapter;
        let ch = (*competitor).network.configuration.ds_config;
        let chset = (*adapter).rfctl.channel_set.as_mut_ptr();
        if rtw_chset_search_ch(chset, ch) < 0 {
            return _FALSE;
        }
        if (*mlme).assoc_by_bssid != 0 {
            if _rtw_memcmp(
                (*competitor).network.mac_address.as_ptr() as *const c_void,
                (*mlme).assoc_bssid.as_ptr() as *const c_void,
                ETH_ALEN,
            ) == _FALSE
            {
                return _FALSE;
            }
        }
        if (*mlme).assoc_ssid.ssid[0] != 0 && (*mlme).assoc_ssid.ssid_length != 0 {
            if (*competitor).network.ssid.ssid_length != (*mlme).assoc_ssid.ssid_length
                || _rtw_memcmp(
                    (*competitor).network.ssid.ssid.as_ptr() as *const c_void,
                    (*mlme).assoc_ssid.ssid.as_ptr() as *const c_void,
                    (*mlme).assoc_ssid.ssid_length as usize,
                ) == _FALSE
            {
                return _FALSE;
            }
        }
        if rtw_is_desired_network(adapter, competitor) == _FALSE {
            return _FALSE;
        }
        if (*candidate).is_null() || (*(*candidate)).network.rssi < (*competitor).network.rssi {
            *candidate = competitor;
            return _TRUE;
        }
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_select_and_join_from_scanned_queue(mlme: *mut MlmePriv) -> c_int {
    if mlme.is_null() {
        return _FAIL;
    }
    unsafe {
        let adapter = (*mlme).nic_hdl as *mut Adapter;
        let head = &mut (*mlme).scanned_queue.queue as *mut List;
        (*mlme).pscanned = (*head).next;
        let mut candidate: *mut WlanNetwork = std::ptr::null_mut();

        while (*mlme).pscanned != head {
            let pnetwork = (*mlme).pscanned as *mut WlanNetwork;
            if pnetwork.is_null() {
                return _FAIL;
            }
            (*mlme).pscanned = (*(*mlme).pscanned).next;
            rtw_check_join_candidate(mlme, &mut candidate, pnetwork);
        }
        if candidate.is_null() {
            return _FAIL;
        }
        set_fwstate(&mut (*mlme), WIFI_UNDER_LINKING);
        rtw_joinbss_cmd(adapter, candidate)
    }
}
