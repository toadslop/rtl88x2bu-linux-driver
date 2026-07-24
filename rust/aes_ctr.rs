// SPDX-License-Identifier: GPL-2.0
//! AES-128/192/256 CTR — Rust port of `core/crypto/aes-ctr.c` (W1-03).
//!
//! Typed logic uses domain types; `extern "C"` symbols preserve the C ABI for
//! remaining callers. AES block operations stay in C (`aes-internal*.c`).
//!
//! Domain types are included via `#[path]` because Kbuild compiles each `.rs`
//! as its own crate (same pattern as `domain_types.rs`). That duplicates type
//! code in `88x2bu.ko` for the pilot; consolidate into a shared crate or
//! `include!` only if binary size or drift becomes a concern in Wave 2+.

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
use types::{AesCtrNonce, AesKey};

#[cfg(host_crypto_test)]
use std::os::raw::c_int;
#[cfg(not(host_crypto_test))]
use core::ffi::c_int;

/// In-place AES-CTR XOR (oracle: `core/crypto/aes-ctr.c`).
pub fn aes_ctr_encrypt_typed(
    key: AesKey,
    nonce: AesCtrNonce,
    data: &mut [u8],
) -> Result<(), ()> {
    let block_size = AES_BLOCK_SIZE as usize;
    let ctx = unsafe { aes_encrypt_init(key.as_bytes().as_ptr(), key.key_len()) };
    if ctx.is_null() {
        return Err(());
    }

    let mut counter = *nonce.as_bytes();
    let mut pos = 0usize;
    let mut left = data.len();

    while left > 0 {
        let mut buf = [0u8; 16];
        let rc = unsafe { aes_encrypt(ctx, counter.as_ptr(), buf.as_mut_ptr()) };
        // C `aes-ctr.c` ignores `aes_encrypt()`'s return; we check for safety.
        // Today `aes_encrypt` always returns 0, so L2 parity holds.
        if rc != 0 {
            unsafe { aes_encrypt_deinit(ctx) };
            return Err(());
        }

        let len = if left < block_size { left } else { block_size };
        for j in 0..len {
            data[pos + j] ^= buf[j];
        }
        pos += len;
        left -= len;

        for i in (0..block_size).rev() {
            counter[i] = counter[i].wrapping_add(1);
            if counter[i] != 0 {
                break;
            }
        }
    }

    unsafe { aes_encrypt_deinit(ctx) };
    Ok(())
}

/// C ABI: `aes_ctr_encrypt` from `core/crypto/aes-ctr.c`.
#[no_mangle]
pub extern "C" fn aes_ctr_encrypt(
    key: *const u8,
    key_len: usize,
    nonce: *const u8,
    data: *mut u8,
    data_len: usize,
) -> c_int {
    let key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(key, key_len) }) {
        Ok(k) => k,
        Err(_) => return -1,
    };
    let nonce =
        match AesCtrNonce::try_from_slice(unsafe { core::slice::from_raw_parts(nonce, AesCtrNonce::SIZE) })
        {
            Ok(n) => n,
            Err(_) => return -1,
        };
    let data = unsafe { core::slice::from_raw_parts_mut(data, data_len) };

    match aes_ctr_encrypt_typed(key, nonce, data) {
        Ok(()) => 0,
        Err(()) => -1,
    }
}

/// C ABI: `aes_128_ctr_encrypt` from `core/crypto/aes-ctr.c`.
#[no_mangle]
pub extern "C" fn aes_128_ctr_encrypt(
    key: *const u8,
    nonce: *const u8,
    data: *mut u8,
    data_len: usize,
) -> c_int {
    aes_ctr_encrypt(key, 16, nonce, data, data_len)
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
/// Wire into `check-symbols.sh` when T1 lands.
#[no_mangle]
pub extern "C" fn rtw_rust_aes_ctr_probe() -> c_int {
    AES_BLOCK_SIZE as c_int
}
