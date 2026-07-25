// SPDX-License-Identifier: GPL-2.0
//! CCMP (CTR with CBC-MAC Protocol) — Rust port of `core/crypto/ccmp.c` (W2-09/W2-10).
//!
//! Part 1 (W2-09): `ccmp_decrypt` + `ccmp_get_pn`; encrypt paths remain in
//! `core/crypto/ccmp_rest.c` until W2-10.

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

#[path = "ccmp_support.rs"]
mod support;

use support::bindings::{aes_ccm_ad, Adapter};
use support::{ccmp_aad_nonce, os_malloc, rtw_mfree, AES_BLOCK_SIZE, Ieee80211Hdr};
use types::AesKey;

const CCMP_MIC_LEN: usize = 8;
const CCMP_256_MIC_LEN: usize = 16;
const CCMP_KEY_LEN: usize = 16;
const CCMP_256_KEY_LEN: usize = 32;

fn ccmp_decrypt_inner(
    amsdu_mode: u8,
    tk: &AesKey,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    mic_len: usize,
    decrypted_len: &mut usize,
) -> *mut u8 {
    if data.len() < 8 + mic_len {
        return core::ptr::null_mut();
    }

    let alloc_len = data.len() + AES_BLOCK_SIZE as usize;
    let plain = os_malloc(alloc_len);
    if plain.is_null() {
        return core::ptr::null_mut();
    }

    let mlen = data.len() - 8 - mic_len;
    let crypt = &data[8..8 + mlen];
    let tag = &data[8 + mlen..];

    let mut aad = [0u8; 30];
    let mut nonce = [0u8; 13];
    let aad_len = ccmp_aad_nonce(amsdu_mode, hdr, data, &mut aad, &mut nonce);

    let key = tk.as_bytes();
    let rc = unsafe {
        aes_ccm_ad(
            key.as_ptr(),
            key.len(),
            nonce.as_ptr(),
            mic_len,
            crypt.as_ptr(),
            mlen,
            aad.as_ptr(),
            aad_len,
            tag.as_ptr(),
            plain as *mut u8,
        )
    };
    if rc < 0 {
        rtw_mfree(plain, alloc_len);
        return core::ptr::null_mut();
    }

    *decrypted_len = mlen;
    plain as *mut u8
}

fn ccmp_decrypt_128_inner(
    amsdu_mode: u8,
    tk: &AesKey,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    decrypted_len: &mut usize,
) -> *mut u8 {
    ccmp_decrypt_inner(amsdu_mode, tk, hdr, data, CCMP_MIC_LEN, decrypted_len)
}

/// C ABI: `ccmp_get_pn` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_get_pn(pn: *mut u8, data: *const u8) {
    if pn.is_null() || data.is_null() {
        return;
    }
    let data = unsafe { core::slice::from_raw_parts(data, 8) };
    let pn = unsafe { core::slice::from_raw_parts_mut(pn, 6) };
    pn[0] = data[7];
    pn[1] = data[6];
    pn[2] = data[5];
    pn[3] = data[4];
    pn[4] = data[1];
    pn[5] = data[0];
}

/// C ABI: `ccmp_decrypt` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_decrypt(
    padapter: *const Adapter,
    tk: *const u8,
    hdr: *const Ieee80211Hdr,
    data: *const u8,
    data_len: usize,
    decrypted_len: *mut usize,
) -> *mut u8 {
    if padapter.is_null() || tk.is_null() || hdr.is_null() || data.is_null()
        || decrypted_len.is_null()
    {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe {
        core::slice::from_raw_parts(tk, CCMP_KEY_LEN)
    }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let data_slice = unsafe { core::slice::from_raw_parts(data, data_len) };
    let hdr_ref = unsafe { &*hdr };
    let amsdu_mode = unsafe { support::bindings::rtw_registrypriv_amsdu_mode(padapter) };

    ccmp_decrypt_128_inner(
        amsdu_mode,
        &aes_key,
        hdr_ref,
        data_slice,
        unsafe { &mut *decrypted_len },
    )
}

/// C ABI: `ccmp_256_decrypt` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_256_decrypt(
    padapter: *const Adapter,
    tk: *const u8,
    hdr: *const Ieee80211Hdr,
    data: *const u8,
    data_len: usize,
    decrypted_len: *mut usize,
) -> *mut u8 {
    if padapter.is_null() || tk.is_null() || hdr.is_null() || data.is_null()
        || decrypted_len.is_null()
    {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe {
        core::slice::from_raw_parts(tk, CCMP_256_KEY_LEN)
    }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let data_slice = unsafe { core::slice::from_raw_parts(data, data_len) };
    let hdr_ref = unsafe { &*hdr };
    let amsdu_mode = unsafe { support::bindings::rtw_registrypriv_amsdu_mode(padapter) };

    ccmp_decrypt_inner(
        amsdu_mode,
        &aes_key,
        hdr_ref,
        data_slice,
        CCMP_256_MIC_LEN,
        unsafe { &mut *decrypted_len },
    )
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_ccmp_probe() -> core::ffi::c_int {
    AES_BLOCK_SIZE as core::ffi::c_int
}
