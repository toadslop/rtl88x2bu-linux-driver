// SPDX-License-Identifier: GPL-2.0
// Default port H2C cmd under CONFIG_FW_MULTI_PORT_SUPPORT (W4-131).

use core::ffi::c_void;

type U8 = u8;
type S32 = i32;

const H2C_DEFAULT_PORT_ID: U8 = 0x2c;
const H2C_DEFAULT_PORT_ID_LEN: u32 = 2;
const _SUCCESS: S32 = 1;
const _FAIL: S32 = 0;

extern "C" {
    fn rtw_hal_get_port(adapter: *mut c_void) -> U8;
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: u32,
        cmd_buf: *mut U8,
    ) -> S32;
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_dft_port_id(dvobj: *mut c_void) -> U8;
    fn rtw_rust_hal_com_dft_mac_id(dvobj: *mut c_void) -> U8;
    fn rtw_rust_hal_com_dft_set(dvobj: *mut c_void, port_id: U8, mac_id: U8);
    fn rtw_rust_hal_com_is_client_associated_to_ap(adapter: *mut c_void) -> S32;
    fn rtw_rust_hal_com_mlmepriv_is_ap(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_assoc_sta_mac_id(adapter: *mut c_void, found: *mut U8) -> U8;
    fn rtw_rust_hal_com_log_dft_port_h2c(
        adapter: *mut c_void,
        parm: *mut U8,
        port_id: U8,
        mac_id: U8,
    );
}

#[cfg(config_fw_multi_port_support)]
#[no_mangle]
pub extern "C" fn rtw_hal_set_default_port_id_cmd(adapter: *mut c_void, mac_id: U8) -> S32 {
    let mut ret = _SUCCESS;
    let mut parm = [0u8; H2C_DEFAULT_PORT_ID_LEN as usize];
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    if dvobj.is_null() {
        return _FAIL;
    }
    let port_id = unsafe { rtw_hal_get_port(adapter) };
    let cached_port = unsafe { rtw_rust_hal_com_dft_port_id(dvobj) };
    let cached_mac = unsafe { rtw_rust_hal_com_dft_mac_id(dvobj) };
    if cached_port == port_id && cached_mac == mac_id {
        return ret;
    }
    parm[0] = port_id;
    parm[1] = mac_id;
    unsafe {
        rtw_rust_hal_com_log_dft_port_h2c(adapter, parm.as_mut_ptr(), port_id, mac_id);
    }
    ret = unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_DEFAULT_PORT_ID,
            H2C_DEFAULT_PORT_ID_LEN,
            parm.as_mut_ptr(),
        )
    };
    unsafe { rtw_rust_hal_com_dft_set(dvobj, port_id, mac_id) };
    ret
}

#[cfg(config_fw_multi_port_support)]
#[no_mangle]
pub extern "C" fn rtw_set_default_port_id(adapter: *mut c_void) -> S32 {
    let mut ret = _SUCCESS;
    if unsafe { rtw_rust_hal_com_is_client_associated_to_ap(adapter) } != 0 {
        let mut found = 0u8;
        let mac_id = unsafe { rtw_rust_hal_com_assoc_sta_mac_id(adapter, &mut found as *mut U8) };
        if found != 0 {
            ret = unsafe { rtw_hal_set_default_port_id_cmd(adapter, mac_id) };
        }
    } else if unsafe { rtw_rust_hal_com_mlmepriv_is_ap(adapter) } != 0 {
    } else {
    }
    ret
}