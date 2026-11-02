// SPDX-License-Identifier: GPL-2.0
//! Kernel half of W3-189 init part 1 (C FFI in `core/rtw_pwrctrl.c`).

use super::{
    rf_on, Padapter, PS_MODE_ACTIVE, PS_STATE_S4, RTW_PWR_STATE_CHK_INTERVAL, _FALSE, _TRUE,
};

extern "C" {
    fn rtw_rust_init_is_primary(padapter: Padapter) -> u8;
    fn rtw_rust_init_reg_u8(padapter: Padapter, field: u32) -> u8;
    fn rtw_rust_init_get_current_time() -> u32;
    fn rtw_rust_init_init_pwrlock(padapter: Padapter, which: u32);
    fn rtw_rust_init_store_u8(padapter: Padapter, field: u32, val: u8);
    fn rtw_rust_init_store_u32(padapter: Padapter, field: u32, val: u32);
    fn rtw_rust_init_hal_set_rpwm(padapter: Padapter);
    fn rtw_rust_init_init_workitem(padapter: Padapter, which: u32);
    fn rtw_rust_init_init_timer(padapter: Padapter);
    fn rtw_rust_init_create_wq(padapter: Padapter);
    fn rtw_rust_init_set_lpspg_name(padapter: Padapter);
}

use super::rtw_rust_init_store_u8_wowlan;

pub fn init_part1_kernel(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_init_is_primary(padapter) == _FALSE {
            return;
        }
        rtw_rust_init_init_pwrlock(padapter, 0);
        rtw_rust_init_init_pwrlock(padapter, 1);
        rtw_rust_init_store_u8(padapter, 0, rf_on);
        rtw_rust_init_store_u32(padapter, 0, 0);
        rtw_rust_init_store_u32(padapter, 1, 0);
        rtw_rust_init_store_u32(padapter, 2, 0);
        rtw_rust_init_store_u32(padapter, 3, 0);
        rtw_rust_init_store_u32(padapter, 4, _FALSE as u32);
        let ips = rtw_rust_init_reg_u8(padapter, 0);
        rtw_rust_init_store_u8(padapter, 1, ips);
        rtw_rust_init_store_u8(padapter, 2, ips);
        let now = rtw_rust_init_get_current_time();
        rtw_rust_init_store_u32(padapter, 5, now);
        rtw_rust_init_store_u8(padapter, 3, rtw_rust_init_reg_u8(padapter, 1));
        rtw_rust_init_store_u32(padapter, 6, RTW_PWR_STATE_CHK_INTERVAL);
        rtw_rust_init_store_u32(padapter, 7, 0);
        rtw_rust_init_store_u8(padapter, 4, _FALSE);
        rtw_rust_init_store_u8(padapter, 5, _FALSE);
        rtw_rust_init_store_u8(padapter, 6, 0);
        let mp = rtw_rust_init_reg_u8(padapter, 2);
        let pwr = if mp == 1 {
            PS_MODE_ACTIVE
        } else {
            rtw_rust_init_reg_u8(padapter, 3)
        };
        rtw_rust_init_store_u8(padapter, 7, pwr);
        rtw_rust_init_store_u8(
            padapter,
            8,
            if pwr != PS_MODE_ACTIVE { _TRUE } else { _FALSE },
        );
        rtw_rust_init_store_u8(padapter, 9, _FALSE);
        rtw_rust_init_store_u32(padapter, 8, now);
        rtw_rust_init_store_u8(padapter, 10, 0);
        rtw_rust_init_store_u8(padapter, 11, PS_STATE_S4);
        rtw_rust_init_store_u8(padapter, 12, PS_MODE_ACTIVE);
        rtw_rust_init_store_u8(padapter, 13, rtw_rust_init_reg_u8(padapter, 4));
        rtw_rust_init_store_u8(padapter, 14, 0);
        rtw_rust_init_store_u8(padapter, 15, 0);
        rtw_rust_init_store_u8(padapter, 16, 0x80);
        rtw_rust_init_store_u8(padapter, 17, 0);
        rtw_rust_init_hal_set_rpwm(padapter);
        rtw_rust_init_init_workitem(padapter, 0);
        rtw_rust_init_init_workitem(padapter, 1);
        rtw_rust_init_set_lpspg_name(padapter);
        rtw_rust_init_init_timer(padapter);
        rtw_rust_init_store_u8_wowlan(padapter, 18, _FALSE);
        rtw_rust_init_store_u8_wowlan(padapter, 19, _FALSE);
        rtw_rust_init_store_u8_wowlan(padapter, 20, _FALSE);
        rtw_rust_init_store_u8_wowlan(padapter, 21, _FALSE);
        rtw_rust_init_store_u8_wowlan(padapter, 22, 0);
        rtw_rust_init_init_workitem(padapter, 2);
        rtw_rust_init_create_wq(padapter);
    }
}
