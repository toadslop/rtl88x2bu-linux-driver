// SPDX-License-Identifier: GPL-2.0
//! W3-38 AID + pre-link sta helpers — Rust port of `core/rtw_sta_mgt_rest.c`.

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
use std::os::raw::{c_int, c_uint};

#[cfg(not(host_sta_mgt_test))]
use core::ffi::{c_int, c_uint};

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const ETH_ALEN: usize = 6;
const RTW_PRE_LINK_STA_NUM: usize = 8;
const WIFI_FW_PRE_LINK: c_uint = 0x0000_0800;

#[cfg(host_sta_mgt_test)]
const NUM_ACL: usize = 16;
#[cfg(host_sta_mgt_test)]
const RTW_ACL_PERIOD_NUM: usize = 2;

#[cfg(host_sta_mgt_test)]
mod host_layout {
    use super::*;

    #[repr(C)]
    pub struct List {
        pub next: *mut List,
        pub prev: *mut List,
    }

    #[repr(C)]
    pub struct Queue {
        pub queue: List,
        pub lock: c_int,
    }

    #[repr(C)]
    pub struct RtwWlanAclNode {
        pub list: List,
        pub addr: [u8; ETH_ALEN],
        pub valid: u8,
    }

    #[repr(C)]
    pub struct WlanAclPool {
        pub mode: c_int,
        pub num: c_int,
        pub aclnode: [RtwWlanAclNode; NUM_ACL],
        pub acl_node_q: Queue,
    }

    #[repr(C)]
    pub struct PreLinkStaNode {
        pub valid: u8,
        pub addr: [u8; ETH_ALEN],
    }

    #[repr(C)]
    pub struct PreLinkStaCtl {
        pub lock: c_int,
        pub num: u8,
        pub node: [PreLinkStaNode; RTW_PRE_LINK_STA_NUM],
    }

    #[repr(C)]
    pub struct CmnStaInfo {
        pub aid: u16,
        pub mac_addr: [u8; ETH_ALEN],
    }

    #[repr(C)]
    pub struct StaInfo {
        pub cmn: CmnStaInfo,
        pub state: c_uint,
    }

    #[repr(C)]
    pub struct StaPriv {
        pub acl_list: [WlanAclPool; RTW_ACL_PERIOD_NUM],
        pub padapter: *mut Adapter,
        pub sta_aid: *mut *mut StaInfo,
        pub max_aid: u16,
        pub started_aid: u16,
        pub rr_aid: u8,
        pub max_num_sta: u16,
        pub pre_link_sta_ctl: PreLinkStaCtl,
    }

    #[repr(C)]
    pub struct Adapter {
        pub stapriv: StaPriv,
    }
}

#[cfg(host_sta_mgt_test)]
use host_layout::{Adapter, StaInfo, StaPriv};

#[cfg(not(host_sta_mgt_test))]
pub type Adapter = core::ffi::c_void;
#[cfg(not(host_sta_mgt_test))]
pub type StaInfo = core::ffi::c_void;
#[cfg(not(host_sta_mgt_test))]
pub type StaPriv = core::ffi::c_void;

#[cfg(not(host_sta_mgt_test))]
mod kernel_layout {
    extern "C" {
        pub fn rtw_rust_sta_stapriv(adapter: *mut u8) -> *mut u8;
        pub fn rtw_rust_sta_adapter(stapriv: *mut u8) -> *mut u8;
        pub fn rtw_rust_sta_max_aid(stapriv: *mut u8) -> u16;
        pub fn rtw_rust_sta_max_num_sta(stapriv: *mut u8) -> u16;
        pub fn rtw_rust_sta_started_aid(stapriv: *mut u8) -> u16;
        pub fn rtw_rust_sta_rr_aid(stapriv: *mut u8) -> u8;
        pub fn rtw_rust_sta_aid_entry(stapriv: *mut u8, aid: u16) -> *mut u8;
        pub fn rtw_rust_sta_aid_entry_set(stapriv: *mut u8, aid: u16, sta: *mut u8);
        pub fn rtw_rust_sta_started_aid_set(stapriv: *mut u8, aid: u16);
        pub fn rtw_rust_sta_cmn_aid_set(sta: *mut u8, aid: u16);
        pub fn rtw_rust_sta_state(sta: *mut u8) -> u32;
        pub fn rtw_rust_pre_link_find(stapriv: *mut u8, addr: *mut u8) -> u8;
        pub fn rtw_rust_pre_link_remove(stapriv: *mut u8, addr: *mut u8) -> u8;
        pub fn rtw_rust_pre_link_drain(stapriv: *mut u8, addrs: *mut u8, max: u8) -> u8;
        pub fn rtw_rust_pre_link_ctl_init(stapriv: *mut u8);
        pub fn rtw_rust_pre_link_ctl_lock_free(stapriv: *mut u8);
        pub fn rtw_check_invalid_mac_address(mac: *const u8, check_local_bit: u8) -> u8;
        pub fn rtw_get_stainfo(stapriv: *mut u8, hwaddr: *const u8) -> *mut u8;
        pub fn rtw_free_stainfo(padapter: *mut u8, psta: *mut u8);
    }
}

#[cfg(host_sta_mgt_test)]
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

fn rtw_aid_alloc_inner(adapter: *mut Adapter, sta: *mut StaInfo) -> u16 {
    if adapter.is_null() || sta.is_null() {
        return 0;
    }
    #[cfg(host_sta_mgt_test)]
    {
        let adapter = unsafe { &mut *adapter };
        let stapriv = &mut adapter.stapriv;
        let sta = unsafe { &mut *sta };
        let mut aid = 0u16;
        let mut used_cnt = 0u16;
        let mut i = 0u16;
        while i < stapriv.max_aid {
            aid = ((i + stapriv.started_aid - 1) % stapriv.max_aid) + 1;
            if unsafe { *stapriv.sta_aid.add((aid - 1) as usize) }.is_null() {
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
            unsafe { *stapriv.sta_aid.add((aid - 1) as usize) = sta };
            if stapriv.rr_aid != 0 {
                stapriv.started_aid = (aid % stapriv.max_aid) + 1;
            }
        }
        return aid;
    }
    #[cfg(not(host_sta_mgt_test))]
    {
        let stapriv = unsafe { kernel_layout::rtw_rust_sta_stapriv(adapter.cast()) };
        let max_aid = unsafe { kernel_layout::rtw_rust_sta_max_aid(stapriv) };
        let max_num_sta = unsafe { kernel_layout::rtw_rust_sta_max_num_sta(stapriv) };
        let mut started_aid = unsafe { kernel_layout::rtw_rust_sta_started_aid(stapriv) };
        let rr_aid = unsafe { kernel_layout::rtw_rust_sta_rr_aid(stapriv) };
        let sta_ptr = sta.cast();
        let mut aid = 0u16;
        let mut used_cnt = 0u16;
        let mut i = 0u16;
        while i < max_aid {
            aid = ((i + started_aid - 1) % max_aid) + 1;
            if unsafe { kernel_layout::rtw_rust_sta_aid_entry(stapriv, aid) }.is_null() {
                break;
            }
            used_cnt += 1;
            if used_cnt >= max_num_sta {
                break;
            }
            i += 1;
        }
        if i >= max_aid || used_cnt >= max_num_sta {
            aid = 0;
        }
        unsafe { kernel_layout::rtw_rust_sta_cmn_aid_set(sta_ptr, aid) };
        if aid != 0 {
            unsafe { kernel_layout::rtw_rust_sta_aid_entry_set(stapriv, aid, sta_ptr) };
            if rr_aid != 0 {
                started_aid = (aid % max_aid) + 1;
                unsafe { kernel_layout::rtw_rust_sta_started_aid_set(stapriv, started_aid) };
            }
        }
        aid
    }
}

#[no_mangle]
pub extern "C" fn rtw_aid_alloc(adapter: *mut Adapter, sta: *mut StaInfo) -> u16 {
    rtw_aid_alloc_inner(adapter, sta)
}

#[no_mangle]
pub extern "C" fn rtw_is_pre_link_sta(stapriv: *mut StaPriv, addr: *mut u8) -> bool {
    if stapriv.is_null() || addr.is_null() {
        return false;
    }
    #[cfg(host_sta_mgt_test)]
    {
        for node in &unsafe { &*stapriv }.pre_link_sta_ctl.node {
            if node.valid == _TRUE as u8 && rtw_memcmp(node.addr.as_ptr(), addr, ETH_ALEN) == _TRUE
            {
                return true;
            }
        }
        return false;
    }
    #[cfg(not(host_sta_mgt_test))]
    return unsafe { kernel_layout::rtw_rust_pre_link_find(stapriv.cast(), addr) != 0 };
}

#[no_mangle]
pub extern "C" fn rtw_pre_link_sta_del(stapriv: *mut StaPriv, hwaddr: *mut u8) {
    if stapriv.is_null() || hwaddr.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    {
        if unsafe { rtw_check_invalid_mac_address(hwaddr, _FALSE as u8) } == _TRUE as u8 {
            return;
        }
        let stapriv = unsafe { &mut *stapriv };
        let mut exist = false;
        for node in &mut stapriv.pre_link_sta_ctl.node {
            if node.valid == _TRUE as u8
                && rtw_memcmp(node.addr.as_ptr(), hwaddr, ETH_ALEN) == _TRUE
            {
                node.valid = _FALSE as u8;
                stapriv.pre_link_sta_ctl.num = stapriv.pre_link_sta_ctl.num.saturating_sub(1);
                exist = true;
                break;
            }
        }
        if !exist {
            return;
        }
        let sta = unsafe { rtw_get_stainfo(stapriv, hwaddr) };
        if !sta.is_null() && unsafe { (*sta).state } == WIFI_FW_PRE_LINK {
            unsafe { rtw_free_stainfo(stapriv.padapter, sta) };
        }
        return;
    }
    #[cfg(not(host_sta_mgt_test))]
    {
        let sp = stapriv.cast();
        if unsafe { kernel_layout::rtw_check_invalid_mac_address(hwaddr, _FALSE as u8) }
            == _TRUE as u8
            || unsafe { kernel_layout::rtw_rust_pre_link_remove(sp, hwaddr) } == 0
        {
            return;
        }
        let padapter = unsafe { kernel_layout::rtw_rust_sta_adapter(sp) };
        let sta = unsafe { kernel_layout::rtw_get_stainfo(sp, hwaddr) };
        if !sta.is_null() && unsafe { kernel_layout::rtw_rust_sta_state(sta) } == WIFI_FW_PRE_LINK {
            unsafe { kernel_layout::rtw_free_stainfo(padapter, sta) };
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_pre_link_sta_ctl_reset(stapriv: *mut StaPriv) {
    if stapriv.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    {
        let stapriv = unsafe { &mut *stapriv };
        let mut addrs = [[0u8; ETH_ALEN]; RTW_PRE_LINK_STA_NUM];
        let mut j = 0usize;
        for node in &mut stapriv.pre_link_sta_ctl.node {
            if node.valid == _FALSE as u8 {
                continue;
            }
            addrs[j] = node.addr;
            node.valid = _FALSE as u8;
            stapriv.pre_link_sta_ctl.num = stapriv.pre_link_sta_ctl.num.saturating_sub(1);
            j += 1;
        }
        for i in 0..j {
            let sta = unsafe { rtw_get_stainfo(stapriv, addrs[i].as_ptr()) };
            if !sta.is_null() && unsafe { (*sta).state } == WIFI_FW_PRE_LINK {
                unsafe { rtw_free_stainfo(stapriv.padapter, sta) };
            }
        }
        return;
    }
    #[cfg(not(host_sta_mgt_test))]
    {
        let sp = stapriv.cast();
        let mut addrs = [0u8; RTW_PRE_LINK_STA_NUM * ETH_ALEN];
        let n = unsafe {
            kernel_layout::rtw_rust_pre_link_drain(
                sp,
                addrs.as_mut_ptr(),
                RTW_PRE_LINK_STA_NUM as u8,
            )
        };
        let padapter = unsafe { kernel_layout::rtw_rust_sta_adapter(sp) };
        for i in 0..n as usize {
            let mac = unsafe { addrs.as_ptr().add(i * ETH_ALEN) };
            let sta = unsafe { kernel_layout::rtw_get_stainfo(sp, mac) };
            if !sta.is_null()
                && unsafe { kernel_layout::rtw_rust_sta_state(sta) } == WIFI_FW_PRE_LINK
            {
                unsafe { kernel_layout::rtw_free_stainfo(padapter, sta) };
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_pre_link_sta_ctl_init(stapriv: *mut StaPriv) {
    if stapriv.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    {
        let ctl = &mut unsafe { &mut *stapriv }.pre_link_sta_ctl;
        ctl.num = 0;
        for node in &mut ctl.node {
            node.valid = _FALSE as u8;
        }
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        kernel_layout::rtw_rust_pre_link_ctl_init(stapriv.cast());
    }
}

#[no_mangle]
pub extern "C" fn rtw_pre_link_sta_ctl_deinit(stapriv: *mut StaPriv) {
    if stapriv.is_null() {
        return;
    }
    rtw_pre_link_sta_ctl_reset(stapriv);
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        kernel_layout::rtw_rust_pre_link_ctl_lock_free(stapriv.cast());
    }
}
