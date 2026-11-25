// SPDX-License-Identifier: GPL-2.0
// FW beacon offload H2C (W4-138).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type Bool = u8;

const H2C_FW_BCN_OFFLOAD: U8 = 0xBA;
const H2C_BCN_RSVDPAGE: U8 = 0x09;
const H2C_BCN_OFFLOAD_LEN: U32 = 1;
const H2C_BCN_RSVDPAGE_LEN: U32 = 5;
const HAL_DEF_TX_PAGE_SIZE: i32 = 25;
const MAX_BEACON_LEN: U32 = 512;
const CONFIG_LIMITED_AP_NUM: usize = 4;

fn set_bits_to_le_1byte(byte: &mut U8, bit_offset: U8, bit_len: U8, value: U8) {
    let mask = (0xFFu8 >> (8 - bit_len)) << bit_offset;
    *byte = (*byte & !mask) | ((value & (0xFFu8 >> (8 - bit_len))) << bit_offset);
}

extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_dvobj_set_fw_bcn_offload(dvobj: *mut c_void, v: U8);
    fn rtw_rust_hal_com_dvobj_set_vap_tbtt_rpt_map(dvobj: *mut c_void, v: U8);
    fn rtw_rust_hal_com_dvobj_get_vap_map(dvobj: *mut c_void) -> U8;
    fn rtw_rust_hal_com_log_fw_bcn_offload(en: U8, map: U8);
    fn rtw_rust_hal_com_log_bcn_rsvdpage_loc(
        vap_map: U8,
        page_size: U32,
        b0: U8,
        b1: U8,
        b2: U8,
        b3: U8,
        b4: U8,
    );
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> i32;
    fn rtw_hal_get_def_var(adapter: *mut c_void, def: i32, val: *mut c_void);
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_fw_ap_bcn_offload_cmd(
    adapter: *mut c_void,
    fw_bcn_en: Bool,
    tbtt_rpt_map: U8,
) {
    let mut fw_bcn_offload = [0u8; H2C_BCN_OFFLOAD_LEN as usize];
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    if fw_bcn_en != 0 {
        set_bits_to_le_1byte(&mut fw_bcn_offload[0], 0, 1, 1);
    }
    if tbtt_rpt_map & (1u8 << 0) != 0 {
        set_bits_to_le_1byte(&mut fw_bcn_offload[0], 1, 1, 1);
    }
    if tbtt_rpt_map & (1u8 << 1) != 0 {
        set_bits_to_le_1byte(&mut fw_bcn_offload[0], 2, 1, 1);
    }
    if tbtt_rpt_map & (1u8 << 2) != 0 {
        set_bits_to_le_1byte(&mut fw_bcn_offload[0], 3, 1, 1);
    }
    if tbtt_rpt_map & (1u8 << 3) != 0 {
        set_bits_to_le_1byte(&mut fw_bcn_offload[0], 4, 1, 1);
    }
    unsafe {
        rtw_rust_hal_com_dvobj_set_vap_tbtt_rpt_map(dvobj, tbtt_rpt_map);
        rtw_rust_hal_com_dvobj_set_fw_bcn_offload(dvobj, fw_bcn_en);
        rtw_rust_hal_com_log_fw_bcn_offload(fw_bcn_en, tbtt_rpt_map);
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_FW_BCN_OFFLOAD,
            H2C_BCN_OFFLOAD_LEN,
            fw_bcn_offload.as_mut_ptr(),
        );
    }
}

#[cfg(any(not(host_hal_com_fw_bcn_offload_test), host_hal_com_fw_bcn_rsvdpage_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_bcn_rsvdpage_loc_cmd(adapter: *mut c_void) {
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let mut page_size: U32 = 0;
    let mut bcn_rsvdpage = [0u8; H2C_BCN_RSVDPAGE_LEN as usize];
    unsafe {
        rtw_hal_get_def_var(
            adapter,
            HAL_DEF_TX_PAGE_SIZE,
            &mut page_size as *mut U32 as *mut c_void,
        );
        let vap_map = rtw_rust_hal_com_dvobj_get_vap_map(dvobj);
        let bcn_page_num = MAX_BEACON_LEN / page_size;
        for vap_id in 0..CONFIG_LIMITED_AP_NUM {
            if vap_map & (1u8 << vap_id) != 0 {
                bcn_rsvdpage[vap_id] = (vap_id as U32 * bcn_page_num) as U8;
            }
        }
        rtw_rust_hal_com_log_bcn_rsvdpage_loc(
            vap_map,
            page_size,
            bcn_rsvdpage[0],
            bcn_rsvdpage[1],
            bcn_rsvdpage[2],
            bcn_rsvdpage[3],
            bcn_rsvdpage[4],
        );
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_BCN_RSVDPAGE,
            H2C_BCN_RSVDPAGE_LEN,
            bcn_rsvdpage.as_mut_ptr(),
        );
    }
}