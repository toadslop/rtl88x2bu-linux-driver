// SPDX-License-Identifier: GPL-2.0
//! AES-OMAC1 (CMAC) — Rust port of `core/crypto/aes-omac1.c` (W2-01).
//!
//! Typed logic uses domain types; `extern "C"` symbols preserve the C ABI for
//! remaining callers. AES block operations stay in C (`aes-internal*.c`).

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
    use std::os::raw::{c_int, c_void};

    pub const AES_BLOCK_SIZE: u32 = 16;

    extern "C" {
        pub fn aes_encrypt_init(key: *const u8, len: usize) -> *mut c_void;
        pub fn aes_encrypt(ctx: *mut c_void, plain: *const u8, crypt: *mut u8) -> c_int;
        pub fn aes_encrypt_deinit(ctx: *mut c_void);
    }
}

#[cfg(not(host_crypto_test))]
mod bindings {
    include!("bindings/generated.rs");
}

use bindings::{aes_encrypt, aes_encrypt_deinit, aes_encrypt_init, AES_BLOCK_SIZE};
use types::{AesKey, AesMac};

#[cfg(host_crypto_test)]
use std::os::raw::c_int;
#[cfg(not(host_crypto_test))]
use core::ffi::c_int;

fn gf_mulx(pad: &mut [u8; 16]) {
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

/// OMAC1 over one or more message fragments (oracle: `omac1_aes_vector`).
pub fn omac1_aes_vector_typed(key: AesKey, fragments: &[&[u8]]) -> Result<AesMac, ()> {
    let block_size = AES_BLOCK_SIZE as usize;
    let ctx = unsafe { aes_encrypt_init(key.as_bytes().as_ptr(), key.key_len()) };
    if ctx.is_null() {
        return Err(());
    }

    let mut cbc = [0u8; 16];
    let mut pad = [0u8; 16];

    let total_len: usize = fragments.iter().map(|f| f.len()).sum();
    let mut left = total_len;

    let mut elem = 0usize;
    let mut pos = 0usize;
    let mut end = if fragments.is_empty() {
        0
    } else {
        fragments[0].len()
    };

    while left >= block_size {
        for i in 0..block_size {
            if fragments.is_empty() {
                break;
            }
            cbc[i] ^= fragments[elem][pos];
            pos += 1;
            if pos >= end {
                if i + 1 == block_size && left == block_size {
                    break;
                }
                elem += 1;
                if elem >= fragments.len() {
                    unsafe { aes_encrypt_deinit(ctx) };
                    return Err(());
                }
                pos = 0;
                end = fragments[elem].len();
            }
        }
        if left > block_size {
            let rc = unsafe { aes_encrypt(ctx, cbc.as_ptr(), cbc.as_mut_ptr()) };
            if rc != 0 {
                unsafe { aes_encrypt_deinit(ctx) };
                return Err(());
            }
        }
        left -= block_size;
    }

    pad.fill(0);
    let rc = unsafe { aes_encrypt(ctx, pad.as_ptr(), pad.as_mut_ptr()) };
    if rc != 0 {
        unsafe { aes_encrypt_deinit(ctx) };
        return Err(());
    }
    gf_mulx(&mut pad);

    if left != 0 || total_len == 0 {
        if !fragments.is_empty() {
            for i in 0..left {
                cbc[i] ^= fragments[elem][pos];
                pos += 1;
                if pos >= end {
                    if i + 1 == left {
                        break;
                    }
                    elem += 1;
                    if elem >= fragments.len() {
                        unsafe { aes_encrypt_deinit(ctx) };
                        return Err(());
                    }
                    pos = 0;
                    end = fragments[elem].len();
                }
            }
        }
        cbc[left] ^= 0x80;
        gf_mulx(&mut pad);
    }

    for i in 0..block_size {
        pad[i] ^= cbc[i];
    }

    let mut mac = [0u8; 16];
    let rc = unsafe { aes_encrypt(ctx, pad.as_ptr(), mac.as_mut_ptr()) };
    unsafe { aes_encrypt_deinit(ctx) };
    if rc != 0 {
        return Err(());
    }
    Ok(AesMac::from_bytes(mac))
}

/// C ABI: `omac1_aes_vector` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_vector(
    key: *const u8,
    key_len: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    mac: *mut u8,
) -> c_int {
    let aes_key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(key, key_len) }) {
        Ok(k) => k,
        Err(_) => return -1,
    };

    if num_elem == 0 || addr.is_null() || len.is_null() || mac.is_null() {
        return -1;
    }

    let mut fragments: [&[u8]; 32] = [&[]; 32];
    let n = num_elem.min(fragments.len());
    for i in 0..n {
        let slice_len = unsafe { *len.add(i) };
        let ptr = unsafe { *addr.add(i) };
        if slice_len > 0 && ptr.is_null() {
            return -1;
        }
        fragments[i] = unsafe { core::slice::from_raw_parts(ptr, slice_len) };
    }

    match omac1_aes_vector_typed(aes_key, &fragments[..n]) {
        Ok(out) => {
            unsafe {
                core::ptr::copy_nonoverlapping(out.as_bytes().as_ptr(), mac, AesMac::SIZE);
            }
            0
        }
        Err(()) => -1,
    }
}

/// C ABI: `omac1_aes_128_vector` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_128_vector(
    key: *const u8,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    mac: *mut u8,
) -> c_int {
    omac1_aes_vector(key, 16, num_elem, addr, len, mac)
}

/// C ABI: `omac1_aes_128` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_128(
    key: *const u8,
    data: *const u8,
    data_len: usize,
    mac: *mut u8,
) -> c_int {
    omac1_aes_vector(key, 16, 1, &data, &data_len, mac)
}

/// C ABI: `omac1_aes_256` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_256(
    key: *const u8,
    data: *const u8,
    data_len: usize,
    mac: *mut u8,
) -> c_int {
    omac1_aes_vector(key, 32, 1, &data, &data_len, mac)
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_aes_omac1_probe() -> c_int {
    AES_BLOCK_SIZE as c_int
}
