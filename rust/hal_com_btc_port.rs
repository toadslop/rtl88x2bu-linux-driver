// SPDX-License-Identifier: GPL-2.0
// BT coexist WLAN port H2C under CONFIG_BT_COEXIST + CONFIG_FW_MULTI_PORT_SUPPORT (W4-144).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type S32 = i32;

const H2C_BTC_WL_PORT_ID: U8 = 0x71;
const H2C_BTC_WL_PORT_ID_LEN: U32 = 1;

#[inline]
fn set_bits_to_le_1byte(byte: &mut U8, bit_offset: U8, bit_len: U8, value: U8) {
    let mask = if bit_len >= 8 {
        0xffu8
    } else {
        (1u8 << bit_len).wrapping_sub(1)
    };
    let cleared = *byte & !(mask.wrapping_shl(bit_offset as u32));
    *byte = cleared | ((value & mask).wrapping_shl(bit_offset as u32));
}

#[inline]
fn set_h2ccmd_btc_wl_port_id(buf: &mut [u8; 1], value: U8) {
    set_bits_to_le_1byte(&mut buf[0], 0, 4, value);
}

extern "C" {
    fn rtw_hal_get_port(adapter: *mut c_void) -> U8;
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> S32;
    #[cfg(not(host_hal_com_btc_port_test))]
    fn rtw_rust_hal_com_log_wifi_btc_port_h2c(adapter: *mut c_void, hw_port: U8);
}

#[cfg(any(
    all(config_bt_coexist, config_fw_multi_port_support),
    host_hal_com_btc_port_test
))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_wifi_btc_port_id_cmd(adapter: *mut c_void) -> S32 {
    let mut h2c_buf = [0u8; H2C_BTC_WL_PORT_ID_LEN as usize];
    let hw_port = unsafe { rtw_hal_get_port(adapter) };

    set_h2ccmd_btc_wl_port_id(&mut h2c_buf, hw_port);

    #[cfg(not(host_hal_com_btc_port_test))]
    unsafe {
        rtw_rust_hal_com_log_wifi_btc_port_h2c(adapter, hw_port);
    }

    unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_BTC_WL_PORT_ID,
            H2C_BTC_WL_PORT_ID_LEN,
            h2c_buf.as_mut_ptr(),
        )
    }
}