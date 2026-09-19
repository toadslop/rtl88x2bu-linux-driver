// SPDX-License-Identifier: GPL-2.0
//! W3-77 stainfo init + hash lookup — host L2 oracle (kernel swap in PR6).

#![cfg(host_sta_mgt_test)]
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

const _FALSE: c_int = 0;
const ETH_ALEN: usize = 6;
const NUM_STA: usize = 4;
const SESSION_TRACKER_REG_ID_NUM: usize = 1;
const STA_PRIV_PSTAINFO_BUF: usize = 928;
const STA_PRIV_STA_HASH: usize = 1016;

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

use host_layout::{List, Queue, StaInfo, StaPriv};

const _STA_INFO_SIZE: usize = core::mem::size_of::<StaInfo>();
const _STA_INFO_SIZE_OK: () = assert!(_STA_INFO_SIZE == 976);
const _STAPRIV_PBUF_OFF: () =
    assert!(core::mem::offset_of!(StaPriv, pstainfo_buf) == STA_PRIV_PSTAINFO_BUF);
const _STAPRIV_HASH_OFF: () =
    assert!(core::mem::offset_of!(StaPriv, sta_hash) == STA_PRIV_STA_HASH);

extern "C" {
    fn _rtw_init_sta_xmit_priv(xmit: *mut host_layout::StaXmitPriv);
    fn _rtw_init_sta_recv_priv(recv: *mut host_layout::StaRecvPriv);
    fn rtw_st_ctl_init(st_ctl: *mut host_layout::StCtl);
}

fn init_listhead(list: *mut List) {
    unsafe {
        (*list).next = list;
        (*list).prev = list;
    }
}

fn init_queue(q: *mut Queue) {
    unsafe {
        init_listhead(core::ptr::addr_of_mut!((*q).queue));
        (*q).lock = 0;
    }
}

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
}

#[no_mangle]
pub extern "C" fn rtw_get_stainfo_by_offset(stapriv: *mut StaPriv, offset: c_int) -> *mut StaInfo {
    if stapriv.is_null() {
        return core::ptr::null_mut();
    }
    unsafe {
        let sp = &*stapriv.cast::<StaPriv>();
        if sp.pstainfo_buf.is_null() {
            return core::ptr::null_mut();
        }
        sp.pstainfo_buf
            .offset((offset as isize) * core::mem::size_of::<StaInfo>() as isize)
            .cast()
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_stainfo(pstapriv: *mut StaPriv, hwaddr: *const u8) -> *mut StaInfo {
    if pstapriv.is_null() || hwaddr.is_null() {
        return core::ptr::null_mut();
    }
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
