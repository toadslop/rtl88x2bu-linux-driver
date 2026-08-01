// SPDX-License-Identifier: GPL-2.0
//! RM util pure helpers — Rust port of `core/rtw_rm_util_rest.c` (W3-33, W3-34).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_rm_test)]
use std::os::raw::c_int;

#[cfg(not(host_rm_test))]
use core::ffi::c_int;

const _SUCCESS: c_int = 1;
const _FALSE: c_int = 0;
const MAX_CH_NUM_IN_OP_CLASS: usize = 11;

#[repr(C)]
pub struct rtw_ieee80211_channel {
    pub hw_value: u16,
    pub flags: u32,
}

#[repr(C)]
struct RtOperatingClass {
    global_op_class: i32,
    len: i32,
    channel: [u8; MAX_CH_NUM_IN_OP_CLASS],
}

static RTW_OP_CLASS_US: [RtOperatingClass; 7] = [
    RtOperatingClass {
        global_op_class: 0,
        len: 0,
        channel: [0; MAX_CH_NUM_IN_OP_CLASS],
    },
    RtOperatingClass {
        global_op_class: 115,
        len: 4,
        channel: [36, 40, 44, 48, 0, 0, 0, 0, 0, 0, 0],
    },
    RtOperatingClass {
        global_op_class: 118,
        len: 4,
        channel: [52, 56, 60, 64, 0, 0, 0, 0, 0, 0, 0],
    },
    RtOperatingClass {
        global_op_class: 124,
        len: 4,
        channel: [149, 153, 157, 161, 0, 0, 0, 0, 0, 0, 0],
    },
    RtOperatingClass {
        global_op_class: 121,
        len: 11,
        channel: [100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140],
    },
    RtOperatingClass {
        global_op_class: 125,
        len: 5,
        channel: [149, 153, 157, 161, 165, 0, 0, 0, 0, 0, 0],
    },
    RtOperatingClass {
        global_op_class: 81,
        len: 11,
        channel: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
    },
];

#[no_mangle]
pub extern "C" fn rm_get_ch_set(
    pch_set: *mut rtw_ieee80211_channel,
    op_class: u8,
    ch_num: u8,
) -> u8 {
    let pch = unsafe { core::slice::from_raw_parts_mut(pch_set, MAX_CH_NUM_IN_OP_CLASS) };
    if ch_num != 0 {
        pch[0].hw_value = u16::from(ch_num);
        return 1;
    }
    for op in &RTW_OP_CLASS_US {
        if op.global_op_class as u8 != op_class {
            continue;
        }
        for j in 0..op.len as usize {
            pch[j].hw_value = u16::from(op.channel[j]);
        }
        return op.len as u8;
    }
    0
}

#[no_mangle]
pub extern "C" fn rm_get_oper_class_via_ch(ch: u8) -> u8 {
    for op in &RTW_OP_CLASS_US {
        for j in 0..op.len as usize {
            if ch == op.channel[j] {
                return op.global_op_class as u8;
            }
        }
    }
    0
}

#[no_mangle]
pub extern "C" fn is_wildcard_bssid(bssid: *mut u8) -> c_int {
    let b = unsafe { core::slice::from_raw_parts(bssid, 6) };
    let val8 = b.iter().fold(0xffu8, |acc, &x| acc & x);
    if val8 == 0xff {
        _SUCCESS
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn translate_dbm_to_rcpi(signal_power: i8) -> u8 {
    ((i16::from(signal_power) + 110) * 2) as u8
}

#[no_mangle]
pub extern "C" fn translate_percentage_to_rcpi(signal_strength_index: u32) -> u8 {
    translate_dbm_to_rcpi((signal_strength_index as i32 - 100) as i8)
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostMlmeExtInfo {
    dialog_token: u8,
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostMlmeExtPriv {
    mlmext_info: HostMlmeExtInfo,
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostRmPriv {
    meas_token: u8,
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostAdapter {
    rmpriv: HostRmPriv,
    mlmeextpriv: HostMlmeExtPriv,
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostRmMeasReq {
    diag_token: u8,
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostCmnStaInfo {
    aid: u16,
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostStaInfo {
    cmn: HostCmnStaInfo,
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[repr(C)]
struct HostRmObj {
    rmid: u32,
    q: HostRmMeasReq,
    psta: *mut HostStaInfo,
}

#[cfg(rtw_80211k)]
mod kernel_layout {
    extern "C" {
        pub fn rtw_rust_rm_dialog_token_ptr(padapter: *mut u8) -> *mut u8;
        pub fn rtw_rust_rm_meas_token_ptr(padapter: *mut u8) -> *mut u8;
        pub fn rtw_rust_rm_obj_psta(prm: *mut u8) -> *mut u8;
        pub fn rtw_rust_rm_obj_diag_token(prm: *mut u8) -> u8;
        pub fn rtw_rust_sta_aid(psta: *mut u8) -> u16;
        pub fn rtw_rust_rm_log_err();
    }
}

#[cfg(any(host_rm_test, rtw_80211k))]
fn gen_nonzero_token(token: &mut u8) -> u8 {
    loop {
        *token = token.wrapping_add(1);
        if *token != 0 {
            break;
        }
    }
    *token
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[no_mangle]
pub extern "C" fn rm_gen_dialog_token(padapter: *mut u8) -> u8 {
    #[cfg(host_rm_test)]
    unsafe {
        let info = &mut (*padapter.cast::<HostAdapter>()).mlmeextpriv.mlmext_info;
        return gen_nonzero_token(&mut info.dialog_token);
    }
    #[cfg(not(host_rm_test))]
    #[cfg(rtw_80211k)]
    unsafe {
        let ptr = kernel_layout::rtw_rust_rm_dialog_token_ptr(padapter);
        gen_nonzero_token(&mut *ptr)
    }
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[no_mangle]
pub extern "C" fn rm_gen_meas_token(padapter: *mut u8) -> u8 {
    #[cfg(host_rm_test)]
    unsafe {
        let token = &mut (*padapter.cast::<HostAdapter>()).rmpriv.meas_token;
        return gen_nonzero_token(token);
    }
    #[cfg(not(host_rm_test))]
    #[cfg(rtw_80211k)]
    unsafe {
        let ptr = kernel_layout::rtw_rust_rm_meas_token_ptr(padapter);
        gen_nonzero_token(&mut *ptr)
    }
}

#[cfg(any(host_rm_test, rtw_80211k))]
#[no_mangle]
pub extern "C" fn rm_gen_rmid(padapter: *mut u8, prm: *mut u8, role: u8) -> u32 {
    let _ = padapter;
    #[cfg(host_rm_test)]
    unsafe {
        let prm = &*prm.cast::<HostRmObj>();
        if prm.psta.is_null() || prm.q.diag_token == 0 {
            return 0;
        }
        let aid = (*prm.psta).cmn.aid;
        return (u32::from(aid) << 16) | (u32::from(prm.q.diag_token) << 8) | u32::from(role);
    }
    #[cfg(not(host_rm_test))]
    #[cfg(rtw_80211k)]
    unsafe {
        let psta = kernel_layout::rtw_rust_rm_obj_psta(prm);
        let diag_token = kernel_layout::rtw_rust_rm_obj_diag_token(prm);
        if psta.is_null() || diag_token == 0 {
            kernel_layout::rtw_rust_rm_log_err();
            return 0;
        }
        let aid = kernel_layout::rtw_rust_sta_aid(psta);
        (u32::from(aid) << 16) | (u32::from(diag_token) << 8) | u32::from(role)
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_rm_util_probe() -> c_int {
    0x1e34
}
