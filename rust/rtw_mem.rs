// SPDX-License-Identifier: GPL-2.0
//! W3-114 mem premem buffer helpers — Rust port (host L2 scope).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_mem_premem_test)]
use std::os::raw::c_int;

#[cfg(host_mem_premem_test)]
const HOST_MEM_PREMEM_BUF_SZ: u16 = 15360;
#[cfg(host_mem_premem_test)]
const HOST_MEM_PREMEM_NR_SKB: usize = 16;
#[cfg(host_mem_premem_test)]
const HOST_MEM_PREMEM_NR_BUF: usize = 8;

#[cfg(host_mem_premem_test)]
#[repr(C)]
#[derive(Copy, Clone)]
pub struct host_skb {
    pub tag: c_int,
}

#[cfg(host_mem_premem_test)]
struct PrememState {
    skb_pool: [host_skb; HOST_MEM_PREMEM_NR_SKB],
    q: [*mut host_skb; HOST_MEM_PREMEM_NR_SKB],
    q_len: usize,
    pool_used: usize,
    rtk_buf_mem: [*mut u8; HOST_MEM_PREMEM_NR_BUF],
    buf_slots: [u8; HOST_MEM_PREMEM_NR_BUF],
}

#[cfg(host_mem_premem_test)]
static mut G_PREMEM: PrememState = PrememState {
    skb_pool: [host_skb { tag: 0 }; HOST_MEM_PREMEM_NR_SKB],
    q: [std::ptr::null_mut(); HOST_MEM_PREMEM_NR_SKB],
    q_len: 0,
    pool_used: 0,
    rtk_buf_mem: [std::ptr::null_mut(); HOST_MEM_PREMEM_NR_BUF],
    buf_slots: [0; HOST_MEM_PREMEM_NR_BUF],
};

#[cfg(host_mem_premem_test)]
unsafe fn state() -> &'static mut PrememState {
    &mut *(&raw mut G_PREMEM)
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn host_mem_premem_reset() {
    unsafe {
        let s = state();
        s.q_len = 0;
        s.pool_used = 0;
        for i in 0..HOST_MEM_PREMEM_NR_BUF {
            s.buf_slots[i] = 0;
            s.rtk_buf_mem[i] = std::ptr::null_mut();
        }
    }
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn host_mem_premem_set_buf(index: c_int, tag: u8) {
    if index < 0 || index as usize >= HOST_MEM_PREMEM_NR_BUF {
        return;
    }
    unsafe {
        let s = state();
        let i = index as usize;
        s.buf_slots[i] = tag;
        s.rtk_buf_mem[i] = &mut s.buf_slots[i];
    }
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn host_mem_premem_seed_skb(tag: c_int) {
    unsafe {
        let s = state();
        if s.pool_used >= HOST_MEM_PREMEM_NR_SKB || s.q_len >= HOST_MEM_PREMEM_NR_SKB {
            return;
        }
        s.skb_pool[s.pool_used].tag = tag;
        s.q[s.q_len] = &mut s.skb_pool[s.pool_used];
        s.q_len += 1;
        s.pool_used += 1;
    }
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn host_mem_premem_queue_len() -> c_int {
    unsafe { state().q_len as c_int }
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn rtw_get_buf_premem(index: c_int) -> *mut u8 {
    if index < 0 || index as usize >= HOST_MEM_PREMEM_NR_BUF {
        return std::ptr::null_mut();
    }
    unsafe { state().rtk_buf_mem[index as usize] }
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn rtw_rtkm_get_buff_size() -> u16 {
    HOST_MEM_PREMEM_BUF_SZ
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn rtw_rtkm_get_nr_recv_skb() -> u8 {
    HOST_MEM_PREMEM_NR_SKB as u8
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn rtw_alloc_skb_premem(in_size: u16) -> *mut host_skb {
    if in_size > HOST_MEM_PREMEM_BUF_SZ {
        return std::ptr::null_mut();
    }
    unsafe {
        let s = state();
        if s.q_len == 0 {
            return std::ptr::null_mut();
        }
        let out = s.q[0];
        for i in 1..s.q_len {
            s.q[i - 1] = s.q[i];
        }
        s.q_len -= 1;
        out
    }
}

#[cfg(host_mem_premem_test)]
#[no_mangle]
pub extern "C" fn rtw_free_skb_premem(pskb: *mut host_skb) -> c_int {
    if pskb.is_null() {
        return -1;
    }
    unsafe {
        let s = state();
        if s.q_len >= HOST_MEM_PREMEM_NR_SKB {
            return -1;
        }
        s.q[s.q_len] = pskb;
        s.q_len += 1;
    }
    0
}
