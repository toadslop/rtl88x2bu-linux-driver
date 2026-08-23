// SPDX-License-Identifier: GPL-2.0
//! W3-55 AP TIM/VAPID helpers — Rust port of `core/rtw_ap_rest.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

const _SUCCESS: u8 = 1;
const _FAIL: u8 = 0;
const BIT0: u8 = 1;
const WLAN_EID_TIM: u8 = 5;

#[cfg(host_ap_rest_test)]
const CONFIG_LIMITED_AP_NUM: usize = 4;

#[cfg(host_ap_rest_test)]
#[repr(C)]
pub struct DvobjPriv {
    pub vap_map: u8,
}

#[cfg(not(host_ap_rest_test))]
pub type DvobjPriv = core::ffi::c_void;

#[inline]
fn bmp_is_set(bmp: &[u8], id: u8) -> bool {
    let idx = (id / 8) as usize;
    idx < bmp.len() && (bmp[idx] & (1u8 << (id % 8))) != 0
}

#[inline]
fn bmp_not_empty(bmp: &[u8]) -> bool {
    bmp.iter().any(|&b| b != 0)
}

#[cfg(all(not(host_ap_rest_test), fw_handle_txbcn))]
mod kernel {
    use core::ffi::c_int;

    use super::DvobjPriv;

    extern "C" {
        pub fn rtw_rust_ap_limited_ap_num() -> u8;
        pub fn rtw_rust_ap_vapid_fail_log(vap_id: u8);
        pub fn rtw_rust_ap_warn_on(cond: c_int);
        pub fn rtw_rust_ap_get_vap_map(dvobj: *mut DvobjPriv) -> u8;
        pub fn rtw_rust_ap_set_vap_map(dvobj: *mut DvobjPriv, vap_map: u8);
    }
}

#[no_mangle]
pub extern "C" fn rtw_set_tim_ie(
    dtim_cnt: u8,
    dtim_period: u8,
    tim_bmp: *const u8,
    tim_bmp_len: u8,
    tim_ie: *mut u8,
) -> u8 {
    if tim_ie.is_null() {
        return 0;
    }
    let bmp = if tim_bmp.is_null() || tim_bmp_len == 0 {
        &[]
    } else {
        unsafe { core::slice::from_raw_parts(tim_bmp, tim_bmp_len as usize) }
    };
    let (n1, _n2, bmp_len) = if bmp_not_empty(bmp) {
        let mut first = 0usize;
        while first < bmp.len() && bmp[first] == 0 {
            first += 1;
        }
        let n1 = (first & 0xFE) as u8;
        let mut last = bmp.len().saturating_sub(1);
        while last > 0 && bmp[last] == 0 {
            last -= 1;
        }
        let n2 = last as u8;
        (n1, n2, n2 - n1 + 1)
    } else {
        (0u8, 0u8, 1u8)
    };
    unsafe {
        let mut p = tim_ie;
        *p = WLAN_EID_TIM;
        p = p.add(1);
        *p = 2 + 1 + bmp_len;
        p = p.add(1);
        *p = dtim_cnt;
        p = p.add(1);
        *p = dtim_period;
        p = p.add(1);
        *p = (if bmp_is_set(bmp, 0) { BIT0 } else { 0 }) | n1;
        p = p.add(1);
        if !bmp.is_empty() && bmp_len > 0 {
            let start = n1 as usize;
            let end = start + bmp_len as usize;
            if end <= bmp.len() {
                core::ptr::copy_nonoverlapping(bmp.as_ptr().add(start), p, bmp_len as usize);
            }
        }
    }
    2 + 2 + 1 + bmp_len
}

#[cfg(any(host_ap_rest_test, fw_handle_txbcn))]
#[no_mangle]
pub extern "C" fn rtw_ap_allocate_vapid(dvobj: *mut DvobjPriv) -> u8 {
    if dvobj.is_null() {
        return 0;
    }
    #[cfg(host_ap_rest_test)]
    let limited = CONFIG_LIMITED_AP_NUM;
    #[cfg(not(host_ap_rest_test))]
    let limited = unsafe { kernel::rtw_rust_ap_limited_ap_num() as usize };
    #[cfg(host_ap_rest_test)]
    unsafe {
        let dvobj = &mut *dvobj;
        for vap_id in 0..limited {
            if (dvobj.vap_map & (1u8 << vap_id)) == 0 {
                dvobj.vap_map |= 1u8 << vap_id;
                return vap_id as u8;
            }
        }
        vap_id_out_of_range(limited)
    }
    #[cfg(not(host_ap_rest_test))]
    unsafe {
        let mut vap_map = kernel::rtw_rust_ap_get_vap_map(dvobj);
        for vap_id in 0..limited {
            if (vap_map & (1u8 << vap_id)) == 0 {
                vap_map |= 1u8 << vap_id;
                kernel::rtw_rust_ap_set_vap_map(dvobj, vap_map);
                return vap_id as u8;
            }
        }
        vap_id_out_of_range(limited)
    }
}

#[cfg(any(host_ap_rest_test, fw_handle_txbcn))]
#[no_mangle]
pub extern "C" fn rtw_ap_release_vapid(dvobj: *mut DvobjPriv, vap_id: u8) -> u8 {
    if dvobj.is_null() {
        return _FAIL;
    }
    #[cfg(host_ap_rest_test)]
    let limited = CONFIG_LIMITED_AP_NUM;
    #[cfg(not(host_ap_rest_test))]
    let limited = unsafe { kernel::rtw_rust_ap_limited_ap_num() as usize };
    if vap_id as usize >= limited {
        #[cfg(host_ap_rest_test)]
        {
            let _ = vap_id;
        }
        #[cfg(not(host_ap_rest_test))]
        unsafe {
            kernel::rtw_rust_ap_vapid_fail_log(vap_id);
            kernel::rtw_rust_ap_warn_on(1);
        }
        return _FAIL;
    }
    #[cfg(host_ap_rest_test)]
    unsafe {
        (*dvobj).vap_map &= !(1u8 << vap_id);
    }
    #[cfg(not(host_ap_rest_test))]
    unsafe {
        let vap_map = kernel::rtw_rust_ap_get_vap_map(dvobj);
        kernel::rtw_rust_ap_set_vap_map(dvobj, vap_map & !(1u8 << vap_id));
    }
    _SUCCESS
}

#[cfg(any(host_ap_rest_test, fw_handle_txbcn))]
#[inline]
fn vap_id_out_of_range(limited: usize) -> u8 {
    limited as u8
}
