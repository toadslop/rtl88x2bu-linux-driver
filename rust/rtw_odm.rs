// SPDX-License-Identifier: GPL-2.0
//! W3-118 odm phydm ability + IC init; W3-119 adaptivity msg/parm leaf.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub,
    unused_unsafe
)]

use core::ffi::{c_char, c_void};

const RTW_ADAPTIVITY_EN_DISABLE: u8 = 0;
const RTW_ADAPTIVITY_EN_ENABLE: u8 = 1;
const RTW_ADAPTIVITY_MODE_NORMAL: u8 = 0;
const RTW_ADAPTIVITY_MODE_CARRIER_SENSE: u8 = 1;
const ADAPTIVITY_VERSION: &[u8] = b"9.7.07";

const DYNAMIC_FUNC_DISABLE: u64 = 0;

type Adapter = c_void;
type DmStruct = c_void;

extern "C" {
    fn rtw_rust_odm_adapter_to_phydm(adapter: *mut Adapter) -> *mut DmStruct;
    fn rtw_rust_odm_support_ability(dm: *mut DmStruct) -> *mut u64;
    fn rtw_rust_odm_bk_support_ability(dm: *mut DmStruct) -> *mut u32;
    fn rtw_rust_odm_bk_rf_ability(adapter: *mut Adapter) -> *mut u64;
    fn rtw_rust_odm_halrf_cmn_info_set(dm: *mut DmStruct, value: u64);
    fn rtw_rust_odm_halrf_cmn_info_get(dm: *mut DmStruct) -> u64;
    fn rtw_rust_odm_odm_cmn_info_init(dm: *mut DmStruct, ic_type: u32);
    fn rtw_rust_odm_chip_type_to_ic(chip_type: u8) -> u32;
    fn rtw_rust_odm_get_chip_type(adapter: *mut Adapter) -> u8;
    fn rtw_rust_odm_warn_on(cond: i32);
    fn rtw_rust_odm_adaptivity_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_odm_adaptivity_phydm(adapter: *mut Adapter) -> *mut DmStruct;
    fn rtw_rust_odm_adaptivity_en(adapter: *mut Adapter) -> u8;
    fn rtw_rust_odm_adaptivity_mode(adapter: *mut Adapter) -> u8;
    fn rtw_rust_odm_adaptivity_th_l2h_ini(dm: *mut DmStruct) -> *mut i8;
    fn rtw_rust_odm_adaptivity_th_edcca_hl(dm: *mut DmStruct) -> *mut i8;
    fn rtw_rust_odm_adaptivity_rx_rate(dm: *mut DmStruct) -> u8;
    fn rtw_rust_odm_adaptivity_rssi_a(dm: *mut DmStruct) -> u8;
    fn rtw_rust_odm_adaptivity_rssi_b(dm: *mut DmStruct) -> u8;
    fn rtw_rust_odm_adaptivity_print_parm_line(sel: *mut c_void, th_l2h: u8, th_edcca_hl: i8);
    fn rtw_rust_odm_adaptivity_print_perpkt(sel: *mut c_void, rate: u8, rssi_a: u8, rssi_b: u8);
}

fn adaptivity_print(sel: *mut c_void, bytes: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(bytes.len(), buf.len() - 1);
    for i in 0..n {
        buf[i] = bytes[i] as i8;
    }
    unsafe {
        rtw_rust_odm_adaptivity_print_sel(sel, buf.as_ptr());
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_ability_ops(adapter: *mut Adapter, ops: u32, ability: u32) -> u32 {
    if adapter.is_null() {
        return 0;
    }
    unsafe {
        let dm = rtw_rust_odm_adapter_to_phydm(adapter);
        if dm.is_null() {
            return 0;
        }
        let mut result = 0u32;
        match ops {
            0 => {
                *rtw_rust_odm_support_ability(dm) = DYNAMIC_FUNC_DISABLE;
                rtw_rust_odm_halrf_cmn_info_set(dm, DYNAMIC_FUNC_DISABLE);
            }
            1 => *rtw_rust_odm_support_ability(dm) |= ability as u64,
            2 => *rtw_rust_odm_support_ability(dm) &= !(ability as u64),
            3 => {
                *rtw_rust_odm_bk_support_ability(dm) = *rtw_rust_odm_support_ability(dm) as u32;
                *rtw_rust_odm_bk_rf_ability(adapter) = rtw_rust_odm_halrf_cmn_info_get(dm);
            }
            4 => {
                *rtw_rust_odm_support_ability(dm) = *rtw_rust_odm_bk_support_ability(dm) as u64;
                rtw_rust_odm_halrf_cmn_info_set(dm, *rtw_rust_odm_bk_rf_ability(adapter));
            }
            5 => *rtw_rust_odm_support_ability(dm) = ability as u64,
            6 => result = *rtw_rust_odm_support_ability(dm) as u32,
            _ => {}
        }
        result
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_init_ic_type(adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let dm = rtw_rust_odm_adapter_to_phydm(adapter);
        if dm.is_null() {
            return;
        }
        let ic_type = rtw_rust_odm_chip_type_to_ic(rtw_rust_odm_get_chip_type(adapter));
        rtw_rust_odm_warn_on((ic_type == 0) as i32);
        rtw_rust_odm_odm_cmn_info_init(dm, ic_type);
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_adaptivity_ver_msg(sel: *mut c_void, adapter: *mut Adapter) {
    let _ = adapter;
    let mut line = [0u8; 48];
    let mut pos = 0usize;
    for &b in b"ADAPTIVITY_VERSION " {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in ADAPTIVITY_VERSION {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    if pos < line.len() {
        line[pos] = b'\n';
        pos += 1;
    }
    adaptivity_print(sel, &line[..pos]);
}

#[no_mangle]
pub extern "C" fn rtw_odm_adaptivity_en_msg(sel: *mut c_void, adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let en = rtw_rust_odm_adaptivity_en(adapter);
        adaptivity_print(sel, b"RTW_ADAPTIVITY_EN_");
        let tail = if en == RTW_ADAPTIVITY_EN_DISABLE {
            b"DISABLE\n" as &[u8]
        } else if en == RTW_ADAPTIVITY_EN_ENABLE {
            b"ENABLE\n" as &[u8]
        } else {
            b"INVALID\n" as &[u8]
        };
        adaptivity_print(sel, tail);
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_adaptivity_mode_msg(sel: *mut c_void, adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let mode = rtw_rust_odm_adaptivity_mode(adapter);
        adaptivity_print(sel, b"RTW_ADAPTIVITY_MODE_");
        let tail = if mode == RTW_ADAPTIVITY_MODE_NORMAL {
            b"NORMAL\n" as &[u8]
        } else if mode == RTW_ADAPTIVITY_MODE_CARRIER_SENSE {
            b"CARRIER_SENSE\n" as &[u8]
        } else {
            b"INVALID\n" as &[u8]
        };
        adaptivity_print(sel, tail);
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_adaptivity_config_msg(sel: *mut c_void, adapter: *mut Adapter) {
    rtw_odm_adaptivity_ver_msg(sel, adapter);
    rtw_odm_adaptivity_en_msg(sel, adapter);
    rtw_odm_adaptivity_mode_msg(sel, adapter);
}

#[no_mangle]
pub extern "C" fn rtw_odm_adaptivity_needed(adapter: *mut Adapter) -> u8 {
    if adapter.is_null() {
        return 0;
    }
    unsafe {
        if rtw_rust_odm_adaptivity_en(adapter) == RTW_ADAPTIVITY_EN_ENABLE {
            1
        } else {
            0
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_adaptivity_parm_set(
    adapter: *mut Adapter,
    th_l2h_ini: i8,
    th_edcca_hl_diff: i8,
) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let dm = rtw_rust_odm_adaptivity_phydm(adapter);
        if dm.is_null() {
            return;
        }
        *rtw_rust_odm_adaptivity_th_l2h_ini(dm) = th_l2h_ini;
        *rtw_rust_odm_adaptivity_th_edcca_hl(dm) = th_edcca_hl_diff;
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_adaptivity_parm_msg(sel: *mut c_void, adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let dm = rtw_rust_odm_adaptivity_phydm(adapter);
        if dm.is_null() {
            return;
        }
        rtw_odm_adaptivity_config_msg(sel, adapter);
        rtw_rust_odm_adaptivity_print_parm_line(
            sel,
            *rtw_rust_odm_adaptivity_th_l2h_ini(dm) as u8,
            *rtw_rust_odm_adaptivity_th_edcca_hl(dm),
        );
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_get_perpkt_rssi(sel: *mut c_void, adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let dm = rtw_rust_odm_adaptivity_phydm(adapter);
        if dm.is_null() {
            return;
        }
        rtw_rust_odm_adaptivity_print_perpkt(
            sel,
            rtw_rust_odm_adaptivity_rx_rate(dm),
            rtw_rust_odm_adaptivity_rssi_a(dm),
            rtw_rust_odm_adaptivity_rssi_b(dm),
        );
    }
}
