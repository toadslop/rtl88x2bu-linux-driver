// SPDX-License-Identifier: GPL-2.0
//! AES block encrypt — Rust port of `core/crypto/aes-internal-enc.c` (W2-15).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

const AES_BLOCK_SIZE: usize = 16;
const AES_PRIV_SIZE: usize = 4 * 4 * 15 + 4;
const AES_PRIV_NR_POS: usize = 4 * 15;

extern "C" {
    static Te0: [u32; 256];
    fn rijndaelKeySetupEnc(rk: *mut u32, cipher_key: *const u8, key_bits: i32) -> i32;
}

#[cfg(host_crypto_test)]
mod alloc {
    use std::os::raw::c_void;

    mod bindings {
        use std::os::raw::c_void;

        extern "C" {
            pub fn os_malloc(sz: usize) -> *mut c_void;
            pub fn rtw_mfree(ptr: *mut c_void, sz: usize);
        }
    }

    pub fn os_malloc(sz: usize) -> *mut c_void {
        unsafe { bindings::os_malloc(sz) }
    }

    pub fn rtw_mfree(ptr: *mut c_void, sz: usize) {
        unsafe { bindings::rtw_mfree(ptr, sz) }
    }
}

#[cfg(not(host_crypto_test))]
mod alloc {
    use core::ffi::c_void;

    extern "C" {
        fn _rtw_malloc(sz: u32) -> *mut c_void;
        fn _rtw_mfree(ptr: *mut c_void, sz: u32);
    }

    pub fn os_malloc(sz: usize) -> *mut c_void {
        unsafe { _rtw_malloc(sz as u32) }
    }

    pub fn rtw_mfree(ptr: *mut c_void, sz: usize) {
        unsafe { _rtw_mfree(ptr, sz as u32) }
    }
}

use alloc::{os_malloc, rtw_mfree};

fn rotr(val: u32, bits: u32) -> u32 {
    (val >> bits) | (val << (32 - bits))
}

fn get_u32(pt: &[u8]) -> u32 {
    ((pt[0] as u32) << 24) ^ ((pt[1] as u32) << 16) ^ ((pt[2] as u32) << 8) ^ (pt[3] as u32)
}

fn put_u32(ct: &mut [u8], st: u32) {
    ct[0] = (st >> 24) as u8;
    ct[1] = (st >> 16) as u8;
    ct[2] = (st >> 8) as u8;
    ct[3] = st as u8;
}

fn te0(i: u32) -> u32 {
    unsafe { Te0[((i >> 24) & 0xff) as usize] }
}

fn te1(i: u32) -> u32 {
    rotr(unsafe { Te0[((i >> 16) & 0xff) as usize] }, 8)
}

fn te2(i: u32) -> u32 {
    rotr(unsafe { Te0[((i >> 8) & 0xff) as usize] }, 16)
}

fn te3(i: u32) -> u32 {
    rotr(unsafe { Te0[(i & 0xff) as usize] }, 24)
}

fn te41(i: u32) -> u32 {
    (unsafe { Te0[((i >> 24) & 0xff) as usize] } << 8) & 0xff000000
}

fn te42(i: u32) -> u32 {
    (unsafe { Te0[((i >> 16) & 0xff) as usize] }) & 0x00ff0000
}

fn te43(i: u32) -> u32 {
    (unsafe { Te0[((i >> 8) & 0xff) as usize] }) & 0x0000ff00
}

fn te44(i: u32) -> u32 {
    ((unsafe { Te0[(i & 0xff) as usize] }) >> 8) & 0x000000ff
}

fn round(d: &mut [u32; 4], s: &[u32; 4], rk: &[u32], off: usize) {
    d[0] = te0(s[0]) ^ te1(s[1]) ^ te2(s[2]) ^ te3(s[3]) ^ rk[off];
    d[1] = te0(s[1]) ^ te1(s[2]) ^ te2(s[3]) ^ te3(s[0]) ^ rk[off + 1];
    d[2] = te0(s[2]) ^ te1(s[3]) ^ te2(s[0]) ^ te3(s[1]) ^ rk[off + 2];
    d[3] = te0(s[3]) ^ te1(s[0]) ^ te2(s[1]) ^ te3(s[2]) ^ rk[off + 3];
}

fn rijndael_encrypt(rk: &[u32], nr: i32, pt: &[u8; 16], ct: &mut [u8; 16]) {
    let mut s = [
        get_u32(&pt[0..4]) ^ rk[0],
        get_u32(&pt[4..8]) ^ rk[1],
        get_u32(&pt[8..12]) ^ rk[2],
        get_u32(&pt[12..16]) ^ rk[3],
    ];
    let mut t = [0u32; 4];
    let mut rk_off = 4usize;
    let mut r = nr >> 1;

    loop {
        round(&mut t, &s, rk, rk_off);
        rk_off += 4;
        r -= 1;
        if r == 0 {
            break;
        }
        round(&mut s, &t, rk, rk_off);
        rk_off += 4;
    }

    let out0 = te41(t[0]) ^ te42(t[1]) ^ te43(t[2]) ^ te44(t[3]) ^ rk[rk_off];
    put_u32(&mut ct[0..4], out0);
    let out1 = te41(t[1]) ^ te42(t[2]) ^ te43(t[3]) ^ te44(t[0]) ^ rk[rk_off + 1];
    put_u32(&mut ct[4..8], out1);
    let out2 = te41(t[2]) ^ te42(t[3]) ^ te43(t[0]) ^ te44(t[1]) ^ rk[rk_off + 2];
    put_u32(&mut ct[8..12], out2);
    let out3 = te41(t[3]) ^ te42(t[0]) ^ te43(t[1]) ^ te44(t[2]) ^ rk[rk_off + 3];
    put_u32(&mut ct[12..16], out3);
}

/// C ABI: `aes_encrypt_init` from `core/crypto/aes-internal-enc.c`.
#[no_mangle]
pub extern "C" fn aes_encrypt_init(key: *const u8, len: usize) -> *mut core::ffi::c_void {
    if key.is_null() {
        return core::ptr::null_mut();
    }

    let rk = os_malloc(AES_PRIV_SIZE);
    if rk.is_null() {
        return core::ptr::null_mut();
    }

    let res = unsafe { rijndaelKeySetupEnc(rk as *mut u32, key, (len * 8) as i32) };
    if res < 0 {
        rtw_mfree(rk, AES_PRIV_SIZE);
        return core::ptr::null_mut();
    }

    unsafe {
        *((rk as *mut u32).add(AES_PRIV_NR_POS)) = res as u32;
    }
    rk
}

/// C ABI: `aes_encrypt` from `core/crypto/aes-internal-enc.c`.
#[no_mangle]
pub extern "C" fn aes_encrypt(
    ctx: *mut core::ffi::c_void,
    plain: *const u8,
    crypt: *mut u8,
) -> i32 {
    if ctx.is_null() || plain.is_null() || crypt.is_null() {
        return -1;
    }

    let rk = unsafe { core::slice::from_raw_parts(ctx as *const u32, 4 * 15 + 1) };
    let nr = rk[AES_PRIV_NR_POS] as i32;
    let pt = unsafe { &*(plain as *const [u8; 16]) };
    let ct = unsafe { &mut *(crypt as *mut [u8; 16]) };
    rijndael_encrypt(&rk[..4 * 15], nr, pt, ct);
    0
}

/// C ABI: `aes_encrypt_deinit` from `core/crypto/aes-internal-enc.c`.
#[no_mangle]
pub extern "C" fn aes_encrypt_deinit(ctx: *mut core::ffi::c_void) {
    if ctx.is_null() {
        return;
    }
    unsafe {
        core::ptr::write_bytes(ctx, 0, AES_PRIV_SIZE);
        rtw_mfree(ctx, AES_PRIV_SIZE);
    }
}

/// Link-time probe for L1.
#[no_mangle]
pub extern "C" fn rtw_rust_aes_internal_enc_probe() -> i32 {
    AES_BLOCK_SIZE as i32
}
