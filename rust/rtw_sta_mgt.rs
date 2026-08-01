// SPDX-License-Identifier: GPL-2.0
//! Sta mgmt match rule + ACL helpers — Rust port of `core/rtw_sta_mgt_rest.c` (W3-37).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_sta_mgt_test)]
use std::os::raw::c_int;

#[cfg(not(host_sta_mgt_test))]
use core::ffi::c_int;

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const ETH_ALEN: usize = 6;
const NUM_ACL: usize = 16;
const RTW_ACL_PERIOD_NUM: usize = 2;
const RTW_ACL_MODE_ACCEPT_UNLESS_LISTED: c_int = 1;
const RTW_ACL_MODE_DENY_UNLESS_LISTED: c_int = 2;

#[cfg(host_sta_mgt_test)]
#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}

#[cfg(host_sta_mgt_test)]
#[repr(C)]
struct Queue {
    queue: List,
    lock: c_int,
}

#[cfg(host_sta_mgt_test)]
#[repr(C)]
struct RtwWlanAclNode {
    list: List,
    addr: [u8; ETH_ALEN],
    valid: u8,
}

#[cfg(host_sta_mgt_test)]
#[repr(C)]
struct WlanAclPool {
    mode: c_int,
    num: c_int,
    aclnode: [RtwWlanAclNode; NUM_ACL],
    acl_node_q: Queue,
}

#[cfg(host_sta_mgt_test)]
#[repr(C)]
struct StaPriv {
    acl_list: [WlanAclPool; RTW_ACL_PERIOD_NUM],
}

#[cfg(host_sta_mgt_test)]
#[repr(C)]
pub struct Adapter {
    stapriv: StaPriv,
}

#[cfg(not(host_sta_mgt_test))]
pub type Adapter = core::ffi::c_void;

#[cfg(not(host_sta_mgt_test))]
mod kernel_layout {
    use core::ffi::c_int;

    extern "C" {
        pub fn rtw_rust_sta_acl_pool(adapter: *mut u8, period: u8) -> *mut u8;
        pub fn rtw_rust_sta_acl_mode(acl: *mut u8) -> c_int;
        pub fn rtw_rust_sta_acl_mac_listed(acl: *mut u8, mac_addr: *const u8) -> u8;
        pub fn rtw_rust_sta_warn_on(condition: c_int);
    }
}

fn port_is_5001(port: *mut u8) -> bool {
    if port.is_null() {
        return false;
    }
    let b = unsafe { core::slice::from_raw_parts(port, 2) };
    u16::from_be_bytes([b[0], b[1]]) == 5001
}

#[cfg(host_sta_mgt_test)]
fn rtw_memcmp(dst: *const u8, src: *const u8, sz: usize) -> c_int {
    let a = unsafe { core::slice::from_raw_parts(dst, sz) };
    let b = unsafe { core::slice::from_raw_parts(src, sz) };
    if a == b {
        _TRUE
    } else {
        _FALSE
    }
}

#[cfg(host_sta_mgt_test)]
fn access_ctrl_period(adapter: *mut Adapter, period: u8, mac_addr: *const u8) -> u8 {
    if adapter.is_null() || mac_addr.is_null() {
        return _TRUE as u8;
    }
    let adapter = unsafe { &mut *adapter };
    if period as usize >= RTW_ACL_PERIOD_NUM {
        return _TRUE as u8;
    }
    let acl = &mut adapter.stapriv.acl_list[period as usize];
    if acl.mode != RTW_ACL_MODE_ACCEPT_UNLESS_LISTED
        && acl.mode != RTW_ACL_MODE_DENY_UNLESS_LISTED
    {
        return _TRUE as u8;
    }

    let head = &mut acl.acl_node_q.queue as *mut List;
    let mut list = unsafe { (*head).next };
    let mut matched = false;
    while list != head {
        let acl_node = unsafe { &*(list as *const RtwWlanAclNode) };
        list = unsafe { (*list).next };
        if rtw_memcmp(acl_node.addr.as_ptr(), mac_addr, ETH_ALEN) == _TRUE
            && acl_node.valid == _TRUE as u8
        {
            matched = true;
            break;
        }
    }

    if acl.mode == RTW_ACL_MODE_ACCEPT_UNLESS_LISTED {
        if matched {
            _FALSE as u8
        } else {
            _TRUE as u8
        }
    } else if matched {
        _TRUE as u8
    } else {
        _FALSE as u8
    }
}

#[cfg(not(host_sta_mgt_test))]
fn access_ctrl_period(adapter: *mut Adapter, period: u8, mac_addr: *const u8) -> u8 {
    if adapter.is_null() || mac_addr.is_null() {
        return _TRUE as u8;
    }
    if period as usize >= RTW_ACL_PERIOD_NUM {
        unsafe {
            kernel_layout::rtw_rust_sta_warn_on(1);
        }
        return _TRUE as u8;
    }
    let acl = unsafe { kernel_layout::rtw_rust_sta_acl_pool(adapter.cast(), period) };
    if acl.is_null() {
        return _TRUE as u8;
    }
    let mode = unsafe { kernel_layout::rtw_rust_sta_acl_mode(acl) };
    if mode != RTW_ACL_MODE_ACCEPT_UNLESS_LISTED && mode != RTW_ACL_MODE_DENY_UNLESS_LISTED {
        return _TRUE as u8;
    }
    let matched = unsafe { kernel_layout::rtw_rust_sta_acl_mac_listed(acl, mac_addr) } != 0;
    if mode == RTW_ACL_MODE_ACCEPT_UNLESS_LISTED {
        if matched {
            _FALSE as u8
        } else {
            _TRUE as u8
        }
    } else if matched {
        _TRUE as u8
    } else {
        _FALSE as u8
    }
}

#[no_mangle]
pub extern "C" fn test_st_match_rule(
    _adapter: *mut Adapter,
    _local_naddr: *mut u8,
    local_port: *mut u8,
    _remote_naddr: *mut u8,
    remote_port: *mut u8,
) -> bool {
    port_is_5001(local_port) || port_is_5001(remote_port)
}

#[no_mangle]
pub extern "C" fn _rtw_access_ctrl(
    adapter: *mut Adapter,
    period: u8,
    mac_addr: *const u8,
) -> u8 {
    access_ctrl_period(adapter, period, mac_addr)
}

#[no_mangle]
pub extern "C" fn rtw_access_ctrl(adapter: *mut Adapter, mac_addr: *const u8) -> u8 {
    for period in 0..RTW_ACL_PERIOD_NUM {
        if access_ctrl_period(adapter, period as u8, mac_addr) == _FALSE as u8 {
            return _FALSE as u8;
        }
    }
    _TRUE as u8
}
