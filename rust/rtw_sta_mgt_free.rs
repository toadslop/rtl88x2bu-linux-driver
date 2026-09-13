// SPDX-License-Identifier: GPL-2.0
//! W3-79 sta priv / mfree — Rust port of `core/rtw_sta_mgt_free.c` (host L2, stacked).

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

#[cfg(host_sta_mgt_test)]
const ETH_ALEN: usize = 6;

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
    pub struct CmnStaInfo {
        pub aid: u16,
        pub mac_addr: [u8; ETH_ALEN],
    }

    #[repr(C)]
    pub struct StaInfo {
        pub cmn: CmnStaInfo,
        pub state: u32,
        pub lock: c_int,
        pub list: List,
        pub hash_list: List,
        pub padapter: *mut u8,
        pub sleep_q: Queue,
        pub mgmt_sleep_q: Queue,
        pub sta_xmitpriv: StaXmitPriv,
        pub sta_recvpriv: StaRecvPriv,
    }
}

#[cfg(host_sta_mgt_test)]
use host_layout::{StaInfo, StaRecvPriv, StaXmitPriv};

#[cfg(host_sta_mgt_test)]
unsafe fn spinlock_free(lock: *mut c_int) {
    *lock = 0;
}

#[cfg(host_sta_mgt_test)]
unsafe fn free_sta_xmit_priv_lock(xmit: *mut StaXmitPriv) {
    spinlock_free(core::ptr::addr_of_mut!((*xmit).lock));
    spinlock_free(core::ptr::addr_of_mut!((*xmit).be_q.sta_pending.lock));
    spinlock_free(core::ptr::addr_of_mut!((*xmit).bk_q.sta_pending.lock));
    spinlock_free(core::ptr::addr_of_mut!((*xmit).vi_q.sta_pending.lock));
    spinlock_free(core::ptr::addr_of_mut!((*xmit).vo_q.sta_pending.lock));
    spinlock_free(core::ptr::addr_of_mut!((*xmit).mgmt_q.sta_pending.lock));
}

#[cfg(host_sta_mgt_test)]
unsafe fn free_sta_recv_priv_lock(recv: *mut StaRecvPriv) {
    spinlock_free(core::ptr::addr_of_mut!((*recv).lock));
    spinlock_free(core::ptr::addr_of_mut!((*recv).defrag_q.lock));
}

#[no_mangle]
pub extern "C" fn rtw_mfree_stainfo(psta: *mut StaInfo) {
    #[cfg(host_sta_mgt_test)]
    unsafe {
        if psta.is_null() {
            return;
        }
        spinlock_free(core::ptr::addr_of_mut!((*psta).lock));
        free_sta_xmit_priv_lock(core::ptr::addr_of_mut!((*psta).sta_xmitpriv));
        free_sta_recv_priv_lock(core::ptr::addr_of_mut!((*psta).sta_recvpriv));
    }
}
