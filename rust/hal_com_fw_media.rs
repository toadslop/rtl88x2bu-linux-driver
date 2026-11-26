// SPDX-License-Identifier: GPL-2.0
// FW media status report H2C (W4-139).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type S32 = i32;
type Bool = u8;

const H2C_MEDIA_STATUS_RPT: U8 = 0x01;
const CHANNEL_WIDTH_20: U8 = 0;
const SUCCESS: S32 = 1;

#[cfg(config_fw_multi_port_support)]
const H2C_MEDIA_STATUS_RPT_LEN: U32 = 4;

#[cfg(not(config_fw_multi_port_support))]
const H2C_MEDIA_STATUS_RPT_LEN: U32 = 3;

fn set_bits_to_le_1byte(byte: &mut U8, bit_offset: U8, bit_len: U8, value: U8) {
    let mask = (0xFFu8 >> (8 - bit_len)) << bit_offset;
    *byte = (*byte & !mask) | ((value & (0xFFu8 >> (8 - bit_len))) << bit_offset);
}

extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_dvobj_macid_ctl(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> S32;
    fn rtw_macid_ctl_set_h2c_msr(macid_ctl: *mut c_void, id: U8, h2c_msr: U8) -> U8;
    fn rtw_macid_ctl_set_bw(macid_ctl: *mut c_void, id: U8, bw: U8);
    fn rtw_macid_ctl_set_vht_en(macid_ctl: *mut c_void, id: U8, en: U8);
    fn rtw_macid_ctl_set_rate_bmp0(macid_ctl: *mut c_void, id: U8, bmp: U32);
    fn rtw_macid_ctl_set_rate_bmp1(macid_ctl: *mut c_void, id: U8, bmp: U32);
    fn rtw_hal_update_tx_aclt(adapter: *mut c_void);
    fn rtw_update_tx_rate_bmp(dvobj: *mut c_void);
    #[cfg(config_fw_multi_port_support)]
    fn rtw_hal_get_port(adapter: *mut c_void) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_FwMediaStatusRpt_cmd(
    adapter: *mut c_void,
    opmode: Bool,
    miracast: Bool,
    miracast_sink: Bool,
    role: U8,
    macid: U8,
    macid_ind: Bool,
    mut macid_end: U8,
) -> S32 {
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let macid_ctl = unsafe { rtw_rust_hal_com_dvobj_macid_ctl(dvobj) };
    let mut parm = [0u8; 4];
    let mut op_num_change_bmp: U8 = 0;

    set_bits_to_le_1byte(&mut parm[0], 0, 1, opmode);
    set_bits_to_le_1byte(&mut parm[0], 1, 1, macid_ind);
    set_bits_to_le_1byte(&mut parm[0], 2, 1, miracast);
    set_bits_to_le_1byte(&mut parm[0], 3, 1, miracast_sink);
    set_bits_to_le_1byte(&mut parm[0], 4, 4, role);
    set_bits_to_le_1byte(&mut parm[1], 0, 8, macid);
    set_bits_to_le_1byte(&mut parm[2], 0, 8, macid_end);
    #[cfg(config_fw_multi_port_support)]
    {
        let hw_port = unsafe { rtw_hal_get_port(adapter) };
        set_bits_to_le_1byte(&mut parm[3], 0, 3, hw_port);
    }

    let ret = unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_MEDIA_STATUS_RPT,
            H2C_MEDIA_STATUS_RPT_LEN,
            parm.as_mut_ptr(),
        )
    };
    if ret != SUCCESS {
        return ret;
    }

    set_bits_to_le_1byte(&mut parm[0], 1, 1, 0);
    if macid_ind == 0 {
        macid_end = macid;
    }

    let mut i = macid as i32;
    let end = macid_end as i32;
    while i <= end {
        let id = i as U8;
        op_num_change_bmp |= unsafe { rtw_macid_ctl_set_h2c_msr(macid_ctl, id, parm[0]) };
        if opmode == 0 {
            unsafe {
                rtw_macid_ctl_set_bw(macid_ctl, id, CHANNEL_WIDTH_20);
                rtw_macid_ctl_set_vht_en(macid_ctl, id, 0);
                rtw_macid_ctl_set_rate_bmp0(macid_ctl, id, 0);
                rtw_macid_ctl_set_rate_bmp1(macid_ctl, id, 0);
            }
        }
        i += 1;
    }

    #[cfg(config_tx_ac_lifetime)]
    if op_num_change_bmp != 0 {
        unsafe { rtw_hal_update_tx_aclt(adapter) };
    }

    if opmode == 0 {
        unsafe { rtw_update_tx_rate_bmp(dvobj) };
    }

    ret
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_FwMediaStatusRpt_single_cmd(
    adapter: *mut c_void,
    opmode: Bool,
    miracast: Bool,
    miracast_sink: Bool,
    role: U8,
    macid: U8,
) -> S32 {
    rtw_hal_set_FwMediaStatusRpt_cmd(
        adapter, opmode, miracast, miracast_sink, role, macid, 0, 0,
    )
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_FwMediaStatusRpt_range_cmd(
    adapter: *mut c_void,
    opmode: Bool,
    miracast: Bool,
    miracast_sink: Bool,
    role: U8,
    macid: U8,
    macid_end: U8,
) -> S32 {
    rtw_hal_set_FwMediaStatusRpt_cmd(
        adapter, opmode, miracast, miracast_sink, role, macid, 1, macid_end,
    )
}