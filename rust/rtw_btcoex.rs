// SPDX-License-Identifier: GPL-2.0
//! W3-126 btcoex init/notify leaf — host L2 oracle (kernel port follows in later PRs).

#![allow(dead_code, improper_ctypes, missing_docs, non_snake_case)]

#[cfg(any(host_btcoex_init_notify_test, host_btcoex_handler_policy_test))]
use std::os::raw::c_int;

type U8 = u8;
type U32 = u32;

#[cfg(any(host_btcoex_init_notify_test, host_btcoex_handler_policy_test))]
const _TRUE: c_int = 1;
#[cfg(any(host_btcoex_init_notify_test, host_btcoex_handler_policy_test))]
const _FALSE: c_int = 0;
const RT_MEDIA_CONNECT: U8 = 1;
const RT_MEDIA_DISCONNECT: U8 = 0;
const WIFI_AP_STATE: U32 = 0x10;

#[repr(C)]
struct MockMlme {
    fw_state: U32,
}

#[repr(C)]
struct MockHal {
    eeprom_coexist: U8,
}

#[repr(C)]
struct MockDv {
    mgmt_tx: U8,
    roch: U8,
}

#[repr(C)]
pub struct MockAdpt {
    mlme: MockMlme,
    hal: MockHal,
    dv: MockDv,
    buddy: U8,
    sreset: U8,
}

#[cfg(host_btcoex_init_notify_test)]
fn chk_fw(m: &MockMlme, st: c_int) -> c_int {
    if (st == 0 && m.fw_state == 0) || (m.fw_state & st as U32) != 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[cfg(host_btcoex_init_notify_test)]
extern "C" {
    fn hal_init(a: *mut MockAdpt);
    fn hal_pwr_on(a: *mut MockAdpt);
    fn hal_ant(a: *mut MockAdpt);
    fn hal_hw_init(a: *mut MockAdpt, w: U8);
    fn hal_ips(a: *mut MockAdpt, t: U8);
    fn hal_lps(a: *mut MockAdpt, t: U8);
    fn hal_scan(a: *mut MockAdpt, t: U8);
    fn hal_media(a: *mut MockAdpt, t: U8);
    fn host_btcoex_dl_rsvd_inc();
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_init(a: *mut MockAdpt) {
    if a.is_null() {
        return;
    }
    hal_init(a);
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_pwr_on(a: *mut MockAdpt) {
    if !a.is_null() {
        hal_pwr_on(a);
    }
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_ant(a: *mut MockAdpt) {
    if !a.is_null() {
        hal_ant(a);
    }
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_hw_init(a: *mut MockAdpt, w: U8) {
    if !a.is_null() {
        hal_hw_init(a, w);
    }
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_ips(a: *mut MockAdpt, t: U8) {
    if a.is_null() {
        return;
    }
    let ad = &*a;
    if ad.hal.eeprom_coexist == 0 {
        return;
    }
    hal_ips(a, t);
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_lps(a: *mut MockAdpt, t: U8) {
    if a.is_null() {
        return;
    }
    let ad = &*a;
    if ad.hal.eeprom_coexist == 0 {
        return;
    }
    hal_lps(a, t);
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_scan(a: *mut MockAdpt, t: U8) {
    if a.is_null() {
        return;
    }
    let ad = &*a;
    if ad.hal.eeprom_coexist == 0 {
        return;
    }
    if t == 0 && (ad.buddy != 0 || ad.dv.mgmt_tx != 0 || ad.dv.roch != 0) {
        return;
    }
    hal_scan(a, t);
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_media(a: *mut MockAdpt, st: U8) {
    if a.is_null() {
        return;
    }
    let ad = &*a;
    if ad.hal.eeprom_coexist == 0 || ad.sreset != 0 {
        return;
    }
    if st == RT_MEDIA_DISCONNECT && ad.buddy != 0 {
        return;
    }
    if st == RT_MEDIA_CONNECT && chk_fw(&ad.mlme, WIFI_AP_STATE as c_int) == _TRUE {
        host_btcoex_dl_rsvd_inc();
    }
    hal_media(a, st);
}

#[cfg(host_btcoex_handler_policy_test)]
type S32 = i32;

#[cfg(host_btcoex_handler_policy_test)]
#[repr(C)]
struct MockHalHp {
    eeprom_coexist: U8,
}

#[cfg(host_btcoex_handler_policy_test)]
#[repr(C)]
pub struct MockAdptHp {
    hal: MockHalHp,
}

#[cfg(host_btcoex_handler_policy_test)]
extern "C" {
    fn hal_btcoex_Hanlder(a: *mut MockAdptHp);
    fn hal_btcoex_IsBTCoexRejectAMPDU(a: *mut MockAdptHp) -> S32;
    fn hal_btcoex_IsBTCoexCtrlAMPDUSize(a: *mut MockAdptHp) -> S32;
    fn hal_btcoex_GetAMPDUSize(a: *mut MockAdptHp) -> U32;
    fn hal_btcoex_SetManualControl(a: *mut MockAdptHp, manual: U8);
    fn hal_btcoex_set_policy_control(a: *mut MockAdptHp, btc_policy: U8);
    fn hal_btcoex_IsBtDisabled(a: *mut MockAdptHp) -> U8;
    fn hal_btcoex_SetBTCoexist(a: *mut MockAdptHp, enable: U8);
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_handler(a: *mut MockAdptHp) {
    if a.is_null() {
        return;
    }
    if (*a).hal.eeprom_coexist == 0 {
        return;
    }
    hal_btcoex_Hanlder(a);
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_reject_ampdu(a: *mut MockAdptHp) -> S32 {
    if a.is_null() {
        return 0;
    }
    hal_btcoex_IsBTCoexRejectAMPDU(a)
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_ctrl_ampdu(a: *mut MockAdptHp) -> S32 {
    if a.is_null() {
        return 0;
    }
    hal_btcoex_IsBTCoexCtrlAMPDUSize(a)
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_get_ampdu(a: *mut MockAdptHp) -> U32 {
    if a.is_null() {
        return 0;
    }
    hal_btcoex_GetAMPDUSize(a)
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_set_manual(a: *mut MockAdptHp, manual: U8) {
    if a.is_null() {
        return;
    }
    if manual == _TRUE as U8 {
        hal_btcoex_SetManualControl(a, _TRUE as U8);
    } else {
        hal_btcoex_SetManualControl(a, _FALSE as U8);
    }
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_set_policy(a: *mut MockAdptHp, pol: U8) {
    if !a.is_null() {
        hal_btcoex_set_policy_control(a, pol);
    }
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_is_disabled(a: *mut MockAdptHp) -> U8 {
    if a.is_null() {
        return 0;
    }
    hal_btcoex_IsBtDisabled(a)
}

#[cfg(host_btcoex_handler_policy_test)]
#[no_mangle]
pub unsafe extern "C" fn o_switch(a: *mut MockAdptHp, en: U8) {
    if !a.is_null() {
        hal_btcoex_SetBTCoexist(a, en);
    }
}
