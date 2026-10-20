// SPDX-License-Identifier: GPL-2.0
// RTW_HALMAC hw port enable/disable + restore (W4-49).

use core::ffi::c_void;

type U8 = u8;

const TRUE: U8 = 1;
const FALSE: U8 = 0;

extern "C" {
    fn rtw_rust_hal_com_hw_var_port_cfg() -> i32;
    fn rtw_rust_hal_com_set_hwreg(adapter: *mut c_void, variable: i32, val: *mut U8);
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_dvobj_iface_nums(dvobj: *mut c_void) -> U8;
    fn rtw_rust_hal_com_dvobj_adapter_at(dvobj: *mut c_void, idx: i32) -> *mut c_void;
}

fn hw_port_set_enable(adapter: *mut c_void, enable: U8) {
    if adapter.is_null() {
        return;
    }
    let mut val = enable;
    unsafe {
        rtw_rust_hal_com_set_hwreg(
            adapter,
            rtw_rust_hal_com_hw_var_port_cfg(),
            &mut val,
        );
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_hw_port_enable(adapter: *mut c_void) {
    hw_port_set_enable(adapter, TRUE);
}

#[no_mangle]
pub extern "C" fn rtw_hal_hw_port_disable(adapter: *mut c_void) {
    hw_port_set_enable(adapter, FALSE);
}

#[no_mangle]
pub extern "C" fn rtw_restore_hw_port_cfg(adapter: *mut c_void) {
    #[cfg(config_mi_with_mbssid_cam)]
    {
        return;
    }
    #[cfg(not(config_mi_with_mbssid_cam))]
    {
        if adapter.is_null() {
            return;
        }
        let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
        if dvobj.is_null() {
            return;
        }
        let n = unsafe { rtw_rust_hal_com_dvobj_iface_nums(dvobj) };
        for i in 0..n {
            let iface = unsafe { rtw_rust_hal_com_dvobj_adapter_at(dvobj, i as i32) };
            if !iface.is_null() {
                rtw_hal_hw_port_enable(iface);
            }
        }
    }
}
