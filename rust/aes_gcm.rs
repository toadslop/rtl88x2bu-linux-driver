// SPDX-License-Identifier: GPL-2.0
//! AES-GCM (part 1/2) — Rust port of `aes_gcm_ae` from `core/crypto/aes-gcm.c` (W2-07).
//!
//! `aes_gcm_ad` and `aes_gmac` remain in `core/crypto/aes-gcm_rest.c` until W2-08.

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
        pub fn wpa_hexdump_key(level: c_int, title: *const u8, buf: *const u8, len: usize);
    }
}

#[cfg(not(host_crypto_test))]
mod bindings {
    include!("bindings/generated.rs");

    extern "C" {
        pub fn wpa_hexdump_key(level: core::ffi::c_int, title: *const u8, buf: *const u8, len: usize);
    }
}

use bindings::{aes_encrypt, aes_encrypt_deinit, aes_encrypt_init, wpa_hexdump_key, AES_BLOCK_SIZE};
use types::AesKey;

#[cfg(host_crypto_test)]
use std::os::raw::{c_int, c_void};
#[cfg(not(host_crypto_test))]
use core::ffi::{c_int, c_void};

const MSG_EXCESSIVE: c_int = 0;

fn get_be32(block: &[u8; 16], off: usize) -> u32 {
    let b = &block[off..off + 4];
    ((b[0] as u32) << 24) | ((b[1] as u32) << 16) | ((b[2] as u32) << 8) | (b[3] as u32)
}

fn put_be32(block: &mut [u8; 16], off: usize, val: u32) {
    block[off] = (val >> 24) as u8;
    block[off + 1] = (val >> 16) as u8;
    block[off + 2] = (val >> 8) as u8;
    block[off + 3] = val as u8;
}

fn put_be64(buf: &mut [u8], off: usize, val: u64) {
    buf[off] = (val >> 56) as u8;
    buf[off + 1] = (val >> 48) as u8;
    buf[off + 2] = (val >> 40) as u8;
    buf[off + 3] = (val >> 32) as u8;
    buf[off + 4] = (val >> 24) as u8;
    buf[off + 5] = (val >> 16) as u8;
    buf[off + 6] = (val >> 8) as u8;
    buf[off + 7] = val as u8;
}

fn inc32(block: &mut [u8; 16]) {
    let mut val = get_be32(block, AES_BLOCK_SIZE as usize - 4);
    val = val.wrapping_add(1);
    put_be32(block, AES_BLOCK_SIZE as usize - 4, val);
}

fn xor_block(dst: &mut [u8; 16], src: &[u8; 16]) {
    for i in 0..16 {
        dst[i] ^= src[i];
    }
}

fn shift_right_block(v: &mut [u8; 16]) {
    let mut val = get_be32(v, 12);
    val >>= 1;
    if v[11] & 0x01 != 0 {
        val |= 0x8000_0000;
    }
    put_be32(v, 12, val);

    let mut val = get_be32(v, 8);
    val >>= 1;
    if v[7] & 0x01 != 0 {
        val |= 0x8000_0000;
    }
    put_be32(v, 8, val);

    let mut val = get_be32(v, 4);
    val >>= 1;
    if v[3] & 0x01 != 0 {
        val |= 0x8000_0000;
    }
    put_be32(v, 4, val);

    let mut val = get_be32(v, 0);
    val >>= 1;
    put_be32(v, 0, val);
}

fn gf_mult(x: &[u8; 16], y: &[u8; 16], z: &mut [u8; 16]) {
    let mut v = [0u8; 16];
    z.fill(0);
    v.copy_from_slice(y);

    for i in 0..16 {
        for j in 0..8 {
            if x[i] & (1u8 << (7 - j)) != 0 {
                xor_block(z, &v);
            }

            if v[15] & 0x01 != 0 {
                shift_right_block(&mut v);
                v[0] ^= 0xe1;
            } else {
                shift_right_block(&mut v);
            }
        }
    }
}

fn ghash_start(y: &mut [u8; 16]) {
    y.fill(0);
}

fn ghash(h: &[u8; 16], x: &[u8], y: &mut [u8; 16]) {
    let m = x.len() / 16;
    let mut xpos = 0usize;

    for _ in 0..m {
        let mut block = [0u8; 16];
        block.copy_from_slice(&x[xpos..xpos + 16]);
        xor_block(y, &block);
        let mut tmp = [0u8; 16];
        gf_mult(y, h, &mut tmp);
        *y = tmp;
        xpos += 16;
    }

    if xpos < x.len() {
        let last = x.len() - xpos;
        let mut tmp = [0u8; 16];
        tmp[..last].copy_from_slice(&x[xpos..]);
        xor_block(y, &tmp);
        let mut out = [0u8; 16];
        gf_mult(y, h, &mut out);
        *y = out;
    }
}

fn aes_gctr(aes: *mut c_void, icb: &[u8; 16], x: &[u8], y: &mut [u8]) {
    if x.is_empty() {
        return;
    }

    let n = x.len() / 16;
    let mut cb = *icb;
    let mut xpos = 0usize;
    let mut ypos = 0usize;

    for _ in 0..n {
        unsafe {
            aes_encrypt(aes, cb.as_ptr(), y[ypos..].as_mut_ptr());
        }
        for j in 0..16 {
            y[ypos + j] ^= x[xpos + j];
        }
        xpos += 16;
        ypos += 16;
        inc32(&mut cb);
    }

    let last = x.len() - xpos;
    if last != 0 {
        let mut tmp = [0u8; 16];
        unsafe {
            aes_encrypt(aes, cb.as_ptr(), tmp.as_mut_ptr());
        }
        for j in 0..last {
            y[ypos + j] = x[xpos + j] ^ tmp[j];
        }
    }
}

fn aes_gcm_init_hash_subkey(key: &[u8], h: &mut [u8; 16]) -> *mut c_void {
    let aes = unsafe { aes_encrypt_init(key.as_ptr(), key.len()) };
    if aes.is_null() {
        return core::ptr::null_mut();
    }

    h.fill(0);
    unsafe {
        aes_encrypt(aes, h.as_ptr(), h.as_mut_ptr());
        wpa_hexdump_key(
            MSG_EXCESSIVE,
            b"Hash subkey H for GHASH\0".as_ptr(),
            h.as_ptr(),
            AES_BLOCK_SIZE as usize,
        );
    }
    aes
}

fn aes_gcm_prepare_j0(iv: &[u8], h: &[u8; 16], j0: &mut [u8; 16]) {
    if iv.len() == 12 {
        j0[..iv.len()].copy_from_slice(iv);
        j0[iv.len()..AES_BLOCK_SIZE as usize].fill(0);
        j0[AES_BLOCK_SIZE as usize - 1] = 0x01;
    } else {
        let mut len_buf = [0u8; 16];
        ghash_start(j0);
        ghash(h, iv, j0);
        put_be64(&mut len_buf, 0, 0);
        put_be64(&mut len_buf, 8, (iv.len() as u64) * 8);
        ghash(h, &len_buf, j0);
    }
}

fn aes_gcm_gctr(aes: *mut c_void, j0: &[u8; 16], input: &[u8], out: &mut [u8]) {
    if input.is_empty() {
        return;
    }

    let mut j0inc = *j0;
    inc32(&mut j0inc);
    aes_gctr(aes, &j0inc, input, out);
}

fn aes_gcm_ghash(h: &[u8; 16], aad: &[u8], crypt: &[u8], s: &mut [u8; 16]) {
    let mut len_buf = [0u8; 16];
    ghash_start(s);
    ghash(h, aad, s);
    ghash(h, crypt, s);
    put_be64(&mut len_buf, 0, (aad.len() as u64) * 8);
    put_be64(&mut len_buf, 8, (crypt.len() as u64) * 8);
    ghash(h, &len_buf, s);

    unsafe {
        wpa_hexdump_key(
            MSG_EXCESSIVE,
            b"S = GHASH_H(...)\0".as_ptr(),
            s.as_ptr(),
            16,
        );
    }
}

/// Typed AES-GCM authenticated encryption (oracle: `aes_gcm_ae`).
pub fn aes_gcm_ae_typed(
    key: AesKey,
    iv: &[u8],
    plain: &[u8],
    aad: &[u8],
    crypt: &mut [u8],
    tag: &mut [u8; 16],
) -> Result<(), ()> {
    if crypt.len() < plain.len() {
        return Err(());
    }

    let mut h = [0u8; 16];
    let aes = aes_gcm_init_hash_subkey(key.as_bytes(), &mut h);
    if aes.is_null() {
        return Err(());
    }

    let mut j0 = [0u8; 16];
    aes_gcm_prepare_j0(iv, &h, &mut j0);

    aes_gcm_gctr(aes, &j0, plain, &mut crypt[..plain.len()]);
    let mut s = [0u8; 16];
    aes_gcm_ghash(&h, aad, &crypt[..plain.len()], &mut s);

    let mut tag_buf = [0u8; 16];
    aes_gctr(aes, &j0, &s, &mut tag_buf);
    *tag = tag_buf;

    unsafe { aes_encrypt_deinit(aes) };
    Ok(())
}

/// C ABI: `aes_gcm_ae` from `core/crypto/aes-gcm.c`.
#[no_mangle]
pub extern "C" fn aes_gcm_ae(
    key: *const u8,
    key_len: usize,
    iv: *const u8,
    iv_len: usize,
    plain: *const u8,
    plain_len: usize,
    aad: *const u8,
    aad_len: usize,
    crypt: *mut u8,
    tag: *mut u8,
) -> c_int {
    if key.is_null() || iv.is_null() || tag.is_null() {
        return -1;
    }
    if plain_len > 0 && (plain.is_null() || crypt.is_null()) {
        return -1;
    }
    if aad_len > 0 && aad.is_null() {
        return -1;
    }

    let aes_key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(key, key_len) }) {
        Ok(k) => k,
        Err(_) => return -1,
    };

    let iv_s = unsafe { core::slice::from_raw_parts(iv, iv_len) };
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
    let crypt_s = if plain_len == 0 {
        &mut [][..]
    } else {
        unsafe { core::slice::from_raw_parts_mut(crypt, plain_len) }
    };
    let mut tag_arr = [0u8; 16];

    let rc = match aes_gcm_ae_typed(aes_key, iv_s, plain_s, aad_s, crypt_s, &mut tag_arr) {
        Ok(()) => 0,
        Err(()) => -1,
    };
    if rc == 0 {
        unsafe {
            core::ptr::copy_nonoverlapping(tag_arr.as_ptr(), tag, 16);
        }
    }
    rc
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_aes_gcm_probe() -> c_int {
    AES_BLOCK_SIZE as c_int
}
