// SPDX-License-Identifier: GPL-2.0
//! Kernel init part 2 + free (W3-190).

use super::{Padapter, _FALSE};

extern "C" {
    fn rtw_rust_init_is_primary(padapter: Padapter) -> u8;
    fn rtw_rust_init_reg_u8(padapter: Padapter, field: u32) -> u8;
    fn rtw_rust_init_reg_u32(padapter: Padapter, field: u32) -> u32;
    fn rtw_rust_init_store_u8(padapter: Padapter, field: u32, val: u8);
    fn rtw_rust_init_store_i8(padapter: Padapter, field: u32, val: i8);
    fn rtw_rust_init_wow_pattern_reset(padapter: Padapter);
    fn rtw_rust_init_wowlan_tail(padapter: Padapter);
    fn rtw_rust_init_lps_poff_init(padapter: Padapter);
    fn rtw_rust_init_lps_ack_init(padapter: Padapter);
    fn rtw_rust_init_wow_hw_cam_mutex_init(padapter: Padapter);
    fn rtw_rust_init_free_workqueue(padapter: Padapter);
    fn rtw_rust_init_cancel_lps_workitems(padapter: Padapter);
    fn rtw_rust_init_free_lps_pg(padapter: Padapter);
    fn rtw_rust_init_free_wow_mutex(padapter: Padapter);
    fn rtw_rust_init_unregister_early_suspend(padapter: Padapter);
    fn rtw_rust_init_free_pwrlocks(padapter: Padapter);
    fn rtw_rust_init_free_lps_ack_mutex(padapter: Padapter);
    fn rtw_rust_init_lps_poff_deinit(padapter: Padapter);
}

#[cfg(config_wowlan)]
use super::rtw_rust_init_store_u8_wowlan;

pub fn init_part2_kernel(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_init_is_primary(padapter) == _FALSE {
            return;
        }
        #[cfg(config_wowlan)]
        {
            let wow_pwr = rtw_rust_init_reg_u8(padapter, 5);
            let wow_lvl = rtw_rust_init_reg_u8(padapter, 6);
            let wake = rtw_rust_init_reg_u32(padapter, 0);
            rtw_rust_init_store_u8_wowlan(padapter, 23, wow_pwr);
            rtw_rust_init_store_u8_wowlan(padapter, 24, wow_lvl);
            let def_pat = if wake & (1 << 1) != 0 { 1 } else { 0 };
            rtw_rust_init_store_u8_wowlan(padapter, 25, def_pat);
            rtw_rust_init_wow_pattern_reset(padapter);
            rtw_rust_init_store_u8_wowlan(padapter, 26, 0);
            rtw_rust_init_wow_hw_cam_mutex_init(padapter);
            rtw_rust_init_wowlan_tail(padapter);
        }
        rtw_rust_init_lps_poff_init(padapter);
        rtw_rust_init_lps_ack_init(padapter);
    }
}

pub fn free_pwrctrl_kernel(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_init_is_primary(padapter) == _FALSE {
            return;
        }
        rtw_rust_init_free_workqueue(padapter);
        rtw_rust_init_lps_poff_deinit(padapter);
        rtw_rust_init_cancel_lps_workitems(padapter);
        rtw_rust_init_free_lps_pg(padapter);
        rtw_rust_init_free_wow_mutex(padapter);
        rtw_rust_init_unregister_early_suspend(padapter);
        rtw_rust_init_free_pwrlocks(padapter);
        rtw_rust_init_free_lps_ack_mutex(padapter);
    }
}
