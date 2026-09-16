// SPDX-License-Identifier: GPL-2.0
//! W3-80 BMC update tx-rate helpers — Rust port of `core/rtw_ap_bmc_update.c` (host L2).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub
)]
#![cfg(host_ap_bmc_update_test)]

use core::ffi::c_void;

const MGN_UNKNOWN: u8 = 0x00;
const MGN_1M: u8 = 0x02;
const MGN_2M: u8 = 0x04;
const MGN_5_5M: u8 = 0x0b;
const MGN_6M: u8 = 0x0c;
const MGN_9M: u8 = 0x12;
const MGN_11M: u8 = 0x16;
const MGN_12M: u8 = 0x18;
const MGN_18M: u8 = 0x24;
const MGN_24M: u8 = 0x30;
const MGN_36M: u8 = 0x48;
const MGN_48M: u8 = 0x60;
const MGN_54M: u8 = 0x6c;
const WIFI_AP_STATE: u32 = 0x0000_0010;
const WIFI_MESH_STATE: u32 = 0x0000_0200;
const WIRELESS_11G: u32 = 2;
const WIRELESS_11B: u32 = 1;
const WIRELESS_11A: u32 = 4;
const WIRELESS_INVALID: i32 = 0;
const WIFI_ASOC_STATE: u32 = 0x0000_0001;
const HOST_BMC_MAX_RATES: usize = 12;
const _FALSE: u8 = 0;

#[repr(C)]
pub struct List {
    pub next: *mut List,
    pub prev: *mut List,
}

#[repr(C)]
pub struct RaStaInfo {
    pub ramask: u64,
    pub curr_tx_rate: u8,
}

#[repr(C)]
pub struct CmnStaInfo {
    pub ra_info: RaStaInfo,
}

#[repr(C)]
pub struct StaInfoStats {
    pub pad: [u8; 8],
}

#[repr(C)]
pub struct HtPriv {
    pub ht_option: u8,
}

#[repr(C)]
pub struct StaInfo {
    pub cmn: CmnStaInfo,
    pub aid: u16,
    pub qos_option: u8,
    pub htpriv: HtPriv,
    pub ieee8021x_blocked: u8,
    pub sta_stats: StaInfoStats,
    pub lock: i32,
    pub state: u8,
    pub wireless_mode: u8,
    pub bssrateset: [u8; HOST_BMC_MAX_RATES],
    pub bssratelen: u8,
    pub init_rate: u8,
    pub asoc_list: List,
}

#[repr(C)]
pub struct WlanConfig {
    pub ds_config: u8,
}

#[repr(C)]
pub struct WlanBssidEx {
    pub supported_rates: [u8; HOST_BMC_MAX_RATES],
    pub configuration: WlanConfig,
}

#[repr(C)]
pub struct CurNetwork {
    pub network: WlanBssidEx,
}

#[repr(C)]
pub struct StaPriv {
    pub asoc_list: List,
    pub asoc_list_lock: i32,
    pub asoc_sta_count: i32,
    pub host_bcmc_sta: *mut StaInfo,
}

#[repr(C)]
pub struct MlmePriv {
    pub state: u32,
    pub cur_network: CurNetwork,
}

#[repr(C)]
pub struct MlmeExtPriv {
    pub cur_wireless_mode: u32,
}

#[repr(C)]
pub struct HalData {
    pub current_band_type: u8,
}

#[repr(C)]
pub struct Adapter {
    pub hal_data: HalData,
    pub stapriv: StaPriv,
    pub mlmepriv: MlmePriv,
    pub mlmeextpriv: MlmeExtPriv,
    pub bmc_tx_rate: u8,
}

extern "C" {
    fn rtw_ap_find_bmc_rate(adapter: *mut c_void, tx_rate: u8) -> u8;
    fn rtw_get_rateset_len(rateset: *mut u8) -> u32;
    fn rtw_check_network_type(rate: *mut u8, ratelen: i32, channel: i32) -> i32;
    fn update_sta_basic_rate(psta: *mut StaInfo, wireless_mode: u8);
    fn rtw_hal_update_sta_ra_info(padapter: *mut c_void, psta: *mut StaInfo);
    fn rtw_sta_media_status_rpt(padapter: *mut c_void, psta: *mut StaInfo, connected: u8);
}

fn mlme_is_ap(padapter: *mut Adapter) -> bool {
    unsafe { ((*padapter).mlmepriv.state & WIFI_AP_STATE) != 0 }
}

fn mlme_is_mesh(padapter: *mut Adapter) -> bool {
    unsafe { ((*padapter).mlmepriv.state & WIFI_MESH_STATE) != 0 }
}

fn get_lowest_rate_idx_ex(mask: u64, start_bit: u32) -> u8 {
    for i in start_bit..64 {
        if (mask >> i) & 1 != 0 {
            return i as u8;
        }
    }
    0
}

fn get_lowest_rate_idx(mask: u64) -> u8 {
    get_lowest_rate_idx_ex(mask, 0)
}

#[cfg(not(bmc_tx_low_rate))]
fn get_highest_rate_idx(mask: u64) -> u8 {
    for i in (0..64).rev() {
        if (mask >> i) & 1 != 0 {
            return i as u8;
        }
    }
    0
}

const HW_TO_MGN: [u8; 12] = [
    MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M, MGN_48M,
    MGN_54M,
];

fn hw_rate_to_m_rate(hw_rate: u8) -> u8 {
    if (hw_rate as usize) < 12 {
        HW_TO_MGN[hw_rate as usize]
    } else {
        MGN_1M
    }
}

fn is_enable_hw_ofdm(net_type: u32) -> bool {
    const WIRELESS_11G: u32 = 2;
    const WIRELESS_11_24N: u32 = 1 << 3;
    const WIRELESS_MODE_5G: u32 = (1 << 2) | (1 << 4) | (1 << 6);
    (net_type & (WIRELESS_11G | WIRELESS_11_24N | WIRELESS_MODE_5G)) != 0
}

/// Host-L2 walk of `asoc_list` (no lock — single-threaded oracle). Kernel uses
/// `rtw_ap_find_mini_tx_rate` via `rust/rtw_ap_bmc_update_kern.rs`.
unsafe fn ap_find_mini_tx_rate_update_host(adapter: *mut Adapter) -> u8 {
    const ODM_RATEVHTSS4MCS9: u8 = 0x53;
    let stapriv = &mut (*adapter).stapriv;
    let phead = core::ptr::addr_of_mut!(stapriv.asoc_list);
    let mut plist = (*phead).next;
    let mut mini = ODM_RATEVHTSS4MCS9;
    while plist != phead {
        let off = core::mem::offset_of!(StaInfo, asoc_list);
        let psta = (plist as *mut u8).sub(off) as *mut StaInfo;
        let sta_tx_rate = (*psta).cmn.ra_info.curr_tx_rate & 0x7f;
        if sta_tx_rate < mini {
            mini = sta_tx_rate;
        }
        plist = (*plist).next;
    }
    mini
}

#[no_mangle]
pub extern "C" fn rtw_update_bmc_sta_tx_rate(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let adapter = adapter as *mut Adapter;
        let psta = (*adapter).stapriv.host_bcmc_sta;
        if psta.is_null() {
            return;
        }
        if (*adapter).bmc_tx_rate != MGN_UNKNOWN {
            (*psta).init_rate = (*adapter).bmc_tx_rate;
            return;
        }
        if (*adapter).stapriv.asoc_sta_count <= 2 {
            return;
        }
        let mut tx_rate = ap_find_mini_tx_rate_update_host(adapter);
        #[cfg(bmc_tx_low_rate)]
        {
            tx_rate = rtw_ap_find_bmc_rate(adapter.cast(), tx_rate);
        }
        (*psta).init_rate = hw_rate_to_m_rate(tx_rate);
    }
}

#[no_mangle]
pub extern "C" fn rtw_init_bmc_sta_tx_rate(padapter: *mut c_void, psta: *mut c_void) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    let padapter = padapter as *mut Adapter;
    let psta = psta as *mut StaInfo;
    if !mlme_is_ap(padapter) && !mlme_is_mesh(padapter) {
        return;
    }
    const BRATE: [u8; 12] = [
        MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M,
        MGN_48M, MGN_54M,
    ];
    unsafe {
        if (*padapter).bmc_tx_rate != MGN_UNKNOWN {
            (*psta).init_rate = (*padapter).bmc_tx_rate;
            return;
        }
        let ramask = (*psta).cmn.ra_info.ramask;
        let wm = (*padapter).mlmeextpriv.cur_wireless_mode;
        let rate_idx = {
            #[cfg(bmc_tx_low_rate)]
            {
                if is_enable_hw_ofdm(wm) && ramask != 0 {
                    get_lowest_rate_idx_ex(ramask, 4)
                } else {
                    get_lowest_rate_idx(ramask)
                }
            }
            #[cfg(not(bmc_tx_low_rate))]
            {
                let _ = wm;
                get_highest_rate_idx(ramask)
            }
        };
        (*psta).init_rate = if (rate_idx as usize) < 12 {
            BRATE[rate_idx as usize]
        } else {
            MGN_1M
        };
    }
}

fn is_supported_tx_cck(net_type: u8) -> bool {
    (net_type as u32 & WIRELESS_11B) != 0
}

#[no_mangle]
pub extern "C" fn update_bmc_sta(padapter: *mut c_void) {
    if padapter.is_null() {
        return;
    }
    let padapter = padapter as *mut Adapter;
    unsafe {
        let psta = (*padapter).stapriv.host_bcmc_sta;
        if psta.is_null() {
            return;
        }
        (*psta).aid = 0;
        (*psta).qos_option = 0;
        (*psta).htpriv.ht_option = _FALSE;
        (*psta).ieee8021x_blocked = 0;
        (*psta).sta_stats.pad = [0; 8];

        let pcur = &mut (*padapter).mlmepriv.cur_network.network;
        let support_rate_num = rtw_get_rateset_len(pcur.supported_rates.as_mut_ptr()) as i32;
        let ds_config = pcur.configuration.ds_config;
        let mut network_type = rtw_check_network_type(
            pcur.supported_rates.as_mut_ptr(),
            support_rate_num,
            ds_config as i32,
        ) as u8;
        if is_supported_tx_cck(network_type) {
            network_type = WIRELESS_11B as u8;
        } else if network_type as i32 == WIRELESS_INVALID {
            network_type = if ds_config > 14 {
                WIRELESS_11A
            } else {
                WIRELESS_11B
            } as u8;
        }
        update_sta_basic_rate(psta, network_type);
        (*psta).wireless_mode = network_type;
        rtw_hal_update_sta_ra_info(padapter.cast(), psta);
        (*psta).state = WIFI_ASOC_STATE as u8;
        rtw_sta_media_status_rpt(padapter.cast(), psta, 1);
        rtw_init_bmc_sta_tx_rate(padapter.cast(), psta.cast());
    }
}
