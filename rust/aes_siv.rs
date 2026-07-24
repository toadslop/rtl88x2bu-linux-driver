// SPDX-License-Identifier: GPL-2.0
//! AES-SIV (RFC 5297) — Rust port of `core/crypto/aes-siv.c` (W2-03b).
//!
//! S2V uses `omac1_aes_vector` (Rust, W2-01); payload encryption uses
//! `aes_ctr_encrypt` (Rust, W1-03). AES block ops remain in C.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[path = "domain/types.rs"]
mod types;

#[cfg(host_crypto_test)]
mod bindings {
    use std::os::raw::c_int;

    pub const AES_BLOCK_SIZE: u32 = 16;

    extern "C" {
        pub fn omac1_aes_vector(
            key: *const u8,
            key_len: usize,
            num_elem: usize,
            addr: *const *const u8,
            len: *const usize,
            mac: *mut u8,
        ) -> c_int;
        pub fn aes_ctr_encrypt(
            key: *const u8,
            key_len: usize,
            nonce: *const u8,
            data: *mut u8,
            data_len: usize,
        ) -> c_int;
        pub fn os_memdup(src: *const u8, sz: u32) -> *mut u8;
        pub fn bin_clear_free(bin: *mut u8, len: usize);
        pub fn os_memcmp(s1: *const u8, s2: *const u8, n: usize) -> c_int;
    }
}

#[cfg(not(host_crypto_test))]
mod bindings {
    include!("bindings/generated.rs");

    extern "C" {
        pub fn omac1_aes_vector(
            key: *const u8,
            key_len: usize,
            num_elem: usize,
            addr: *const *const u8,
            len: *const usize,
            mac: *mut u8,
        ) -> core::ffi::c_int;
        pub fn aes_ctr_encrypt(
            key: *const u8,
            key_len: usize,
            nonce: *const u8,
            data: *mut u8,
            data_len: usize,
        ) -> core::ffi::c_int;
        pub fn os_memdup(src: *const u8, sz: u32) -> *mut u8;
        pub fn bin_clear_free(bin: *mut u8, len: usize);
        pub fn os_memcmp(s1: *const u8, s2: *const u8, n: usize) -> core::ffi::c_int;
    }
}

use bindings::{
    aes_ctr_encrypt, bin_clear_free, omac1_aes_vector, os_memcmp, os_memdup, AES_BLOCK_SIZE,
};

#[cfg(host_crypto_test)]
use std::os::raw::c_int;
#[cfg(not(host_crypto_test))]
use core::ffi::c_int;

const ZERO_BLOCK: [u8; 16] = [0u8; 16];

fn dbl(pad: &mut [u8; 16]) {
    let carry = pad[0] & 0x80 != 0;
    let block_size = AES_BLOCK_SIZE as usize;
    for i in 0..block_size - 1 {
        pad[i] = (pad[i] << 1) | (pad[i + 1] >> 7);
    }
    pad[block_size - 1] <<= 1;
    if carry {
        pad[block_size - 1] ^= 0x87;
    }
}

fn xor_block(a: &mut [u8; 16], b: &[u8; 16]) {
    for i in 0..16 {
        a[i] ^= b[i];
    }
}

fn xorend(a: &mut [u8], alen: usize, b: &[u8]) {
    if alen < b.len() {
        return;
    }
    let start = alen - b.len();
    for (i, byte) in b.iter().enumerate() {
        a[start + i] ^= *byte;
    }
}

fn pad_block(addr: &[u8]) -> [u8; 16] {
    let mut pad = [0u8; 16];
    let len = addr.len().min(16);
    pad[..len].copy_from_slice(&addr[..len]);
    if addr.len() < 16 {
        pad[addr.len()] = 0x80;
    }
    pad
}

fn aes_s2v(
    key: *const u8,
    key_len: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    mac: *mut u8,
) -> c_int {
    let block_size = AES_BLOCK_SIZE as usize;
    let mut tmp = [0u8; 16];
    let mut tmp2 = [0u8; 16];

    if num_elem == 0 {
        tmp.copy_from_slice(&ZERO_BLOCK);
        tmp[block_size - 1] = 1;
        let data = tmp.as_ptr();
        let data_len = tmp.len();
        return unsafe {
            omac1_aes_vector(
                key,
                key_len,
                1,
                &data,
                &data_len,
                mac,
            )
        };
    }

    let zero = ZERO_BLOCK.as_ptr();
    let zero_len = ZERO_BLOCK.len();
    let mut ret = unsafe { omac1_aes_vector(key, key_len, 1, &zero, &zero_len, tmp.as_mut_ptr()) };
    if ret != 0 {
        return ret;
    }

    for i in 0..num_elem - 1 {
        let elem_len = unsafe { *len.add(i) };
        let elem_ptr = unsafe { *addr.add(i) };
        ret = unsafe {
            omac1_aes_vector(
                key,
                key_len,
                1,
                &elem_ptr,
                &elem_len,
                tmp2.as_mut_ptr(),
            )
        };
        if ret != 0 {
            return ret;
        }
        dbl(&mut tmp);
        xor_block(&mut tmp, &tmp2);
    }

    let i = num_elem - 1;
    let elem_len = unsafe { *len.add(i) };
    let elem_ptr = unsafe { *addr.add(i) };

    if elem_len >= block_size {
        let buf = unsafe { os_memdup(elem_ptr, elem_len as u32) };
        if buf.is_null() {
            return -12; // ENOMEM
        }
        let slice = unsafe { core::slice::from_raw_parts_mut(buf, elem_len) };
        xorend(slice, elem_len, &tmp);
        let data = slice.as_ptr();
        ret = unsafe { omac1_aes_vector(key, key_len, 1, &data, &elem_len, mac) };
        unsafe { bin_clear_free(buf, elem_len) };
        return ret;
    }

    dbl(&mut tmp);
    tmp2 = pad_block(unsafe { core::slice::from_raw_parts(elem_ptr, elem_len) });
    xor_block(&mut tmp, &tmp2);

    let data_len = tmp.len();
    let data = tmp.as_ptr();
    unsafe { omac1_aes_vector(key, key_len, 1, &data, &data_len, mac) }
}

/// C ABI: `aes_siv_encrypt` from `core/crypto/aes-siv.c`.
#[no_mangle]
pub extern "C" fn aes_siv_encrypt(
    key: *const u8,
    key_len: usize,
    pw: *const u8,
    pwlen: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    out: *mut u8,
) -> c_int {
    const MAX_ADDR: usize = 6;

    if num_elem > MAX_ADDR - 1 || (key_len != 32 && key_len != 48 && key_len != 64) {
        return -1;
    }

    let half_key = key_len / 2;
    let k1 = key;
    let k2 = unsafe { key.add(half_key) };

    let mut addrs: [*const u8; MAX_ADDR] = [core::ptr::null(); MAX_ADDR];
    let mut lens = [0usize; MAX_ADDR];

    for i in 0..num_elem {
        addrs[i] = unsafe { *addr.add(i) };
        lens[i] = unsafe { *len.add(i) };
    }
    addrs[num_elem] = pw;
    lens[num_elem] = pwlen;

    let mut v = [0u8; 16];
    if aes_s2v(k1, half_key, num_elem + 1, addrs.as_ptr(), lens.as_ptr(), v.as_mut_ptr()) != 0 {
        return -1;
    }

    unsafe {
        core::ptr::copy_nonoverlapping(v.as_ptr(), out, 16);
        core::ptr::copy_nonoverlapping(pw, out.add(16), pwlen);
    }

    v[8] &= 0x7f;
    v[12] &= 0x7f;

    if unsafe { aes_ctr_encrypt(k2, half_key, v.as_ptr(), out.add(16), pwlen) } != 0 {
        return -1;
    }
    0
}

/// C ABI: `aes_siv_decrypt` from `core/crypto/aes-siv.c`.
#[no_mangle]
pub extern "C" fn aes_siv_decrypt(
    key: *const u8,
    key_len: usize,
    iv_crypt: *const u8,
    iv_c_len: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    out: *mut u8,
) -> c_int {
    const MAX_ADDR: usize = 6;

    if iv_c_len < AES_BLOCK_SIZE as usize
        || num_elem > MAX_ADDR - 1
        || (key_len != 32 && key_len != 48 && key_len != 64)
    {
        return -1;
    }

    let crypt_len = iv_c_len - AES_BLOCK_SIZE as usize;
    let half_key = key_len / 2;
    let k1 = key;
    let k2 = unsafe { key.add(half_key) };

    let mut addrs: [*const u8; MAX_ADDR] = [core::ptr::null(); MAX_ADDR];
    let mut lens = [0usize; MAX_ADDR];

    for i in 0..num_elem {
        addrs[i] = unsafe { *addr.add(i) };
        lens[i] = unsafe { *len.add(i) };
    }
    addrs[num_elem] = out;
    lens[num_elem] = crypt_len;

    let mut iv = [0u8; 16];
    unsafe {
        core::ptr::copy_nonoverlapping(iv_crypt, iv.as_mut_ptr(), 16);
        core::ptr::copy_nonoverlapping(iv_crypt.add(16), out, crypt_len);
    }

    iv[8] &= 0x7f;
    iv[12] &= 0x7f;

    if unsafe { aes_ctr_encrypt(k2, half_key, iv.as_ptr(), out, crypt_len) } != 0 {
        return -1;
    }

    let mut check = [0u8; 16];
    if aes_s2v(k1, half_key, num_elem + 1, addrs.as_ptr(), lens.as_ptr(), check.as_mut_ptr()) != 0 {
        return -1;
    }

    if unsafe { os_memcmp(check.as_ptr(), iv_crypt, 16) } == 0 {
        0
    } else {
        -1
    }
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_aes_siv_probe() -> c_int {
    AES_BLOCK_SIZE as c_int
}
