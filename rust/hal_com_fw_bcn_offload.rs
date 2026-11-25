// SPDX-License-Identifier: GPL-2.0
// FW beacon offload H2C (W4-138).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type Bool = u8;

const H2C_FW_BCN_OFFLOAD: U8 = 0xBA;
const H2C_BCN_OFFLOAD_LEN: U32 = 1;

fn set_bits_to_le_1byte(byte: &mut U8, bit_offset: U8, bit_len: U8, value: U8) {
    let mask = (0xFFu8 >> (8 - bit_len)) << bit_offset;
    *byte = (*byte & !mask) | ((value & (0xFFu8 >> (8 - bit_len))) << bit_offset);
}

#[cfg(host_hal_com_fw_bcn_offload_test)]
extern "C" {
    fn adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> i32;
}

#[cfg(not(host_hal_com_fw_bcn_offload_test))]
extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_dvobj_set_fw_bcn_offload(dvobj: *mut c_void, v: U8);
    fn rtw_rust_hal_com_dvobj_set_vap_tbtt_rpt_map(dvobj: *mut c_void, v: U8);
    fn rtw_rust_hal_com_log_fw_bcn_offload(en: U8, map: U8);
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> i32;
}

#[repr(C)]
struct DvobjOffloadFields {
    fw_bcn_offload: U8,
    vap_tbtt_rpt_map: U8,
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_fw_ap_bcn_offload_cmd(
    adapter: *mut c_void,
    fw_bcn_en: Bool,
    tbtt_rpt_map: U8,
) {
    let mut fw_bcn_offload = [0u8; H2C_BCN_OFFLOAD_LEN as usize];
    let dvobj = unsafe {
        #[cfg(host_hal_com_fw_bcn_offload_test)]
        {
            adapter_to_dvobj(adapter)
        }
        #[cfg(not(host_hal_com_fw_bcn_offload_test))]
        {
            rtw_rust_hal_com_adapter_to_dvobj(adapter)
        }
    };
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
        #[cfg(not(host_hal_com_fw_bcn_offload_test))]
        {
            rtw_rust_hal_com_dvobj_set_fw_bcn_offload(dvobj, fw_bcn_en);
            rtw_rust_hal_com_dvobj_set_vap_tbtt_rpt_map(dvobj, tbtt_rpt_map);
            rtw_rust_hal_com_log_fw_bcn_offload(fw_bcn_en, tbtt_rpt_map);
        }
        #[cfg(host_hal_com_fw_bcn_offload_test)]
        {
            let f = dvobj as *mut DvobjOffloadFields;
            if !f.is_null() {
                (*f).vap_tbtt_rpt_map = tbtt_rpt_map;
                (*f).fw_bcn_offload = fw_bcn_en;
            }
        }
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_FW_BCN_OFFLOAD,
            H2C_BCN_OFFLOAD_LEN,
            fw_bcn_offload.as_mut_ptr(),
        );
    }
}