// SPDX-License-Identifier: GPL-2.0
//! AES-CCM (Counter with CBC-MAC) — Rust port of `core/crypto/aes-ccm.c` (W2-04b).
//!
//! Fixed `L=2` and `aad_len <= 30` assumptions match the C oracle. AES block
//! ops remain in C (`aes-internal*.c` / bindgen).

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
use types::AesKey;

#[cfg(host_crypto_test)]
use std::os::raw::{c_int, c_void};
#[cfg(not(host_crypto_test))]
use core::ffi::{c_int, c_void};

const L: usize = 2;
const NONCE_LEN: usize = 15 - L; // 13

fn put_be16(dst: &mut [u8], val: u16) {
    dst[0] = (val >> 8) as u8;
    dst[1] = (val & 0xff) as u8;
}

fn xor_aes_block(dst: &mut [u8], src: &[u8]) {
    for i in 0..AES_BLOCK_SIZE as usize {
        dst[i] ^= src[i];
    }
}

fn aes_ccm_auth_start(
    aes: *mut c_void,
    m: usize,
    nonce: &[u8],
    aad: &[u8],
    plain_len: usize,
    x: &mut [u8; 16],
) {
    let mut aad_buf = [0u8; 2 * AES_BLOCK_SIZE as usize];
    let mut b = [0u8; AES_BLOCK_SIZE as usize];

    b[0] = if aad.is_empty() { 0 } else { 0x40 };
    // Match C `size_t` wrap for M<2 (frozen `ae-m-zero`); checked sub panics
    // under kernel CONFIG_RUST_OVERFLOW_CHECKS / -C overflow-checks=y.
    b[0] |= ((m.wrapping_sub(2) / 2) as u8) << 3;
    b[0] |= (L - 1) as u8;
    b[1..1 + NONCE_LEN].copy_from_slice(nonce);
    put_be16(&mut b[AES_BLOCK_SIZE as usize - L..], plain_len as u16);

    unsafe {
        aes_encrypt(aes, b.as_ptr(), x.as_mut_ptr());
    }

    if aad.is_empty() {
        return;
    }

    put_be16(&mut aad_buf[0..2], aad.len() as u16);
    aad_buf[2..2 + aad.len()].copy_from_slice(aad);

    xor_aes_block(&mut aad_buf[0..16], x);
    unsafe {
        aes_encrypt(aes, aad_buf.as_ptr(), x.as_mut_ptr());
    }

    if aad.len() > AES_BLOCK_SIZE as usize - 2 {
        xor_aes_block(&mut aad_buf[16..32], x);
        unsafe {
            aes_encrypt(aes, aad_buf[16..].as_ptr(), x.as_mut_ptr());
        }
    }
}

fn aes_ccm_auth(aes: *mut c_void, data: &[u8], x: &mut [u8; 16]) {
    let block = AES_BLOCK_SIZE as usize;
    let mut offset = 0usize;

    while offset + block <= data.len() {
        xor_aes_block(x, &data[offset..offset + block]);
        unsafe {
            aes_encrypt(aes, x.as_ptr(), x.as_mut_ptr());
        }
        offset += block;
    }

    let last = data.len() - offset;
    if last != 0 {
        for i in 0..last {
            x[i] ^= data[offset + i];
        }
        unsafe {
            aes_encrypt(aes, x.as_ptr(), x.as_mut_ptr());
        }
    }
}

fn aes_ccm_encr_start(nonce: &[u8], a: &mut [u8; 16]) {
    a.fill(0);
    a[0] = (L - 1) as u8;
    a[1..1 + NONCE_LEN].copy_from_slice(nonce);
}

fn aes_ccm_encr(aes: *mut c_void, input: &[u8], out: &mut [u8], a: &mut [u8; 16]) {
    let block = AES_BLOCK_SIZE as usize;
    let mut offset = 0usize;
    let mut i = 1usize;
    // C writes the AES keystream into `out` (callers pad by AES_BLOCK_SIZE).
    // Use a temp block so exact-sized host buffers stay in-bounds; XOR result
    // matches the oracle byte-for-byte.
    let mut s = [0u8; AES_BLOCK_SIZE as usize];

    while offset + block <= input.len() {
        put_be16(&mut a[block - 2..], i as u16);
        unsafe {
            aes_encrypt(aes, a.as_ptr(), s.as_mut_ptr());
        }
        for j in 0..block {
            out[offset + j] = s[j] ^ input[offset + j];
        }
        offset += block;
        i += 1;
    }

    let last = input.len() - offset;
    if last != 0 {
        put_be16(&mut a[block - 2..], i as u16);
        unsafe {
            aes_encrypt(aes, a.as_ptr(), s.as_mut_ptr());
        }
        for j in 0..last {
            out[offset + j] = s[j] ^ input[offset + j];
        }
    }
}

fn aes_ccm_encr_auth(aes: *mut c_void, m: usize, x: &[u8; 16], a: &mut [u8; 16], auth: &mut [u8]) {
    let mut tmp = [0u8; AES_BLOCK_SIZE as usize];

    put_be16(&mut a[AES_BLOCK_SIZE as usize - 2..], 0);
    unsafe {
        aes_encrypt(aes, a.as_ptr(), tmp.as_mut_ptr());
    }
    for i in 0..m {
        auth[i] = x[i] ^ tmp[i];
    }
}

fn aes_ccm_decr_auth(aes: *mut c_void, m: usize, a: &mut [u8; 16], auth: &[u8], t: &mut [u8; 16]) {
    let mut tmp = [0u8; AES_BLOCK_SIZE as usize];

    put_be16(&mut a[AES_BLOCK_SIZE as usize - 2..], 0);
    unsafe {
        aes_encrypt(aes, a.as_ptr(), tmp.as_mut_ptr());
    }
    for i in 0..m {
        t[i] = auth[i] ^ tmp[i];
    }
}

fn memcmp_const(a: &[u8], b: &[u8]) -> u8 {
    let mut res = 0u8;
    for i in 0..a.len() {
        res |= a[i] ^ b[i];
    }
    res
}

/// Typed AES-CCM authenticated encryption (oracle: `aes_ccm_ae`).
pub fn aes_ccm_ae_typed(
    key: AesKey,
    nonce: &[u8; NONCE_LEN],
    m: usize,
    plain: &[u8],
    aad: &[u8],
    crypt: &mut [u8],
    auth: &mut [u8],
) -> Result<(), ()> {
    if aad.len() > 30 || m > AES_BLOCK_SIZE as usize || crypt.len() < plain.len() || auth.len() < m
    {
        return Err(());
    }

    let aes = unsafe { aes_encrypt_init(key.as_bytes().as_ptr(), key.key_len()) };
    if aes.is_null() {
        return Err(());
    }

    let mut x = [0u8; AES_BLOCK_SIZE as usize];
    let mut a = [0u8; AES_BLOCK_SIZE as usize];

    aes_ccm_auth_start(aes, m, nonce, aad, plain.len(), &mut x);
    aes_ccm_auth(aes, plain, &mut x);
    aes_ccm_encr_start(nonce, &mut a);
    aes_ccm_encr(aes, plain, &mut crypt[..plain.len()], &mut a);
    aes_ccm_encr_auth(aes, m, &x, &mut a, auth);

    unsafe { aes_encrypt_deinit(aes) };
    Ok(())
}

/// Typed AES-CCM authenticated decryption (oracle: `aes_ccm_ad`).
pub fn aes_ccm_ad_typed(
    key: AesKey,
    nonce: &[u8; NONCE_LEN],
    m: usize,
    crypt: &[u8],
    aad: &[u8],
    auth: &[u8],
    plain: &mut [u8],
) -> Result<(), ()> {
    if aad.len() > 30
        || m > AES_BLOCK_SIZE as usize
        || auth.len() < m
        || plain.len() < crypt.len()
    {
        return Err(());
    }

    let aes = unsafe { aes_encrypt_init(key.as_bytes().as_ptr(), key.key_len()) };
    if aes.is_null() {
        return Err(());
    }

    let mut x = [0u8; AES_BLOCK_SIZE as usize];
    let mut a = [0u8; AES_BLOCK_SIZE as usize];
    let mut t = [0u8; AES_BLOCK_SIZE as usize];

    aes_ccm_encr_start(nonce, &mut a);
    aes_ccm_decr_auth(aes, m, &mut a, &auth[..m], &mut t);
    aes_ccm_encr(aes, crypt, &mut plain[..crypt.len()], &mut a);
    aes_ccm_auth_start(aes, m, nonce, aad, crypt.len(), &mut x);
    aes_ccm_auth(aes, &plain[..crypt.len()], &mut x);

    unsafe { aes_encrypt_deinit(aes) };

    if memcmp_const(&x[..m], &t[..m]) != 0 {
        return Err(());
    }
    Ok(())
}

/// C ABI: `aes_ccm_ae` from `core/crypto/aes-ccm.c`.
#[no_mangle]
pub extern "C" fn aes_ccm_ae(
    key: *const u8,
    key_len: usize,
    nonce: *const u8,
    m: usize,
    plain: *const u8,
    plain_len: usize,
    aad: *const u8,
    aad_len: usize,
    crypt: *mut u8,
    auth: *mut u8,
) -> c_int {
    // Match C reject checks before forming `auth`/`crypt` slices of length `m` /
    // `plain_len` (callers may pass short scratch buffers on the reject path).
    if aad_len > 30 || m > AES_BLOCK_SIZE as usize {
        return -1;
    }
    if key.is_null() || nonce.is_null() || crypt.is_null() || auth.is_null() {
        return -1;
    }
    if plain_len > 0 && plain.is_null() {
        return -1;
    }
    if aad_len > 0 && aad.is_null() {
        return -1;
    }

    let aes_key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(key, key_len) })
    {
        Ok(k) => k,
        Err(_) => return -1,
    };

    let mut nonce_buf = [0u8; NONCE_LEN];
    unsafe {
        core::ptr::copy_nonoverlapping(nonce, nonce_buf.as_mut_ptr(), NONCE_LEN);
    }

    let plain_s = if plain_len == 0 {
        &[][..]
    } else {
        unsafe { core::slice::from_raw_parts(plain, plain_len) }
    };
    let aad_s = if aad_len == 0 {
        &[][..]
    } else {
        unsafe { core::slice::from_raw_parts(aad, aad_len) }
    };
    let crypt_s = unsafe { core::slice::from_raw_parts_mut(crypt, plain_len) };
    let auth_s = unsafe { core::slice::from_raw_parts_mut(auth, m) };

    match aes_ccm_ae_typed(aes_key, &nonce_buf, m, plain_s, aad_s, crypt_s, auth_s) {
        Ok(()) => 0,
        Err(()) => -1,
    }
}

/// C ABI: `aes_ccm_ad` from `core/crypto/aes-ccm.c`.
#[no_mangle]
pub extern "C" fn aes_ccm_ad(
    key: *const u8,
    key_len: usize,
    nonce: *const u8,
    m: usize,
    crypt: *const u8,
    crypt_len: usize,
    aad: *const u8,
    aad_len: usize,
    auth: *const u8,
    plain: *mut u8,
) -> c_int {
    if aad_len > 30 || m > AES_BLOCK_SIZE as usize {
        return -1;
    }
    if key.is_null() || nonce.is_null() || auth.is_null() || plain.is_null() {
        return -1;
    }
    if crypt_len > 0 && crypt.is_null() {
        return -1;
    }
    if aad_len > 0 && aad.is_null() {
        return -1;
    }

    let aes_key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(key, key_len) })
    {
        Ok(k) => k,
        Err(_) => return -1,
    };

    let mut nonce_buf = [0u8; NONCE_LEN];
    unsafe {
        core::ptr::copy_nonoverlapping(nonce, nonce_buf.as_mut_ptr(), NONCE_LEN);
    }

    let crypt_s = if crypt_len == 0 {
        &[][..]
    } else {
        unsafe { core::slice::from_raw_parts(crypt, crypt_len) }
    };
    let aad_s = if aad_len == 0 {
        &[][..]
    } else {
        unsafe { core::slice::from_raw_parts(aad, aad_len) }
    };
    let auth_s = unsafe { core::slice::from_raw_parts(auth, m) };
    let plain_s = unsafe { core::slice::from_raw_parts_mut(plain, crypt_len) };

    match aes_ccm_ad_typed(aes_key, &nonce_buf, m, crypt_s, aad_s, auth_s, plain_s) {
        Ok(()) => 0,
        Err(()) => -1,
    }
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_aes_ccm_probe() -> c_int {
    AES_BLOCK_SIZE as c_int
}
