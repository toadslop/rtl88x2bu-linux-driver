// SPDX-License-Identifier: GPL-2.0
//! HAL DM — `Init_ODM_ComInfo` board-type block from `hal/hal_dm.c` (W4-30).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

type DmStruct = u8;

const ODM_BOARD_DEFAULT: u8 = 0;
const ODM_BOARD_BT: u8 = 1 << 2;
const ODM_BOARD_EXT_PA: u8 = 1 << 3;
const ODM_BOARD_EXT_LNA: u8 = 1 << 4;
const ODM_BOARD_EXT_PA_5G: u8 = 1 << 6;
const ODM_BOARD_EXT_LNA_5G: u8 = 1 << 7;

const ODM_CMNINFO_BOARD_TYPE: u32 = 12;
const ODM_CMNINFO_EXT_LNA: u32 = 14;
const ODM_CMNINFO_5G_EXT_LNA: u32 = 15;
const ODM_CMNINFO_EXT_PA: u32 = 16;
const ODM_CMNINFO_5G_EXT_PA: u32 = 17;

unsafe extern "C" {
    fn odm_cmn_info_init(dm: *mut DmStruct, cmn_info: u32, value: u64);
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_dm_init_odm_board_type(
    pdm_odm: *mut DmStruct, // struct dm_struct — opaque
    external_lna_2g: u8,
    external_lna_5g: u8,
    external_pa_2g: u8,
    external_pa_5g: u8,
    eeprom_bt_coexist: u8,
) {
    if pdm_odm.is_null() {
        return;
    }

    let mut odm_board_type = ODM_BOARD_DEFAULT;

    if external_lna_2g != 0 {
        odm_board_type |= ODM_BOARD_EXT_LNA;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_EXT_LNA, 1);
        }
    }
    if external_lna_5g != 0 {
        odm_board_type |= ODM_BOARD_EXT_LNA_5G;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_5G_EXT_LNA, 1);
        }
    }
    if external_pa_2g != 0 {
        odm_board_type |= ODM_BOARD_EXT_PA;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_EXT_PA, 1);
        }
    }
    if external_pa_5g != 0 {
        odm_board_type |= ODM_BOARD_EXT_PA_5G;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_5G_EXT_PA, 1);
        }
    }
    if eeprom_bt_coexist != 0 {
        odm_board_type |= ODM_BOARD_BT;
    }

    unsafe {
        odm_cmn_info_init(pdm_odm, ODM_CMNINFO_BOARD_TYPE, odm_board_type as u64);
    }
}
