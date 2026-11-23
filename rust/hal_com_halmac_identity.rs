// SPDX-License-Identifier: GPL-2.0
// HALMAC BSSID + port MAC accessors (W4-135).

use core::ffi::c_void;

type U8 = u8;

const ETH_ALEN: usize = 6;

#[cfg(not(host_hal_com_halmac_identity_test))]
extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_get_hw_port(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_log_set_macaddr_port(adapter: *mut c_void, hwport: U8, mac_addr: *mut U8);
    fn rtw_rust_hal_com_log_get_macaddr_port(adapter: *mut c_void, hwport: U8, mac_addr: *mut U8);
    fn rtw_rust_hal_com_log_set_bssid(adapter: *mut c_void, hw_port: U8, val: *mut U8);
}

extern "C" {
    fn rtw_hal_get_port(adapter: *mut c_void) -> U8;
    fn rtw_halmac_set_bssid(d: *mut c_void, hwport: i32, addr: *mut U8) -> i32;
    fn rtw_halmac_set_mac_address(d: *mut c_void, hwport: i32, addr: *mut U8) -> i32;
    fn rtw_halmac_get_mac_address(d: *mut c_void, hwport: i32, addr: *mut U8) -> i32;
    fn _rtw_memset(s: *mut c_void, c: i32, n: usize) -> *mut c_void;
}

fn adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void {
    #[cfg(host_hal_com_halmac_identity_test)]
    {
        return unsafe { host_halmac_identity_adapter_to_dvobj(adapter) };
    }
    #[cfg(not(host_hal_com_halmac_identity_test))]
    {
        return unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    }
}

#[cfg(not(host_hal_com_halmac_identity_test))]
fn hal_hw_port(adapter: *mut c_void) -> U8 {
    unsafe { rtw_rust_hal_com_get_hw_port(adapter) }
}

#[cfg(host_hal_com_halmac_identity_test)]
fn hal_hw_port(adapter: *mut c_void) -> U8 {
    unsafe { rtw_rust_hal_com_get_hw_port(adapter) }
}

#[cfg(any(config_rtw_halmac, host_hal_com_halmac_identity_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_bssid(adapter: *mut c_void, val: *mut U8) {
    let hw_port = unsafe { rtw_hal_get_port(adapter) };
    let dvobj = adapter_to_dvobj(adapter);
    unsafe { rtw_halmac_set_bssid(dvobj, hw_port as i32, val) };
    #[cfg(not(host_hal_com_halmac_identity_test))]
    unsafe {
        rtw_rust_hal_com_log_set_bssid(adapter, hw_port, val);
    }
}

#[cfg(all(
    any(config_rtw_halmac, host_hal_com_halmac_identity_test),
    not(config_mi_with_mbssid_cam),
))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_macaddr_port(adapter: *mut c_void, mac_addr: *mut U8) {
    if mac_addr.is_null() {
        return;
    }
    let hwport = hal_hw_port(adapter);
    let dvobj = adapter_to_dvobj(adapter);
    unsafe { rtw_halmac_set_mac_address(dvobj, hwport as i32, mac_addr) };
    #[cfg(not(host_hal_com_halmac_identity_test))]
    unsafe {
        rtw_rust_hal_com_log_set_macaddr_port(adapter, hwport, mac_addr);
    }
}

#[cfg(all(
    any(config_rtw_halmac, host_hal_com_halmac_identity_test),
    not(config_mi_with_mbssid_cam),
))]
#[no_mangle]
pub extern "C" fn rtw_hal_get_macaddr_port(adapter: *mut c_void, mac_addr: *mut U8) {
    if mac_addr.is_null() {
        return;
    }
    let hwport = hal_hw_port(adapter);
    let dvobj = adapter_to_dvobj(adapter);
    unsafe { _rtw_memset(mac_addr.cast(), 0, ETH_ALEN) };
    unsafe { rtw_halmac_get_mac_address(dvobj, hwport as i32, mac_addr) };
    #[cfg(not(host_hal_com_halmac_identity_test))]
    unsafe {
        rtw_rust_hal_com_log_get_macaddr_port(adapter, hwport, mac_addr);
    }
}