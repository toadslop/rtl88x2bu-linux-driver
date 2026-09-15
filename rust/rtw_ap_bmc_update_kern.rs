// SPDX-License-Identifier: GPL-2.0
//! W3-80 BMC update tx-rate helpers — kernel object (`CONFIG_RUST_AP_BMC_UPDATE`).

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
type Adapter = *mut c_void;
type StaInfo = *mut c_void;

const MGN_UNKNOWN: U8 = 0x00;
const MGN_1M: U8 = 0x02;
const MGN_2M: U8 = 0x04;
const MGN_5_5M: U8 = 0x0b;
const MGN_6M: U8 = 0x0c;
const MGN_9M: U8 = 0x12;
const MGN_11M: U8 = 0x16;
const MGN_12M: U8 = 0x18;
const MGN_18M: U8 = 0x24;
const MGN_24M: U8 = 0x30;
const MGN_36M: U8 = 0x48;
const MGN_48M: U8 = 0x60;
const MGN_54M: U8 = 0x6c;

extern "C" {
    fn rtw_get_bcmc_stainfo(adapter: Adapter) -> StaInfo;
    fn rtw_ap_find_mini_tx_rate(adapter: Adapter) -> U8;
    #[cfg(bmc_tx_low_rate)]
    fn rtw_ap_find_bmc_rate(adapter: Adapter, tx_rate: U8) -> U8;
    fn rtw_rust_bmc_update_bmc_tx_rate(adapter: Adapter) -> U8;
    fn rtw_rust_bmc_update_asoc_sta_count(adapter: Adapter) -> i32;
    fn rtw_rust_bmc_update_set_init_rate(psta: StaInfo, rate: U8);
    fn rtw_rust_bmc_update_mlme_is_ap(adapter: Adapter) -> U8;
    fn rtw_rust_bmc_update_mlme_is_mesh(adapter: Adapter) -> U8;
    fn rtw_rust_bmc_update_wireless_mode(adapter: Adapter) -> u32;
    fn rtw_rust_bmc_update_sta_ramask(psta: StaInfo) -> u64;
    fn rtw_rust_bmc_update_is_enable_hw_ofdm(adapter: Adapter) -> U8;
    fn rtw_rust_bmc_update_err_missing_bmc_sta(adapter: Adapter);
    fn rtw_get_rateset_len(rateset: *mut U8) -> u32;
    fn rtw_check_network_type(rate: *mut U8, ratelen: i32, channel: i32) -> i32;
    fn update_sta_basic_rate(psta: StaInfo, wireless_mode: U8);
    fn rtw_hal_update_sta_ra_info(padapter: Adapter, psta: StaInfo);
    fn rtw_sta_media_status_rpt(padapter: Adapter, psta: StaInfo, connected: u8);
    fn rtw_rust_bmc_update_cur_supported_rates(adapter: Adapter) -> *mut U8;
    fn rtw_rust_bmc_update_cur_ds_config(adapter: Adapter) -> i32;
    fn rtw_rust_bmc_update_sta_prepare(adapter: Adapter, psta: StaInfo);
    fn rtw_rust_bmc_update_sta_set_asoc(psta: StaInfo);
    fn rtw_rust_bmc_update_sta_set_wireless_mode(psta: StaInfo, mode: U8);
}

const WIRELESS_11B: i32 = 1;
const WIRELESS_11A: i32 = 4;
const WIRELESS_INVALID: i32 = 0;

fn is_supported_tx_cck_kern(net_type: U8) -> bool {
    (net_type as i32 & WIRELESS_11B) != 0
}

fn hw_rate_to_m_rate(hw_rate: U8) -> U8 {
    const HW_TO_MGN: [U8; 12] = [
        MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M,
        MGN_48M, MGN_54M,
    ];
    if (hw_rate as usize) < 12 {
        HW_TO_MGN[hw_rate as usize]
    } else {
        MGN_1M
    }
}

fn get_lowest_rate_idx_ex(mask: u64, start_bit: u32) -> U8 {
    for i in start_bit..64 {
        if (mask >> i) & 1 != 0 {
            return i as U8;
        }
    }
    0
}

fn get_lowest_rate_idx(mask: u64) -> U8 {
    get_lowest_rate_idx_ex(mask, 0)
}

#[cfg(not(bmc_tx_low_rate))]
fn get_highest_rate_idx(mask: u64) -> U8 {
    for i in (0..64).rev() {
        if (mask >> i) & 1 != 0 {
            return i as U8;
        }
    }
    0
}

#[cfg(bmc_tx_rate_select)]
#[no_mangle]
pub extern "C" fn rtw_update_bmc_sta_tx_rate(adapter: Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let psta = rtw_get_bcmc_stainfo(adapter);
        if psta.is_null() {
            rtw_rust_bmc_update_err_missing_bmc_sta(adapter);
            return;
        }
        if rtw_rust_bmc_update_bmc_tx_rate(adapter) != MGN_UNKNOWN {
            rtw_rust_bmc_update_set_init_rate(psta, rtw_rust_bmc_update_bmc_tx_rate(adapter));
            return;
        }
        if rtw_rust_bmc_update_asoc_sta_count(adapter) <= 2 {
            return;
        }
        #[cfg(bmc_tx_low_rate)]
        let tx_rate = {
            let mini = rtw_ap_find_mini_tx_rate(adapter);
            rtw_ap_find_bmc_rate(adapter, mini)
        };
        #[cfg(not(bmc_tx_low_rate))]
        let tx_rate = rtw_ap_find_mini_tx_rate(adapter);
        rtw_rust_bmc_update_set_init_rate(psta, hw_rate_to_m_rate(tx_rate));
    }
}

#[no_mangle]
pub extern "C" fn rtw_init_bmc_sta_tx_rate(padapter: Adapter, psta: StaInfo) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    const BRATE: [U8; 12] = [
        MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M,
        MGN_48M, MGN_54M,
    ];
    unsafe {
        if rtw_rust_bmc_update_mlme_is_ap(padapter) == 0
            && rtw_rust_bmc_update_mlme_is_mesh(padapter) == 0
        {
            return;
        }
        let fixed = rtw_rust_bmc_update_bmc_tx_rate(padapter);
        if fixed != MGN_UNKNOWN {
            rtw_rust_bmc_update_set_init_rate(psta, fixed);
            return;
        }
        let ramask = rtw_rust_bmc_update_sta_ramask(psta);
        let rate_idx = {
            #[cfg(bmc_tx_low_rate)]
            {
                if rtw_rust_bmc_update_is_enable_hw_ofdm(padapter) != 0 && ramask != 0 {
                    get_lowest_rate_idx_ex(ramask, 4)
                } else {
                    get_lowest_rate_idx(ramask)
                }
            }
            #[cfg(not(bmc_tx_low_rate))]
            {
                get_highest_rate_idx(ramask)
            }
        };
        let init = if (rate_idx as usize) < 12 {
            BRATE[rate_idx as usize]
        } else {
            MGN_1M
        };
        rtw_rust_bmc_update_set_init_rate(psta, init);
    }
}

#[no_mangle]
pub extern "C" fn update_bmc_sta(padapter: Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let psta = rtw_get_bcmc_stainfo(padapter);
        if psta.is_null() {
            return;
        }
        rtw_rust_bmc_update_sta_prepare(padapter, psta);
        let rates = rtw_rust_bmc_update_cur_supported_rates(padapter);
        let ds_config = rtw_rust_bmc_update_cur_ds_config(padapter);
        let support_rate_num = rtw_get_rateset_len(rates) as i32;
        let mut network_type = rtw_check_network_type(rates, support_rate_num, ds_config) as U8;
        if is_supported_tx_cck_kern(network_type) {
            network_type = WIRELESS_11B as U8;
        } else if network_type as i32 == WIRELESS_INVALID {
            network_type = if ds_config > 14 {
                WIRELESS_11A
            } else {
                WIRELESS_11B
            } as U8;
        }
        update_sta_basic_rate(psta, network_type);
        rtw_rust_bmc_update_sta_set_wireless_mode(psta, network_type);
        rtw_hal_update_sta_ra_info(padapter, psta);
        rtw_rust_bmc_update_sta_set_asoc(psta);
        rtw_sta_media_status_rpt(padapter, psta, 1);
        rtw_init_bmc_sta_tx_rate(padapter, psta);
    }
}
