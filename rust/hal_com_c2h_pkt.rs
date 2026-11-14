// SPDX-License-Identifier: GPL-2.0
// CONFIG_FW_C2H_PKT dispatch wrappers (W4-77 PR1: RTW_HALMAC path).

use core::ffi::c_void;

type U8 = u8;
type U16 = u16;

extern "C" {
    #[cfg(config_rtw_halmac)]
    fn rtw_rust_hal_com_hal_mac_c2h_handler(adapter: *mut c_void, buf: *mut U8, len: U16);
}

#[no_mangle]
pub extern "C" fn rtw_hal_c2h_pkt_pre_hdl(adapter: *mut c_void, buf: *mut U8, len: U16) {
    #[cfg(config_rtw_halmac)]
    {
        let _ = (adapter, buf, len);
    }
    #[cfg(not(config_rtw_halmac))]
    {
        let _ = (adapter, buf, len);
        // Non-HALMAC body lands in PR2.
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_c2h_pkt_hdl(adapter: *mut c_void, buf: *mut U8, len: U16) {
    #[cfg(config_rtw_halmac)]
    unsafe {
        rtw_rust_hal_com_hal_mac_c2h_handler(adapter, buf, len);
    }
    #[cfg(not(config_rtw_halmac))]
    {
        let _ = (adapter, buf, len);
    }
}