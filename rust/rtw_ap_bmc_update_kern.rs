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
const WIRELESS_11G: u32 = 2;

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

fn is_enable_hw_ofdm(net_type: u32) -> bool {
    (net_type & (WIRELESS_11G | 0x0000_0020)) != 0
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
            return;
        }
        if rtw_rust_bmc_update_bmc_tx_rate(adapter) != MGN_UNKNOWN {
            rtw_rust_bmc_update_set_init_rate(psta, rtw_rust_bmc_update_bmc_tx_rate(adapter));
            return;
        }
        if rtw_rust_bmc_update_asoc_sta_count(adapter) <= 2 {
            return;
        }
        let mut tx_rate = rtw_ap_find_mini_tx_rate(adapter);
        #[cfg(bmc_tx_low_rate)]
        {
            tx_rate = rtw_ap_find_bmc_rate(adapter, tx_rate);
        }
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
        let wm = rtw_rust_bmc_update_wireless_mode(padapter);
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
        let init = if (rate_idx as usize) < 12 {
            BRATE[rate_idx as usize]
        } else {
            MGN_1M
        };
        rtw_rust_bmc_update_set_init_rate(psta, init);
    }
}
