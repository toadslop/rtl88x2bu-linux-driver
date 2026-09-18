// SPDX-License-Identifier: GPL-2.0
//! W3-77 stainfo init + hash lookup — Rust port of `core/rtw_sta_mgt_lookup.c`.

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
use core::ffi::{c_int, c_ulong};

const _FALSE: c_int = 0;
const ETH_ALEN: usize = 6;

#[cfg(host_sta_mgt_test)]
const NUM_STA: usize = 4;
#[cfg(host_sta_mgt_test)]
const SESSION_TRACKER_REG_ID_NUM: usize = 1;
#[cfg(host_sta_mgt_test)]
const STA_PRIV_PSTAINFO_BUF: usize = 928;
#[cfg(host_sta_mgt_test)]
const STA_PRIV_STA_HASH: usize = 1016;

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
    pub struct CmnStaInfo {
        pub aid: u16,
        pub mac_addr: [u8; ETH_ALEN],
    }

    #[repr(C)]
    pub struct TxServq {
        pub sta_pending: Queue,
        pub tx_pending: List,
        pub qcnt: c_int,
    }

    #[repr(C)]
    pub struct StaXmitPriv {
        pub lock: c_int,
        pub be_q: TxServq,
        pub bk_q: TxServq,
        pub vi_q: TxServq,
        pub vo_q: TxServq,
        pub mgmt_q: TxServq,
    }

    #[repr(C)]
    pub struct StaRecvPriv {
        pub lock: c_int,
        pub defrag_q: Queue,
    }

    #[repr(C)]
    pub struct StRegister {
        pub s_proto: u8,
        pub rule: Option<extern "C" fn(*mut u8, *mut u8, *mut u8, *mut u8, *mut u8) -> bool>,
    }

    #[repr(C)]
    pub struct StCtl {
        pub reg: [StRegister; SESSION_TRACKER_REG_ID_NUM],
        pub tracker_q: Queue,
    }

    #[repr(C)]
    pub struct RecvReorderCtrl {
        pub reordering_ctrl_timer: c_int,
        pub pending_recvframe_queue: Queue,
    }

    #[repr(C)]
    pub struct StaPriv {
        _leading: [u8; STA_PRIV_PSTAINFO_BUF],
        pub pstainfo_buf: *mut u8,
        _mid: [u8; STA_PRIV_STA_HASH - STA_PRIV_PSTAINFO_BUF - core::mem::size_of::<*mut u8>()],
        pub sta_hash: [List; NUM_STA],
    }

    #[repr(C)]
    pub struct StaInfo {
        pub cmn: CmnStaInfo,
        pub state: c_uint,
        pub lock: c_int,
        pub list: List,
        pub hash_list: List,
        pub padapter: *mut u8,
        pub sleep_q: Queue,
        pub mgmt_sleep_q: Queue,
        pub sta_xmitpriv: StaXmitPriv,
        pub sta_recvpriv: StaRecvPriv,
        pub asoc_list: List,
        pub auth_list: List,
        pub bpairwise_key_installed: u8,
        pub st_ctl: StCtl,
        pub recvreorder_ctrl: [RecvReorderCtrl; 16],
    }
}

#[cfg(host_sta_mgt_test)]
use host_layout::{List, Queue, StaInfo, StaPriv};

#[cfg(host_sta_mgt_test)]
const _STA_INFO_SIZE: usize = core::mem::size_of::<StaInfo>();
#[cfg(host_sta_mgt_test)]
const _STA_INFO_SIZE_OK: () = assert!(_STA_INFO_SIZE == 976);
#[cfg(host_sta_mgt_test)]
const _STAPRIV_PBUF_OFF: () =
    assert!(core::mem::offset_of!(StaPriv, pstainfo_buf) == STA_PRIV_PSTAINFO_BUF);
#[cfg(host_sta_mgt_test)]
const _STAPRIV_HASH_OFF: () =
    assert!(core::mem::offset_of!(StaPriv, sta_hash) == STA_PRIV_STA_HASH);

#[cfg(not(host_sta_mgt_test))]
pub type StaInfo = core::ffi::c_void;
#[cfg(not(host_sta_mgt_test))]
pub type StaPriv = core::ffi::c_void;

#[cfg(not(host_sta_mgt_test))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_lookup_init_stainfo_fields(psta: *mut StaInfo);
        pub fn rtw_rust_sta_info_size() -> u32;
        pub fn rtw_rust_stainfo_buf(stapriv: *mut StaPriv) -> *mut u8;
        pub fn rtw_rust_stainfo_offset_valid(offset: c_int) -> u8;
        pub fn rtw_rust_stainfo_offset_invalid_log(func: *const u8, offset: c_int);
        pub fn rtw_rust_lookup_enter_hash(stapriv: *mut StaPriv, irql: *mut c_ulong);
        pub fn rtw_rust_lookup_exit_hash(stapriv: *mut StaPriv, irql: *mut c_ulong);
        pub fn rtw_rust_lookup_find_sta_unlocked(
            stapriv: *mut StaPriv,
            hwaddr: *const u8,
        ) -> *mut StaInfo;
    }
}

#[cfg(host_sta_mgt_test)]
extern "C" {
    fn _rtw_init_sta_xmit_priv(xmit: *mut host_layout::StaXmitPriv);
    fn _rtw_init_sta_recv_priv(recv: *mut host_layout::StaRecvPriv);
    fn rtw_st_ctl_init(st_ctl: *mut host_layout::StCtl);
}

#[cfg(host_sta_mgt_test)]
fn init_listhead(list: *mut List) {
    unsafe {
        (*list).next = list;
        (*list).prev = list;
    }
}

#[cfg(host_sta_mgt_test)]
fn init_queue(q: *mut Queue) {
    unsafe {
        init_listhead(core::ptr::addr_of_mut!((*q).queue));
        (*q).lock = 0;
    }
}

#[cfg(host_sta_mgt_test)]
fn wifi_mac_hash(mac: &[u8; ETH_ALEN]) -> u32 {
    let mut x = mac[0] as u32;
    x = (x << 2) ^ mac[1] as u32;
    x = (x << 2) ^ mac[2] as u32;
    x = (x << 2) ^ mac[3] as u32;
    x = (x << 2) ^ mac[4] as u32;
    x = (x << 2) ^ mac[5] as u32;
    x ^= x >> 8;
    x & (NUM_STA as u32 - 1)
}

#[no_mangle]
pub extern "C" fn _rtw_init_stainfo(psta: *mut StaInfo) {
    if psta.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        let psta = &mut *psta;
        core::ptr::write_bytes(
            psta as *mut StaInfo as *mut u8,
            0,
            core::mem::size_of::<StaInfo>(),
        );
        psta.lock = 0;
        init_listhead(core::ptr::addr_of_mut!(psta.list));
        init_listhead(core::ptr::addr_of_mut!(psta.hash_list));
        init_queue(core::ptr::addr_of_mut!(psta.sleep_q));
        init_queue(core::ptr::addr_of_mut!(psta.mgmt_sleep_q));
        _rtw_init_sta_xmit_priv(core::ptr::addr_of_mut!(psta.sta_xmitpriv));
        _rtw_init_sta_recv_priv(core::ptr::addr_of_mut!(psta.sta_recvpriv));
        init_listhead(core::ptr::addr_of_mut!(psta.asoc_list));
        init_listhead(core::ptr::addr_of_mut!(psta.auth_list));
        psta.bpairwise_key_installed = _FALSE as u8;
        rtw_st_ctl_init(core::ptr::addr_of_mut!(psta.st_ctl));
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        kernel::rtw_rust_lookup_init_stainfo_fields(psta);
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_stainfo_by_offset(stapriv: *mut StaPriv, offset: c_int) -> *mut StaInfo {
    if stapriv.is_null() {
        return core::ptr::null_mut();
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        let sp = &*stapriv.cast::<StaPriv>();
        if sp.pstainfo_buf.is_null() {
            return core::ptr::null_mut();
        }
        sp.pstainfo_buf
            .offset((offset as isize) * core::mem::size_of::<StaInfo>() as isize)
            .cast()
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        if kernel::rtw_rust_stainfo_offset_valid(offset) == 0 {
            kernel::rtw_rust_stainfo_offset_invalid_log(
                b"rtw_get_stainfo_by_offset\0".as_ptr(),
                offset,
            );
        }
        let buf = kernel::rtw_rust_stainfo_buf(stapriv);
        if buf.is_null() {
            return core::ptr::null_mut();
        }
        let size = kernel::rtw_rust_sta_info_size() as usize;
        buf.add((offset as usize) * size).cast()
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_stainfo(pstapriv: *mut StaPriv, hwaddr: *const u8) -> *mut StaInfo {
    if pstapriv.is_null() || hwaddr.is_null() {
        return core::ptr::null_mut();
    }
    #[cfg(host_sta_mgt_test)]
    {
        let bc_addr: [u8; ETH_ALEN] = [0xff; ETH_ALEN];
        unsafe {
            let mac = &*hwaddr.cast::<[u8; ETH_ALEN]>();
            let addr = if (mac[0] & 0x01) != 0 { &bc_addr } else { mac };
            let index = wifi_mac_hash(addr) as usize;
            let sp = &*pstapriv.cast::<StaPriv>();
            let head = core::ptr::addr_of!(sp.sta_hash[index]) as *mut List;
            let mut plist = (*head).next;
            while plist != head {
                let psta = plist
                    .cast::<u8>()
                    .sub(core::mem::offset_of!(StaInfo, hash_list))
                    .cast::<StaInfo>();
                if (*psta).cmn.mac_addr == *addr {
                    return psta;
                }
                plist = (*plist).next;
            }
            core::ptr::null_mut()
        }
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        let mut irql: c_ulong = 0;
        kernel::rtw_rust_lookup_enter_hash(pstapriv, core::ptr::addr_of_mut!(irql));
        let found = kernel::rtw_rust_lookup_find_sta_unlocked(pstapriv, hwaddr);
        kernel::rtw_rust_lookup_exit_hash(pstapriv, core::ptr::addr_of_mut!(irql));
        found
    }
}
