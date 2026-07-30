// SPDX-License-Identifier: GPL-2.0
//! RM util pure helpers — Rust port of `core/rtw_rm_util_rest.c` (W3-33).

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
    RtOperatingClass { global_op_class: 0, len: 0, channel: [0; MAX_CH_NUM_IN_OP_CLASS] },
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
    if pch_set.is_null() {
        return 0;
    }
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
    if bssid.is_null() {
        return _FALSE;
    }
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

#[no_mangle]
pub extern "C" fn rtw_rust_rm_util_probe() -> c_int {
    0x1e33
}
