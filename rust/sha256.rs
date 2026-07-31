// SPDX-License-Identifier: GPL-2.0
//! HMAC-SHA256 — Rust port of `core/crypto/sha256.c` (W2-16b).
//!
//! Uses `sha256_vector` from `sha256_internal.rs` at link time (same ABI as C).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_crypto_test)]
use std::os::raw::c_int;
#[cfg(not(host_crypto_test))]
use core::ffi::c_int;

const SHA256_MAC_LEN: usize = 32;
const IPAD: u8 = 0x36;
const OPAD: u8 = 0x5c;

#[cfg(host_crypto_test)]
mod bindings {
    use std::os::raw::c_uchar;

    pub type u8 = c_uchar;

    unsafe extern "C" {
        pub fn sha256_vector(
            num_elem: usize,
            addr: *const *const u8,
            len: *const usize,
            mac: *mut u8,
        ) -> i32;
    }
}

#[cfg(not(host_crypto_test))]
mod bindings {
    use core::ffi::c_int;

    unsafe extern "C" {
        pub fn sha256_vector(
            num_elem: usize,
            addr: *const *const u8,
            len: *const usize,
            mac: *mut u8,
        ) -> c_int;
    }
}

use bindings::sha256_vector;

/// Typed HMAC-SHA256 over a slice vector (oracle: `hmac_sha256_vector`).
pub fn hmac_sha256_vector_typed(
    key: &[u8],
    parts: &[&[u8]],
    mac: &mut [u8; SHA256_MAC_LEN],
) -> Result<(), ()> {
    if parts.len() > 5 {
        return Err(());
    }

    let mut tk = [0u8; SHA256_MAC_LEN];
    let key = if key.len() > 64 {
        let key_len = key.len();
        let rc = unsafe {
            sha256_vector(
                1,
                &key.as_ptr(),
                &key_len,
                tk.as_mut_ptr(),
            )
        };
        if rc < 0 {
            return Err(());
        }
        &tk[..SHA256_MAC_LEN]
    } else {
        key
    };

    let mut k_pad = [0u8; 64];
    k_pad[..key.len()].copy_from_slice(key);
    for b in k_pad.iter_mut() {
        *b ^= IPAD;
    }

    let inner_count = 1 + parts.len();
    let mut inner_addr: [*const u8; 6] = [core::ptr::null(); 6];
    let mut inner_len: [usize; 6] = [0; 6];
    inner_addr[0] = k_pad.as_ptr();
    inner_len[0] = 64;
    for (i, part) in parts.iter().enumerate() {
        inner_addr[i + 1] = part.as_ptr();
        inner_len[i + 1] = part.len();
    }
    let rc = unsafe {
        sha256_vector(
            inner_count,
            inner_addr.as_ptr(),
            inner_len.as_ptr(),
            mac.as_mut_ptr(),
        )
    };
    if rc < 0 {
        return Err(());
    }

    k_pad.fill(0);
    k_pad[..key.len()].copy_from_slice(key);
    for b in k_pad.iter_mut() {
        *b ^= OPAD;
    }

    let outer_addr = [k_pad.as_ptr(), mac.as_ptr()];
    let outer_len = [64usize, SHA256_MAC_LEN];
    let rc = unsafe {
        sha256_vector(
            2,
            outer_addr.as_ptr(),
            outer_len.as_ptr(),
            mac.as_mut_ptr(),
        )
    };
    if rc < 0 {
        return Err(());
    }
    Ok(())
}

/// C ABI: `hmac_sha256_vector` from `core/crypto/sha256.c`.
#[unsafe(no_mangle)]
pub extern "C" fn hmac_sha256_vector(
    key: *const u8,
    key_len: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    mac: *mut u8,
) -> c_int {
    if num_elem > 5 {
        return -1;
    }
    if mac.is_null() {
        return -1;
    }
    if key_len > 0 && key.is_null() {
        return -1;
    }
    if num_elem > 0 && (addr.is_null() || len.is_null()) {
        return -1;
    }

    let key_slice = if key_len == 0 {
        &[][..]
    } else {
        unsafe { core::slice::from_raw_parts(key, key_len) }
    };

    let mut parts_storage: [&[u8]; 5] = [&[], &[], &[], &[], &[]];
    for i in 0..num_elem {
        let inlen = unsafe { *len.add(i) };
        let ptr = unsafe { *addr.add(i) };
        if inlen > 0 && ptr.is_null() {
            return -1;
        }
        parts_storage[i] = if inlen == 0 {
            &[]
        } else {
            unsafe { core::slice::from_raw_parts(ptr, inlen) }
        };
    }

    let mac_buf = unsafe { &mut *(mac as *mut [u8; SHA256_MAC_LEN]) };
    match hmac_sha256_vector_typed(key_slice, &parts_storage[..num_elem], mac_buf) {
        Ok(()) => 0,
        Err(()) => -1,
    }
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[unsafe(no_mangle)]
pub extern "C" fn rtw_rust_sha256_probe() -> c_int {
    SHA256_MAC_LEN as c_int
}
