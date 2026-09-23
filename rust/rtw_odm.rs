// SPDX-License-Identifier: GPL-2.0
//! W3-118 odm phydm ability + IC init (kernel Rust port).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub,
    unused_unsafe
)]

use core::ffi::c_void;

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
