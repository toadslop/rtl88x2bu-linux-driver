// SPDX-License-Identifier: GPL-2.0
//! W3-78 stainfo alloc — Rust port of `core/rtw_sta_mgt_alloc.c`.

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
use core::ffi::c_ulong;

const _SUCCESS: u32 = 1;
const ETH_ALEN: usize = 6;

#[cfg(host_sta_mgt_test)]
const NUM_STA: usize = 4;
#[cfg(host_sta_mgt_test)]
const NUM_ACL: usize = 16;
#[cfg(host_sta_mgt_test)]
const RTW_ACL_PERIOD_NUM: usize = 2;
#[cfg(host_sta_mgt_test)]
const RTW_PRE_LINK_STA_NUM: usize = 8;
#[cfg(host_sta_mgt_test)]
const SESSION_TRACKER_REG_ID_NUM: usize = 1;

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
        #[cfg(host_sta_mgt_test)]
        pub mgmt_q: TxServq,
    }

    #[repr(C)]
    pub struct RecvReorderCtrl {
        pub reordering_ctrl_timer: c_int,
        pub pending_recvframe_queue: Queue,
    }

    #[repr(C)]
    pub struct StaRecvPriv {
        pub lock: c_int,
        pub defrag_q: Queue,
    }

    #[repr(C)]
    pub struct StRegister {
        pub s_proto: u8,
        pub rule: Option<extern "C" fn(*mut Adapter, *mut u8, *mut u8, *mut u8, *mut u8) -> bool>,
    }

    #[repr(C)]
    pub struct StCtl {
        pub reg: [StRegister; SESSION_TRACKER_REG_ID_NUM],
        pub tracker_q: Queue,
    }

    #[repr(C)]
    pub struct StaInfo {
        pub cmn: CmnStaInfo,
        pub state: c_uint,
        pub lock: c_int,
        pub list: List,
        pub hash_list: List,
        pub padapter: *mut Adapter,
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
        pub pallocated_stainfo_buf: *mut u8,
        pub pstainfo_buf: *mut u8,
        pub free_sta_queue: Queue,
        pub sleep_q: Queue,
        pub wakeup_q: Queue,
        pub sta_hash_lock: c_int,
        pub sta_hash: [List; NUM_STA],
        pub asoc_sta_count: c_int,
        pub adhoc_expire_to: u8,
        pub aid_bmp_len: u16,
        pub sta_dz_bitmap: *mut u8,
        pub tim_bitmap: *mut u8,
        pub asoc_list: List,
        pub auth_list: List,
        pub asoc_list_lock: c_int,
        pub auth_list_lock: c_int,
        pub asoc_list_cnt: c_int,
        pub auth_list_cnt: c_int,
        pub auth_to: u8,
        pub assoc_to: u8,
        pub expire_to: u16,
    }

    #[repr(C)]
    pub struct MacidCtl {
        pub num: u8,
    }

    #[repr(C)]
    pub struct MlmePriv {
        pub dummy: c_int,
    }

    #[repr(C)]
    pub struct Adapter {
        pub stapriv: StaPriv,
        pub macid_ctl: MacidCtl,
        pub mlmepriv: MlmePriv,
    }
}

#[cfg(host_sta_mgt_test)]
use host_layout::{List, Queue, StaInfo, StaPriv};

#[cfg(not(host_sta_mgt_test))]
pub type StaInfo = core::ffi::c_void;
#[cfg(not(host_sta_mgt_test))]
pub type StaPriv = core::ffi::c_void;

extern "C" {
    fn _rtw_init_stainfo(psta: *mut StaInfo);
}

#[cfg(not(host_sta_mgt_test))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_alloc_lock(stapriv: *mut StaPriv, irql: *mut c_ulong);
        pub fn rtw_rust_alloc_unlock(stapriv: *mut StaPriv, irql: *mut c_ulong);
        pub fn rtw_rust_alloc_pop_free(stapriv: *mut StaPriv) -> *mut StaInfo;
        pub fn rtw_rust_alloc_hash_attach(
            psta: *mut StaInfo,
            stapriv: *mut StaPriv,
            hwaddr: *const u8,
        ) -> u32;
        pub fn rtw_rust_alloc_post_init(psta: *mut StaInfo, stapriv: *mut StaPriv);
        pub fn rtw_rust_alloc_mi_update(stapriv: *mut StaPriv);
    }
}

#[cfg(host_sta_mgt_test)]
fn queue_empty(queue: *mut Queue) -> bool {
    unsafe {
        let head = core::ptr::addr_of_mut!((*queue).queue);
        (*head).next == head
    }
}

#[cfg(host_sta_mgt_test)]
unsafe fn queue_pop_sta(free_q: *mut Queue) -> *mut StaInfo {
    let head = core::ptr::addr_of_mut!((*free_q).queue);
    let entry = (*head).next;
    if entry == head {
        return core::ptr::null_mut();
    }
    (*(*entry).next).prev = (*entry).prev;
    (*(*entry).prev).next = (*entry).next;
    (*entry).next = entry;
    (*entry).prev = entry;
    entry
        .cast::<u8>()
        .sub(core::mem::offset_of!(StaInfo, list))
        .cast::<StaInfo>()
}

#[cfg(host_sta_mgt_test)]
unsafe fn list_insert_tail(entry: *mut List, head: *mut List) {
    let prev = (*head).prev;
    (*entry).next = head;
    (*entry).prev = prev;
    (*prev).next = entry;
    (*head).prev = entry;
}

#[cfg(host_sta_mgt_test)]
fn wifi_mac_hash_host(mac: &[u8; ETH_ALEN]) -> u32 {
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
pub extern "C" fn rtw_alloc_stainfo(stapriv: *mut StaPriv, hwaddr: *const u8) -> *mut StaInfo {
    if stapriv.is_null() || hwaddr.is_null() {
        return core::ptr::null_mut();
    }

    #[cfg(host_sta_mgt_test)]
    unsafe {
        let sp = &mut *stapriv;
        if queue_empty(core::ptr::addr_of_mut!(sp.free_sta_queue)) {
            return core::ptr::null_mut();
        }
        let psta = queue_pop_sta(core::ptr::addr_of_mut!(sp.free_sta_queue));
        if psta.is_null() {
            return core::ptr::null_mut();
        }
        _rtw_init_stainfo(psta);
        (*psta).padapter = sp.padapter;
        core::ptr::copy_nonoverlapping(hwaddr, (*psta).cmn.mac_addr.as_mut_ptr(), ETH_ALEN);
        let index = wifi_mac_hash_host(&*hwaddr.cast::<[u8; ETH_ALEN]>());
        if index as usize >= NUM_STA {
            return core::ptr::null_mut();
        }
        list_insert_tail(
            core::ptr::addr_of_mut!((*psta).hash_list),
            core::ptr::addr_of_mut!(sp.sta_hash[index as usize]),
        );
        sp.asoc_sta_count += 1;
        return psta;
    }

    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        let mut irql: c_ulong = 0;
        kernel::rtw_rust_alloc_lock(stapriv, core::ptr::addr_of_mut!(irql));
        let mut psta = kernel::rtw_rust_alloc_pop_free(stapriv);
        if psta.is_null() {
            kernel::rtw_rust_alloc_unlock(stapriv, core::ptr::addr_of_mut!(irql));
            return core::ptr::null_mut();
        }
        _rtw_init_stainfo(psta);
        if kernel::rtw_rust_alloc_hash_attach(psta, stapriv, hwaddr) != _SUCCESS {
            psta = core::ptr::null_mut();
        } else {
            kernel::rtw_rust_alloc_post_init(psta, stapriv);
        }
        kernel::rtw_rust_alloc_unlock(stapriv, core::ptr::addr_of_mut!(irql));
        if !psta.is_null() {
            kernel::rtw_rust_alloc_mi_update(stapriv);
        }
        psta
    }
}
