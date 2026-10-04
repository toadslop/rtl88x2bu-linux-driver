// SPDX-License-Identifier: GPL-2.0
//! W3-126 btcoex init/notify leaf — host L2 oracle (kernel port follows in later PRs).

#![allow(dead_code, improper_ctypes, missing_docs, non_snake_case)]

#[cfg(host_btcoex_init_notify_test)]
use std::os::raw::c_int;

type U8 = u8;
type U32 = u32;
const _TRUE: c_int = 1;
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
    buddy_survey: U8,
    buddy_asoc: U8,
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
    fn hal_pwr_off(a: *mut MockAdpt);
    fn hal_preload(a: *mut MockAdpt);
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
pub unsafe extern "C" fn o_pwr_off(a: *mut MockAdpt) {
    if !a.is_null() {
        hal_pwr_off(a);
    }
}

#[cfg(host_btcoex_init_notify_test)]
#[no_mangle]
pub unsafe extern "C" fn o_preload(a: *mut MockAdpt) {
    if !a.is_null() {
        hal_preload(a);
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
    if t == 0 && (ad.buddy_survey != 0 || ad.dv.mgmt_tx != 0 || ad.dv.roch != 0) {
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
    if st == RT_MEDIA_DISCONNECT && ad.buddy_asoc != 0 {
        return;
    }
    if st == RT_MEDIA_CONNECT && chk_fw(&ad.mlme, WIFI_AP_STATE as c_int) == _TRUE {
        host_btcoex_dl_rsvd_inc();
    }
    hal_media(a, st);
}
