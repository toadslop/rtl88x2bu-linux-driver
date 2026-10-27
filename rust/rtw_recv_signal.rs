// SPDX-License-Identifier: GPL-2.0
//! W3-178: recv signal-stat profile + SNAP/bridge header tables.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

type U8 = u8;

const SIGNAL_STAT_CALC_PROFILE_MAX: usize = 3;

#[no_mangle]
pub static mut signal_stat_calc_profile: [[U8; 3]; SIGNAL_STAT_CALC_PROFILE_MAX] = [
    [4, 1, 0],
    [3, 7, 0],
    [0, 10, 0],
];

#[no_mangle]
pub static mut rtw_bridge_tunnel_header: [U8; 6] = [0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8];

#[no_mangle]
pub static mut rtw_rfc1042_header: [U8; 6] = [0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00];

#[no_mangle]
pub static mut SNAP_ETH_TYPE_IPX: [U8; 2] = [0x81, 0x37];

#[no_mangle]
pub static mut SNAP_ETH_TYPE_APPLETALK_AARP: [U8; 2] = [0x80, 0xf3];

#[cfg(any(tdls, host_recv_signal_test))]
#[no_mangle]
pub static mut SNAP_ETH_TYPE_TDLS: [U8; 2] = [0x89, 0x0d];
