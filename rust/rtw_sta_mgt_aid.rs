// SPDX-License-Identifier: GPL-2.0
//! W3-38 AID + pre-link sta helpers — host L2 Rust oracle.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use std::os::raw::{c_int, c_uint};

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const ETH_ALEN: usize = 6;
const NUM_ACL: usize = 16;
const RTW_ACL_PERIOD_NUM: usize = 2;
const RTW_PRE_LINK_STA_NUM: usize = 8;
const WIFI_FW_PRE_LINK: c_uint = 0x0000_0800;

#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}

#[repr(C)]
struct Queue {
    queue: List,
    lock: c_int,
}

#[repr(C)]
struct RtwWlanAclNode {
    list: List,
    addr: [u8; ETH_ALEN],
    valid: u8,
}

#[repr(C)]
struct WlanAclPool {
    mode: c_int,
    num: c_int,
    aclnode: [RtwWlanAclNode; NUM_ACL],
    acl_node_q: Queue,
}

#[repr(C)]
struct PreLinkStaNode {
    valid: u8,
    addr: [u8; ETH_ALEN],
}

#[repr(C)]
struct PreLinkStaCtl {
    lock: c_int,
    num: u8,
    node: [PreLinkStaNode; RTW_PRE_LINK_STA_NUM],
}

#[repr(C)]
struct CmnStaInfo {
    aid: u16,
    mac_addr: [u8; ETH_ALEN],
}

#[repr(C)]
pub struct StaInfo {
    cmn: CmnStaInfo,
    state: c_uint,
}

#[repr(C)]
pub struct StaPriv {
    acl_list: [WlanAclPool; RTW_ACL_PERIOD_NUM],
    padapter: *mut Adapter,
    sta_aid: *mut *mut StaInfo,
    max_aid: u16,
    started_aid: u16,
    rr_aid: u8,
    max_num_sta: u16,
    pre_link_sta_ctl: PreLinkStaCtl,
}

#[repr(C)]
pub struct Adapter {
    stapriv: StaPriv,
}

extern "C" {
    fn rtw_check_invalid_mac_address(mac: *const u8, check_local_bit: u8) -> u8;
    fn rtw_get_stainfo(stapriv: *mut StaPriv, hwaddr: *const u8) -> *mut StaInfo;
    fn rtw_free_stainfo(padapter: *mut Adapter, psta: *mut StaInfo);
}

fn rtw_memcmp(dst: *const u8, src: *const u8, sz: usize) -> c_int {
    let a = unsafe { core::slice::from_raw_parts(dst, sz) };
    let b = unsafe { core::slice::from_raw_parts(src, sz) };
    if a == b {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_aid_alloc(adapter: *mut Adapter, sta: *mut StaInfo) -> u16 {
    if adapter.is_null() || sta.is_null() {
        return 0;
    }
    let adapter = unsafe { &mut *adapter };
    let stapriv = &mut adapter.stapriv;
    let sta = unsafe { &mut *sta };
    let mut aid = 0u16;
    let mut used_cnt = 0u16;
    let mut i = 0u16;

    while i < stapriv.max_aid {
        aid = ((i + stapriv.started_aid - 1) % stapriv.max_aid) + 1;
        let slot = unsafe { *stapriv.sta_aid.add((aid - 1) as usize) };
        if slot.is_null() {
            break;
        }
        used_cnt += 1;
        if used_cnt >= stapriv.max_num_sta {
            break;
        }
        i += 1;
    }
    if i >= stapriv.max_aid || used_cnt >= stapriv.max_num_sta {
        aid = 0;
    }
    sta.cmn.aid = aid;
    if aid != 0 {
        unsafe {
            *stapriv.sta_aid.add((aid - 1) as usize) = sta;
        }
        if stapriv.rr_aid != 0 {
            stapriv.started_aid = (aid % stapriv.max_aid) + 1;
        }
    }
    aid
}

#[no_mangle]
pub extern "C" fn rtw_is_pre_link_sta(stapriv: *mut StaPriv, addr: *mut u8) -> bool {
    if stapriv.is_null() || addr.is_null() {
        return false;
    }
    let stapriv = unsafe { &*stapriv };
    let ctl = &stapriv.pre_link_sta_ctl;
    for node in &ctl.node {
        if node.valid == _TRUE as u8
            && rtw_memcmp(node.addr.as_ptr(), addr, ETH_ALEN) == _TRUE
        {
            return true;
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn rtw_pre_link_sta_del(stapriv: *mut StaPriv, hwaddr: *mut u8) {
    if stapriv.is_null() || hwaddr.is_null() {
        return;
    }
    if unsafe { rtw_check_invalid_mac_address(hwaddr, _FALSE as u8) } == _TRUE as u8 {
        return;
    }
    let stapriv = unsafe { &mut *stapriv };
    let ctl = &mut stapriv.pre_link_sta_ctl;
    let mut exist = false;
    for node in &mut ctl.node {
        if node.valid == _TRUE as u8
            && rtw_memcmp(node.addr.as_ptr(), hwaddr, ETH_ALEN) == _TRUE
        {
            node.valid = _FALSE as u8;
            ctl.num = ctl.num.saturating_sub(1);
            exist = true;
            break;
        }
    }
    if !exist {
        return;
    }
    let sta = unsafe { rtw_get_stainfo(stapriv, hwaddr) };
    if sta.is_null() {
        return;
    }
    let state = unsafe { (*sta).state };
    if state == WIFI_FW_PRE_LINK {
        unsafe { rtw_free_stainfo(stapriv.padapter, sta) };
    }
}

#[no_mangle]
pub extern "C" fn rtw_pre_link_sta_ctl_reset(stapriv: *mut StaPriv) {
    if stapriv.is_null() {
        return;
    }
    let stapriv = unsafe { &mut *stapriv };
    let ctl = &mut stapriv.pre_link_sta_ctl;
    let mut addrs = [[0u8; ETH_ALEN]; RTW_PRE_LINK_STA_NUM];
    let mut j = 0usize;
    for node in &mut ctl.node {
        if node.valid == _FALSE as u8 {
            continue;
        }
        addrs[j] = node.addr;
        node.valid = _FALSE as u8;
        ctl.num = ctl.num.saturating_sub(1);
        j += 1;
    }
    for i in 0..j {
        let sta = unsafe { rtw_get_stainfo(stapriv, addrs[i].as_ptr()) };
        if sta.is_null() {
            continue;
        }
        if unsafe { (*sta).state } == WIFI_FW_PRE_LINK {
            unsafe { rtw_free_stainfo(stapriv.padapter, sta) };
        }
    }
}
