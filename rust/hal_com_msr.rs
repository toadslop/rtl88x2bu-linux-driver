// SPDX-License-Identifier: GPL-2.0
// MSR network type get/set under RTW_HALMAC (W4-134).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const HW_PORT0: U8 = 0;
const _HW_STATE_NOLINK_: U8 = 0x00;
const _HW_STATE_AP_: U8 = 0x03;

#[cfg(not(host_hal_com_msr_test))]
extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_msr_hw_port(adapter: *mut c_void) -> U8;
}

extern "C" {
    fn rtw_halmac_get_network_type(d: *mut c_void, hwport: i32, net_type: *mut U8) -> i32;
    fn rtw_halmac_set_network_type(d: *mut c_void, hwport: i32, net_type: U8) -> i32;
    fn rtw_get_mbid_cam_entry_num(adapter: *mut c_void) -> U8;
}

#[cfg(all(config_mi_with_mbssid_cam, config_mbssid_cam))]
fn net_type_decision(adapter: *mut c_void, net_type: U8) -> U8 {
    let hw_port = unsafe { rtw_rust_hal_com_msr_hw_port(adapter) };
    if hw_port == HW_PORT0 && unsafe { rtw_get_mbid_cam_entry_num(adapter) } != 0
        && net_type != _HW_STATE_NOLINK_
    {
        return _HW_STATE_AP_;
    }
    net_type
}

#[cfg(not(all(config_mi_with_mbssid_cam, config_mbssid_cam)))]
fn net_type_decision(_adapter: *mut c_void, net_type: U8) -> U8 {
    net_type
}

#[cfg(any(config_rtw_halmac, host_hal_com_msr_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_get_msr(adapter: *mut c_void, net_type: *mut U8) {
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let hw_port = unsafe { rtw_rust_hal_com_msr_hw_port(adapter) };
    unsafe { rtw_halmac_get_network_type(dvobj, hw_port as i32, net_type) };
}

#[cfg(any(config_rtw_halmac, host_hal_com_msr_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_msr(adapter: *mut c_void, mut net_type: U8) {
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let hw_port = unsafe { rtw_rust_hal_com_msr_hw_port(adapter) };
    net_type = net_type_decision(adapter, net_type);
    // SAFETY: HALMAC set matches hal_com_rest.c under RTW_HALMAC.
    unsafe { rtw_halmac_set_network_type(dvobj, hw_port as i32, net_type) };
}