// SPDX-License-Identifier: GPL-2.0
// rtw_hal_get_port + rtw_init_hal_com_default_value (W4-48).

use core::ffi::c_void;

type U8 = u8;

extern "C" {
    fn rtw_rust_hal_com_get_hw_port(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_get_clt_port(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_dbg_hw_port_validate(adapter: *mut c_void, hw_port: U8);
    fn rtw_rust_hal_com_read_reg_offloads(adapter: *mut c_void, iqk_fw_offload: *mut U8, ch_switch_offload: *mut U8);
    fn rtw_rust_hal_com_write_hal_com_init_defaults(adapter: *mut c_void, iqk_fw_offload: U8, ch_switch_offload: U8);
}

#[no_mangle]
pub extern "C" fn rtw_hal_get_port(adapter: *mut c_void) -> U8 {
    let mut hw_port = unsafe { rtw_rust_hal_com_get_hw_port(adapter) };
    let clt_port = unsafe { rtw_rust_hal_com_get_clt_port(adapter) };
    if clt_port != 0 {
        hw_port = clt_port;
    }
    unsafe { rtw_rust_hal_com_dbg_hw_port_validate(adapter, hw_port) };
    hw_port
}

#[no_mangle]
pub extern "C" fn rtw_init_hal_com_default_value(adapter: *mut c_void) {
    let mut iqk_fw_offload = 0u8;
    let mut ch_switch_offload = 0u8;
    unsafe {
        rtw_rust_hal_com_read_reg_offloads(adapter, &mut iqk_fw_offload, &mut ch_switch_offload);
    }
    unsafe {
        rtw_rust_hal_com_write_hal_com_init_defaults(adapter, iqk_fw_offload, ch_switch_offload);
    }
}
