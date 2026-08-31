// SPDX-License-Identifier: GPL-2.0
//! W3-64 roaming candidate check/select — host L2 oracle (kernel port in PR5b).

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

#[cfg(host_mlme_roaming_test)]
use std::os::raw::{c_int, c_long, c_ulong, c_void};

type U8 = u8;
type U32 = u32;
type S32 = i32;
type Systime = c_ulong;
type IrqL = c_ulong;

const ETH_ALEN: usize = 6;
const MAX_IE_SZ: usize = 768;
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
struct WlanBssidEx {
    length: U32,
    mac_address: [U8; ETH_ALEN],
    reserved: [U8; 2],
    ssid: Ndis80211Ssid,
    mesh_id: Ndis80211Ssid,
    privacy: U32,
    rssi: c_long,
    configuration: Ndis80211Configuration,
    infrastructure_mode: U32,
    supported_rates: [U8; 16],
    phy_info: [U8; 8],
    ie_length: U32,
    ies: [U8; MAX_IE_SZ],
}

#[repr(C)]
struct RtChannelInfo {
    channel_num: U8,
}

#[repr(C)]
struct RfCtl {
    channel_set: [RtChannelInfo; 14],
    ch_num: U8,
}

#[repr(C)]
struct WlanNetwork {
    list: List,
    network: WlanBssidEx,
    last_scanned: Systime,
}

#[repr(C)]
struct MlmePriv {
    scanned_queue: Queue,
    cur_network_scanned: *mut WlanNetwork,
    cur_network: WlanNetwork,
    need_to_roam: U8,
    roam_tgt_addr: [U8; ETH_ALEN],
    roam_scanr_exp_ms: U32,
    roam_rssi_diff_th: S32,
    nic_hdl: *mut c_void,
    roam_network: *mut WlanNetwork,
    pscanned: *mut List,
}

#[repr(C)]
struct Adapter {
    mlmepriv: MlmePriv,
    rfctl: RfCtl,
}

const _: () = assert!(core::mem::size_of::<MlmePriv>() == 1008);

extern "C" {
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int;
    fn rtw_get_passing_time_ms(start: Systime) -> U32;
    fn rtw_chset_search_ch(ch_set: *mut RtChannelInfo, ch: U32) -> c_int;
    fn rtw_is_desired_network(adapter: *mut Adapter, pnetwork: *mut WlanNetwork) -> c_int;
}

fn is_same_ess(a: *mut WlanBssidEx, b: *mut WlanBssidEx) -> c_int {
    if a.is_null() || b.is_null() {
        return _FALSE;
    }
    unsafe {
        let al = (*a).ssid.ssid_length;
        let bl = (*b).ssid.ssid_length;
        if al != bl {
            return _FALSE;
        }
        if _rtw_memcmp(
            (*a).ssid.ssid.as_ptr() as *const c_void,
            (*b).ssid.ssid.as_ptr() as *const c_void,
            al as usize,
        ) == _TRUE
        {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[inline]
fn is_zero_mac(addr: &[U8; ETH_ALEN]) -> bool {
    addr.iter().all(|&b| b == 0)
}

#[no_mangle]
pub extern "C" fn rtw_check_roaming_candidate(
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
        if is_same_ess(&mut (*competitor).network, &mut (*mlme).cur_network.network) == _FALSE {
            return _FALSE;
        }
        if rtw_is_desired_network(adapter, competitor) == _FALSE {
            return _FALSE;
        }
        if (*mlme).need_to_roam == 0 {
            return _FALSE;
        }
        if !is_zero_mac(&(*mlme).roam_tgt_addr) {
            if _rtw_memcmp(
                (*mlme).roam_tgt_addr.as_ptr() as *const c_void,
                (*competitor).network.mac_address.as_ptr() as *const c_void,
                ETH_ALEN,
            ) == _TRUE
            {
                *candidate = competitor;
                return _TRUE;
            }
            return _FALSE;
        }
        if rtw_get_passing_time_ms((*competitor).last_scanned) >= (*mlme).roam_scanr_exp_ms {
            return _FALSE;
        }
        let cur = (*mlme).cur_network_scanned;
        if cur.is_null() {
            return _FALSE;
        }
        if (*competitor).network.rssi - (*cur).network.rssi < (*mlme).roam_rssi_diff_th as c_long {
            return _FALSE;
        }
        if !(*candidate).is_null() && (*(*candidate)).network.rssi >= (*competitor).network.rssi {
            return _FALSE;
        }
        *candidate = competitor;
        _TRUE
    }
}

#[no_mangle]
pub extern "C" fn rtw_select_roaming_candidate(mlme: *mut MlmePriv) -> c_int {
    if mlme.is_null() {
        return _FAIL;
    }
    unsafe {
        if (*mlme).cur_network_scanned.is_null() {
            return _FAIL;
        }
        let head = &mut (*mlme).scanned_queue.queue as *mut List;
        (*mlme).pscanned = (*head).next;
        let mut candidate: *mut WlanNetwork = core::ptr::null_mut();

        while (*mlme).pscanned != head {
            let pnetwork = (*mlme).pscanned as *mut WlanNetwork;
            if pnetwork.is_null() {
                return _FAIL;
            }
            (*mlme).pscanned = (*(*mlme).pscanned).next;
            rtw_check_roaming_candidate(mlme, &mut candidate, pnetwork);
        }
        if candidate.is_null() {
            return _FAIL;
        }
        (*mlme).roam_network = candidate;
        if !is_zero_mac(&(*mlme).roam_tgt_addr)
            && _rtw_memcmp(
                (*mlme).roam_tgt_addr.as_ptr() as *const c_void,
                (*candidate).network.mac_address.as_ptr() as *const c_void,
                ETH_ALEN,
            ) == _TRUE
        {
            (*mlme).roam_tgt_addr = [0; ETH_ALEN];
        }
        _SUCCESS
    }
}
