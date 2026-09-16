// SPDX-License-Identifier: GPL-2.0
//! W3-83 `rtw_ap_update_sta_ra_info` — Rust port of `core/rtw_ap_sta_ra.c`.

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
type U32 = u32;
type U64 = u64;
type Adapter = c_void;
type StaInfo = c_void;

const WIFI_ASOC_STATE: U32 = 0x0000_0001;
const WIRELESS_11B: U8 = 1 << 0;
const WIRELESS_11G: U8 = 1 << 1;
const WIRELESS_11A: U8 = 1 << 2;
const WIRELESS_11_24N: U8 = 1 << 3;
const WIRELESS_11_5N: U8 = 1 << 4;
const WIRELESS_11AC: U8 = 1 << 6;
const WIRELESS_11_5AC: U8 = WIRELESS_11A | WIRELESS_11AC;

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct RtwRaInfo {
    pub ramask: U64,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct CmnStaInfo {
    pub ra_info: RtwRaInfo,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct VhtPriv {
    pub vht_option: U8,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct StaInfoHost {
    pub cmn: CmnStaInfo,
    pub state: U32,
    pub wireless_mode: U8,
    pub vhtpriv: VhtPriv,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct WlanConfig {
    pub DSConfig: U32,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct WlanBssidEx {
    pub Configuration: WlanConfig,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct WlanNetwork {
    pub network: WlanBssidEx,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct MlmePriv {
    pub cur_network: WlanNetwork,
}

#[cfg(host_ap_sta_ra_test)]
#[repr(C)]
pub struct AdapterHost {
    pub mlmepriv: MlmePriv,
}

extern "C" {
    fn rtw_hal_update_sta_ra_info(padapter: *mut Adapter, psta: *mut StaInfo);
    fn rtw_hal_update_sta_wset(padapter: *mut Adapter, psta: *mut StaInfo);
}

#[cfg(not(host_ap_sta_ra_test))]
extern "C" {
    fn rtw_rust_ap_sta_ra_ds_config(padapter: *mut Adapter) -> U32;
    fn rtw_rust_ap_sta_ra_state(psta: *mut StaInfo) -> U32;
    fn rtw_rust_ap_sta_ra_ramask(psta: *mut StaInfo) -> U64;
    fn rtw_rust_ap_sta_ra_vht_option(psta: *mut StaInfo) -> U8;
    fn rtw_rust_ap_sta_ra_set_wireless_mode(psta: *mut StaInfo, mode: U8);
}

fn wireless_mode_from_ramask(ds_config: U32, tx_ra_bitmap: U64, vht_option: U8) -> U8 {
    let mut sta_band: U8 = 0;
    if ds_config > 14 {
        if tx_ra_bitmap & 0x0fff_f000 != 0 {
            sta_band |= WIRELESS_11_5N;
        }
        if tx_ra_bitmap & 0x0000_0ff0 != 0 {
            sta_band |= WIRELESS_11A;
        }
        #[cfg(any(host_ap_sta_ra_test, config_80211ac_vht))]
        if vht_option != 0 {
            sta_band = WIRELESS_11_5AC;
        }
    } else {
        if tx_ra_bitmap & 0x0fff_f000 != 0 {
            sta_band |= WIRELESS_11_24N;
        }
        if tx_ra_bitmap & 0x0000_0ff0 != 0 {
            sta_band |= WIRELESS_11G;
        }
        if tx_ra_bitmap & 0x0000_000f != 0 {
            sta_band |= WIRELESS_11B;
        }
    }
    sta_band
}

#[cfg(host_ap_sta_ra_test)]
fn update_sta_ra_info_host(padapter: *mut AdapterHost, psta: *mut StaInfoHost) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    let adapter = unsafe { &*padapter };
    let sta = unsafe { &mut *psta };
    if sta.state & WIFI_ASOC_STATE == 0 {
        return;
    }
    unsafe {
        rtw_hal_update_sta_ra_info(padapter as *mut Adapter, psta as *mut StaInfo);
    }
    let ds_config = adapter.mlmepriv.cur_network.network.Configuration.DSConfig;
    let tx_ra_bitmap = sta.cmn.ra_info.ramask;
    sta.wireless_mode = wireless_mode_from_ramask(ds_config, tx_ra_bitmap, sta.vhtpriv.vht_option);
    unsafe {
        rtw_hal_update_sta_wset(padapter as *mut Adapter, psta as *mut StaInfo);
    }
}

#[cfg(not(host_ap_sta_ra_test))]
fn update_sta_ra_info_kernel(padapter: *mut Adapter, psta: *mut StaInfo) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_ap_sta_ra_state(psta) & WIFI_ASOC_STATE == 0 {
            return;
        }
        rtw_hal_update_sta_ra_info(padapter, psta);
        let ds_config = rtw_rust_ap_sta_ra_ds_config(padapter);
        let tx_ra_bitmap = rtw_rust_ap_sta_ra_ramask(psta);
        let vht_option = rtw_rust_ap_sta_ra_vht_option(psta);
        let mode = wireless_mode_from_ramask(ds_config, tx_ra_bitmap, vht_option);
        rtw_rust_ap_sta_ra_set_wireless_mode(psta, mode);
        rtw_hal_update_sta_wset(padapter, psta);
    }
}

#[no_mangle]
pub extern "C" fn rtw_ap_update_sta_ra_info(padapter: *mut Adapter, psta: *mut StaInfo) {
    #[cfg(host_ap_sta_ra_test)]
    update_sta_ra_info_host(padapter as *mut AdapterHost, psta as *mut StaInfoHost);
    #[cfg(not(host_ap_sta_ra_test))]
    update_sta_ra_info_kernel(padapter, psta);
}
