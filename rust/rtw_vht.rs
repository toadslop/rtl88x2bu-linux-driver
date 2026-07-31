// SPDX-License-Identifier: GPL-2.0
//! VHT MCS/NSS helpers — Rust port of `core/rtw_vht_rest.c` (W3-35).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_vht_test)]
use std::os::raw::c_int;

#[cfg(not(host_vht_test))]
use core::ffi::c_int;

#[no_mangle]
pub extern "C" fn rtw_vht_nss_to_mcsmap(nss: u8, target_mcs_map: *mut u8, cur_mcs_map: *mut u8) {
    let cur = unsafe { core::slice::from_raw_parts(cur_mcs_map, 2) };
    let target = unsafe { core::slice::from_raw_parts_mut(target_mcs_map, 2) };
    for i in 0..2 {
        target[i] = 0;
        for j in (0..8).step_by(2) {
            let cur_rate = (cur[i] >> j) & 3;
            let target_rate = if cur_rate == 3 {
                3
            } else if nss <= (j / 2) as u8 + (i as u8) * 4 {
                3
            } else {
                cur_rate
            };
            target[i] |= target_rate << j;
        }
    }
}

#[cfg(host_vht_test)]
#[no_mangle]
pub extern "C" fn VHT_get_ss_from_map(vht_mcs_map: *mut u8) -> u8 {
    let map = unsafe { core::slice::from_raw_parts(vht_mcs_map, 2) };
    let mut ss = 0u8;
    for i in 0..2 {
        if map[i] == 0xff {
            continue;
        }
        for j in (0..8).step_by(2) {
            if ((map[i] >> j) & 0x03) == 0x03 {
                break;
            }
            ss = ss.saturating_add(1);
        }
    }
    ss
}

#[no_mangle]
pub extern "C" fn rtw_rust_vht_probe() -> c_int {
    0x1e35
}
