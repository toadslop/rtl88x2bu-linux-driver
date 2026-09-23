// SPDX-License-Identifier: GPL-2.0
//! W3-118 odm phydm ability + IC init — Rust port of `core/rtw_odm_phydm_init.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub,
    unused_unsafe
)]

const DYNAMIC_FUNC_DISABLE: u64 = 0;

#[repr(u32)]
enum HalPhydmOps {
    DisAllFunc = 0,
    FuncSet = 1,
    FuncClr = 2,
    AbilityBk = 3,
    AbilityRestore = 4,
    AbilitySet = 5,
    AbilityGet = 6,
}

#[cfg(host_odm_phydm_init_test)]
mod host {
    use super::{DYNAMIC_FUNC_DISABLE, HalPhydmOps};

    #[repr(C)]
    pub struct DmStruct {
        pub support_ability: u64,
        pub bk_support_ability: u32,
    }

    #[repr(C)]
    pub struct HalDataType {
        pub odmpriv: DmStruct,
        pub bk_rf_ability: u64,
    }

    #[repr(C)]
    pub struct Adapter {
        pub HalData: HalDataType,
        pub chip_type: u8,
    }

    pub static mut G_RF_ABILITY: u64 = 0;
    pub static mut G_IC_TYPE: u32 = 0;

    static CHIP_MAP: [u32; 11] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 << 7];

    pub unsafe fn chip_type_to_odm_ic_type(chip: u8) -> u32 {
        if (chip as usize) < CHIP_MAP.len() {
            CHIP_MAP[chip as usize]
        } else {
            0
        }
    }

    pub unsafe fn halrf_cmn_info_set(_dm: *mut DmStruct, value: u64) {
        G_RF_ABILITY = value;
    }

    pub unsafe fn halrf_cmn_info_get(_dm: *mut DmStruct) -> u64 {
        G_RF_ABILITY
    }

    pub unsafe fn odm_cmn_info_init(_dm: *mut DmStruct, ic: u32) {
        G_IC_TYPE = ic;
    }

    pub unsafe fn reset_host() {
        G_RF_ABILITY = 0;
        G_IC_TYPE = 0;
    }

    pub unsafe fn phydm_ability_ops(adapter: *mut Adapter, ops: u32, ability: u32) -> u32 {
        let dm = &mut (*adapter).HalData.odmpriv as *mut DmStruct;
        let mut result = 0u32;
        match ops {
            x if x == HalPhydmOps::DisAllFunc as u32 => {
                (*dm).support_ability = DYNAMIC_FUNC_DISABLE;
                halrf_cmn_info_set(dm, DYNAMIC_FUNC_DISABLE);
            }
            x if x == HalPhydmOps::FuncSet as u32 => (*dm).support_ability |= ability as u64,
            x if x == HalPhydmOps::FuncClr as u32 => (*dm).support_ability &= !(ability as u64),
            x if x == HalPhydmOps::AbilityBk as u32 => {
                (*dm).bk_support_ability = (*dm).support_ability as u32;
                (*adapter).HalData.bk_rf_ability = halrf_cmn_info_get(dm);
            }
            x if x == HalPhydmOps::AbilityRestore as u32 => {
                (*dm).support_ability = (*dm).bk_support_ability as u64;
                halrf_cmn_info_set(dm, (*adapter).HalData.bk_rf_ability);
            }
            x if x == HalPhydmOps::AbilitySet as u32 => (*dm).support_ability = ability as u64,
            x if x == HalPhydmOps::AbilityGet as u32 => result = (*dm).support_ability as u32,
            _ => {}
        }
        result
    }

    pub unsafe fn odm_init_ic_type(adapter: *mut Adapter) {
        let dm = &mut (*adapter).HalData.odmpriv as *mut DmStruct;
        let ic_type = chip_type_to_odm_ic_type((*adapter).chip_type);
        let _ = ic_type;
        odm_cmn_info_init(dm, ic_type);
    }
}

#[cfg(not(host_odm_phydm_init_test))]
mod kernel {
    use super::{DYNAMIC_FUNC_DISABLE, HalPhydmOps};
    use core::ffi::c_void;

    pub type Adapter = c_void;
    pub type DmStruct = c_void;

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

    pub unsafe fn phydm_ability_ops(adapter: *mut Adapter, ops: u32, ability: u32) -> u32 {
        unsafe {
        let mut result = 0u32;
        let dm = rtw_rust_odm_adapter_to_phydm(adapter);
        if dm.is_null() {
            return 0;
        }
        match ops {
            x if x == HalPhydmOps::DisAllFunc as u32 => {
                *rtw_rust_odm_support_ability(dm) = DYNAMIC_FUNC_DISABLE;
                rtw_rust_odm_halrf_cmn_info_set(dm, DYNAMIC_FUNC_DISABLE);
            }
            x if x == HalPhydmOps::FuncSet as u32 => {
                *rtw_rust_odm_support_ability(dm) |= ability as u64;
            }
            x if x == HalPhydmOps::FuncClr as u32 => {
                *rtw_rust_odm_support_ability(dm) &= !(ability as u64);
            }
            x if x == HalPhydmOps::AbilityBk as u32 => {
                *rtw_rust_odm_bk_support_ability(dm) = *rtw_rust_odm_support_ability(dm) as u32;
                *rtw_rust_odm_bk_rf_ability(adapter) = rtw_rust_odm_halrf_cmn_info_get(dm);
            }
            x if x == HalPhydmOps::AbilityRestore as u32 => {
                *rtw_rust_odm_support_ability(dm) = *rtw_rust_odm_bk_support_ability(dm) as u64;
                rtw_rust_odm_halrf_cmn_info_set(dm, *rtw_rust_odm_bk_rf_ability(adapter));
            }
            x if x == HalPhydmOps::AbilitySet as u32 => {
                *rtw_rust_odm_support_ability(dm) = ability as u64;
            }
            x if x == HalPhydmOps::AbilityGet as u32 => {
                result = *rtw_rust_odm_support_ability(dm) as u32;
            }
            _ => {}
        }
        result
        }
    }

    pub unsafe fn odm_init_ic_type(adapter: *mut Adapter) {
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
}

#[no_mangle]
pub extern "C" fn rtw_phydm_ability_ops(adapter: *mut core::ffi::c_void, ops: u32, ability: u32) -> u32 {
    if adapter.is_null() {
        return 0;
    }
    unsafe {
        #[cfg(host_odm_phydm_init_test)]
        return host::phydm_ability_ops(adapter as *mut host::Adapter, ops, ability);
        #[cfg(not(host_odm_phydm_init_test))]
        return kernel::phydm_ability_ops(adapter as *mut kernel::Adapter, ops, ability);
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_init_ic_type(adapter: *mut core::ffi::c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        #[cfg(host_odm_phydm_init_test)]
        host::odm_init_ic_type(adapter as *mut host::Adapter);
        #[cfg(not(host_odm_phydm_init_test))]
        kernel::odm_init_ic_type(adapter as *mut kernel::Adapter);
    }
}

#[cfg(host_odm_phydm_init_test)]
#[no_mangle]
pub extern "C" fn host_odm_reset(_adapter: *mut host::Adapter) {
    unsafe {
        host::reset_host();
    }
}

#[cfg(host_odm_phydm_init_test)]
#[no_mangle]
pub extern "C" fn host_odm_ic_type(_dm: *mut host::DmStruct) -> u32 {
    let _ = _dm;
    unsafe { host::G_IC_TYPE }
}

#[cfg(host_odm_phydm_init_test)]
#[no_mangle]
pub extern "C" fn host_odm_rf_ability(_dm: *mut host::DmStruct) -> u32 {
    let _ = _dm;
    unsafe { host::G_RF_ABILITY as u32 }
}
